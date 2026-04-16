#include "comms_net_client.h"
#include "common/common.h"

// Single definition of the hardware serial connected to the A7680 GSM module.
// Declared as extern in common/common.h so all translation units can access it.
HardwareSerial ModemSerial(SYSTEM_GSM_RX_PIN, SYSTEM_GSM_TX_PIN);
