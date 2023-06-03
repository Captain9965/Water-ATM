
#include "main.h"
#include <SPI.h>
#include "MFRC522.h"

MFRC522 mfrc522(SYSTEM_RFID_SS_PIN, SYSTEM_RFID_RESET_PIN);

long long debug_time = 0;


void setup() {
    int error = 0;
    DEBUG_INIT(&error);

    if (error < 0){
      while (1)
      {
        /* maybe reset mcu??*/
      }
    }
   
    DEBUG_INFO_LN("*****************Starting Application***************");
    SPI.begin();			// Init SPI bus
	  mfrc522.PCD_Init();

    pinMode(SYSTEM_RFID_SS_PIN, OUTPUT);
    digitalWrite(SYSTEM_RFID_SS_PIN, HIGH);

    delay(4);
    mfrc522.PCD_DumpVersionToSerial();
  
}

void check_rfid()
{
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	// Dump debug info about the card; PICC_HaltA() is automatically called
	mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

}
void loop() {
  check_rfid();
  if (millis() - debug_time > 2000){
    DEBUG_INFO_LN("Scanning");
    debug_time = millis();
  }
}

