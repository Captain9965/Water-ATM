#!/usr/bin/env python3
"""
Eagopool OTA flash tool
========================
Sends a firmware .bin file to an Eagopool device via MQTT.

Usage:
    python ota_flash.py <IMEI> <firmware.bin>

Example:
    python ota_flash.py 867584038123456 firmware.bin

Requirements:
    pip install paho-mqtt
"""

import argparse
import json
import math
import sys
import time
import zlib

import paho.mqtt.client as mqtt

# ── Broker config (mirrors common/config.h) ──────────────────────────────────
MQTT_HOST     = "52.146.3.202"
MQTT_PORT     = 1883
MQTT_USERNAME = "eago"
MQTT_PASSWORD = "eago254"

# Topic prefixes:  device publishes to e/p/<IMEI>, server subscribes there.
#                  server publishes to e/s/<IMEI>, device subscribes there.
PUB_PREFIX = "w/s/"   # server → device
SUB_PREFIX = "w/p/"   # device → server

CHUNK_SIZE        = 256   # bytes per chunk (must match OTA_CHUNK_SIZE in firmware)
CHUNK_RETRY_S     = 10    # seconds to wait for ota_ack before retransmitting
MAX_CHUNK_RETRIES = 3     # retransmit up to this many times before aborting
START_TIMEOUT_S   = 60    # seconds to wait for ota_ready after ota_start
DONE_TIMEOUT_S    = 60    # seconds to wait for ota_done after last chunk
BUSY_RETRY_S      = 30    # seconds to wait before retrying ota_start when device is busy
MAX_BUSY_RETRIES  = 10    # give up after this many busy retries (~5 minutes)


# ── Helpers ───────────────────────────────────────────────────────────────────

def crc32(data: bytes) -> int:
    """IEEE 802.3 CRC32 — matches the firmware's compute_crc32() function."""
    return zlib.crc32(data) & 0xFFFFFFFF


def chunk_file(data: bytes, chunk_size: int):
    """Yield (index, chunk_bytes) pairs."""
    for i in range(0, len(data), chunk_size):
        yield i // chunk_size, data[i : i + chunk_size]


def progress_bar(current: int, total: int, width: int = 40) -> str:
    filled = int(width * current / total) if total else 0
    bar    = "█" * filled + "░" * (width - filled)
    pct    = 100 * current / total if total else 0
    return f"[{bar}] {pct:5.1f}%  {current}/{total} chunks"


# ── OTA state machine ─────────────────────────────────────────────────────────

class OTASession:
    def __init__(self, imei: str, firmware: bytes):
        self.imei      = imei
        self.firmware  = firmware
        self.fw_size   = len(firmware)
        self.fw_crc32  = crc32(firmware)
        self.total_chunks = math.ceil(self.fw_size / CHUNK_SIZE)

        self.pub_topic = PUB_PREFIX + imei   # server → device
        self.sub_topic = SUB_PREFIX + imei   # device → server

        self._client        = None
        self._state         = "idle"          # idle | waiting_ready | sending | waiting_done | done | error
        self._next_chunk    = 0
        self._chunk_size    = CHUNK_SIZE      # overridden by device-advertised cs in ota_ready
        self._chunk_retries = 0              # retransmission count for the current chunk
        self._busy_retries  = 0              # ota_start retries while device is gaming
        self._last_event    = None
        self._error_msg     = None
        self._send_deadline = None

    # ── MQTT callbacks ──────────────────────────────────────────────────────

    def _on_connect(self, client, userdata, flags, rc):
        if rc != 0:
            print(f"[ERROR] MQTT connect failed (rc={rc})")
            self._state = "error"
            return
        print(f"[OK]    Connected to {MQTT_HOST}:{MQTT_PORT}")
        client.subscribe(self.sub_topic)
        print(f"[OK]    Subscribed to {self.sub_topic}")
        self._send_ota_start()

    def _on_message(self, client, userdata, msg):
        try:
            payload = json.loads(msg.payload.decode())
        except Exception:
            print(f"[WARN]  Unparseable message: {msg.payload!r}")
            return

        ev = payload.get("ev", "")

        if ev == "ota_ready":
            device_cs   = int(payload.get("cs", self._chunk_size))
            resume_from = int(payload.get("next", 0))
            if self._state == "waiting_ready":
                # Honour the chunk size the device advertises — recalculate totals if needed.
                if device_cs != self._chunk_size:
                    print(f"[INFO]  Device chunk size={device_cs} (expected {self._chunk_size}) — adjusting")
                    self._chunk_size  = device_cs
                    self.total_chunks = math.ceil(self.fw_size / device_cs)
                if resume_from > 0:
                    print(f"[OK]    Device resuming from chunk {resume_from}/{self.total_chunks} "
                          f"({resume_from * self._chunk_size} / {self.fw_size} bytes already written)")
                    self._next_chunk = resume_from
                else:
                    print(f"[OK]    Device ready (chunk_size={device_cs}) — fresh start")
                self._state = "sending"
                self._send_next_chunk()

        elif ev == "ota_ack":
            idx = payload.get("idx", -1)
            if self._state == "sending" and idx == self._next_chunk - 1:
                self._chunk_retries = 0   # ack received — reset retry counter
                self._send_next_chunk()
            else:
                print(f"[WARN]  Unexpected ota_ack idx={idx} (expected {self._next_chunk - 1})")

        elif ev == "ota_done":
            print("\n[OK]    Device confirmed firmware applied — it will restart now.")
            self._state = "done"

        elif ev == "ota_err":
            msg_str = payload.get("msg", "unknown")
            if msg_str == "seq_err" and self._chunk_retries > 0:
                # Device already wrote this chunk; our retransmission was a
                # duplicate.  The original ota_ack was lost in transit.
                # Just advance to the next chunk as if the ack had arrived.
                print(f"\n[INFO]  seq_err on retry — ota_ack was lost, advancing.")
                self._chunk_retries = 0
                self._send_next_chunk()
            elif msg_str == "busy":
                # Device is mid-game — retry ota_start after a delay.
                self._busy_retries += 1
                if self._busy_retries > MAX_BUSY_RETRIES:
                    print(f"\n[ERROR] Device still busy after {MAX_BUSY_RETRIES} retries — giving up.")
                    self._state = "error"
                    self._error_msg = "busy"
                else:
                    print(f"\n[INFO]  Device busy (gaming) — retry {self._busy_retries}/{MAX_BUSY_RETRIES} "
                          f"in {BUSY_RETRY_S}s...")
                    self._state         = "waiting_busy"
                    self._send_deadline = time.time() + BUSY_RETRY_S
            else:
                print(f"\n[ERROR] Device reported error: {msg_str}")
                self._state = "error"
                self._error_msg = msg_str

        else:
            # A 'check' heartbeat from the new firmware while waiting for
            # ota_done means the device rebooted successfully — ota_done was
            # sent at QoS 0 and lost in transit before the reset completed.
            if ev == "check" and self._state == "waiting_done":
                print("\n[OK]    Device rebooted into new firmware "
                      "(ota_done lost in transit — confirmed via check heartbeat).")
                self._state = "done"
            else:
                # Log other device events (check, grec, …) without acting on them
                print(f"[INFO]  Device event: {payload}")

    def _on_disconnect(self, client, userdata, rc):
        if self._state not in ("done", "error"):
            print(f"\n[ERROR] Disconnected unexpectedly (rc={rc})")
            self._state = "error"

    # ── Send helpers ────────────────────────────────────────────────────────

    def _publish(self, payload: dict):
        self._client.publish(self.pub_topic, json.dumps(payload))

    def _send_ota_start(self):
        print(f"\n[INFO]  Firmware: {self.fw_size} bytes, "
              f"{self.total_chunks} chunks, CRC32=0x{self.fw_crc32:08X}")
        print(f"[INFO]  Publishing ota_start → {self.pub_topic}")
        self._publish({
            "ev":     "ota_start",
            "size":   self.fw_size,
            "chunks": self.total_chunks,
            "crc32":  self.fw_crc32,
        })
        self._state         = "waiting_ready"
        self._send_deadline = time.time() + START_TIMEOUT_S

    def _send_next_chunk(self):
        if self._next_chunk >= self.total_chunks:
            # All chunks sent — wait for ota_done
            self._state         = "waiting_done"
            self._send_deadline = time.time() + DONE_TIMEOUT_S
            return

        idx   = self._next_chunk
        start = idx * self._chunk_size
        chunk = self.firmware[start : start + self._chunk_size]

        self._publish({
            "ev":   "ota_chunk",
            "idx":  idx,
            "data": chunk.hex(),
        })
        self._next_chunk += 1

        # After the last chunk the device sends ota_done (not ota_ack),
        # so switch to waiting_done immediately with the longer timeout.
        if self._next_chunk >= self.total_chunks:
            self._state         = "waiting_done"
            self._send_deadline = time.time() + DONE_TIMEOUT_S
        else:
            self._send_deadline = time.time() + CHUNK_RETRY_S

        # Print progress on the same line
        bar = progress_bar(self._next_chunk, self.total_chunks)
        print(f"\r{bar}", end="", flush=True)

    # ── Main loop ───────────────────────────────────────────────────────────

    def run(self):
        client = mqtt.Client(client_id=f"ota-tool-{self.imei}")
        client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
        client.on_connect    = self._on_connect
        client.on_message    = self._on_message
        client.on_disconnect = self._on_disconnect
        self._client = client

        print(f"[INFO]  Connecting to {MQTT_HOST}:{MQTT_PORT}  (IMEI={self.imei})")
        client.connect(MQTT_HOST, MQTT_PORT, keepalive=60)
        client.loop_start()

        try:
            while self._state not in ("done", "error"):
                time.sleep(0.05)

                # Timeout / retry check
                if self._send_deadline and time.time() > self._send_deadline:
                    if self._state == "waiting_busy":
                        # Delay elapsed — retry ota_start
                        self._send_deadline = None
                        self._send_ota_start()
                    elif self._state == "sending" and self._chunk_retries < MAX_CHUNK_RETRIES:
                        self._chunk_retries += 1
                        retry_idx = self._next_chunk - 1
                        print(f"\n[WARN]  No ack for chunk {retry_idx}, "
                              f"retransmitting (attempt {self._chunk_retries}/{MAX_CHUNK_RETRIES})...")
                        self._next_chunk -= 1   # back up so _send_next_chunk resends it
                        self._send_next_chunk()
                    else:
                        print(f"\n[ERROR] Timeout in state '{self._state}'")
                        self._publish({"ev": "ota_abort"})
                        self._state = "error"
                        self._error_msg = "timeout"
                        break
        finally:
            client.loop_stop()
            client.disconnect()

        return self._state == "done"


# ── Entry point ───────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(
        description="Eagopool MQTT OTA flash tool",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument("imei",     help="Device IMEI (15 digits)")
    parser.add_argument("firmware", help="Path to firmware .bin file")
    args = parser.parse_args()

    if len(args.imei) != 15 or not args.imei.isdigit():
        parser.error(f"IMEI must be exactly 15 digits, got: {args.imei!r}")

    try:
        with open(args.firmware, "rb") as f:
            firmware = f.read()
    except FileNotFoundError:
        parser.error(f"Firmware file not found: {args.firmware}")

    if len(firmware) == 0:
        parser.error("Firmware file is empty")

    if len(firmware) > 128 * 1024:
        parser.error(f"Firmware too large: {len(firmware)} bytes (max 131072)")

    session = OTASession(imei=args.imei, firmware=firmware)
    success = session.run()

    if success:
        print("[OK]    OTA complete.")
        sys.exit(0)
    else:
        print(f"[FAIL]  OTA failed: {session._error_msg or 'unknown error'}")
        sys.exit(1)


if __name__ == "__main__":
    main()
