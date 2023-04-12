#include "main.h"

void debug_init(int * error){
    SerialDebug.setRx(SYSTEM_SERIAL_DEBUG_RX);
    SerialDebug.setTx(SYSTEM_SERIAL_DEBUG_TX);
    SerialDebug.begin(SYSTEM_DEBUG_BAUDRATE);
    delay(100);
    /* delay for 100 ms*/
    if (!SerialDebug){
        *error = -1;
    }
    return;
}