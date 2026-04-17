#pragma once
#include <stdint.h>

// ─── APN candidate list ────────────────────────────────────────────────────
//
// Entries are tried in order during NET_OPEN.  Once one succeeds the index is
// kept so that reconnects skip straight back to the working APN.  The index
// resets to 0 on a modem power cycle (powerOn()) or after the list is
// exhausted, so the search always restarts cleanly.
//
// user / pass are stored for completeness; most African ISP APNs do not need
// AT+CGAUTH authentication and it is not currently sent — add it to the
// NET_OPEN flow if a carrier requires it.

struct APNEntry {
    const char* apn;
    const char* user;
    const char* pass;
};

static const APNEntry APN_LIST[] = {
    { "iot.safaricom.com",          "none", "none" },  // preferred / default
    { "iot.safaricom",              "none", "none" },
    { "internet",                   "none", "none" },
    { "safaricom",                  "saf",  "data" },
    { "internet.ng.airtel.com",     "none", "none" },
    { "web.gprs.mtnnigeria.net",    "web",  "web"  },
    { "orangecmgprs",               "orange","orange"},
    { "orange",                     "none", "none" },
    { "vodanet",                    "none", "none" },
    { "ltedefault.vodacom.za",      "none", "none" },
    { "internet.vodafone.net",      "none", "none" },
    { "internet.mascom",            "none", "none" },
    { "internet.unitel.co.ao",      "none", "none" },
    { "djezzy.internet",            "none", "none" },
    { "moov",                       "none", "none" },
    { "econet.net",                 "none", "none" },
    { "etisalat",                   "none", "none" },
    { "rain",                       "none", "none" },
    { "hologram",                   "none", "none" },
    { "iot.1nce.net",               "none", "none" },
    { "iot.aer.net",                "none", "none" },
    { "soracom.io",                 "none", "none" },
    { "onomondo",                   "none", "none" },
    { "wireless.twilio.com",        "none", "none" },
};

static const uint8_t APN_LIST_SIZE = sizeof(APN_LIST) / sizeof(APN_LIST[0]);
