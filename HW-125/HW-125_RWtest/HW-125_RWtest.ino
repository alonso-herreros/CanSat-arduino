// libraries for HW-125
#include <SPI.h>
#include <SD.h>


bool card_status;


void setup() {
  Serial.begin(9600);
  while ( !Serial ) delay(100);   // wait for serial
  Serial.println("Serial connection started. Setting up modules...");

  Serial.print("HW-125 SD card adapter: ");
  card_status = bool(SD.begin(10));
  Serial.print(card_status ? "Found... " : "Not found!");
  if (card_status) {
    File rwfile = SD.open("RWTEST.txt", FILE_WRITE);
    if (rwfile) {
      rwfile.println("rwtest");
      rwfile.close();
    }
    else {
      Serial.print("Write test failed!");
    }
    rwfile = SD.open("RWTEST.txt");
    if (rwfile) {
      while (rwfile.available()) {
        Serial.println();
        Serial.println(rwfile.read());
      }
      rwfile.close();
    }
    else {
      Serial.print("Read test failed!");
    }
    //bool card_status = (String(rwfile.read()) == "rwtest");
    Serial.print(card_status ? "Read-write passed... Success!" : "Read-write test failed!");
  }
  Serial.println();

}


void loop() {
  
  delay(2000);
}