/*
TODO:
- Check everything in here. This only works in theory

Code created by Alonso Herreros Copete for local CanSat project
*/


#include <RH_RF95.h>
#include "NMEA_L.h" // Custom built for basic NMEA sentence building (local variant)

#define SERIAL_BAUD 19200 // For wired serial connection
#define RF95_FREQ 868.0 // For radio connection
#define RF95_POW 13 // Expected dBm

RH_RF95 rf95; // Will occupy pins 11, 12 and 13

char msg[NMEA_LEN] = ""; // Buffer for incoming data, should be able to hold NMEA sentences



void setup() {
    Serial.begin(SERIAL_BAUD); // We'll check with (Serial) to see if connection is set up
    bool rf95_status = rf95.init(); // Save the result for checking if it's initialized properly

    rf95.setFrequency(RF95_FREQ);
    rf95.setTxPower(RF95_POW);

    Serial.println(NMEA::txt(msg, (Serial) ? "[I] Serial Started" : "[W] Serial not working!"));
    Serial.println(NMEA::txt(msg, (rf95_status) ? "[I] RF95   Started" : "[W] RF95   unable to start!"));
}


void loop() {
    uint8_t buf[NMEA_LEN];
    if (rf95.available()) {
        if ( rf95.recv(buf, (uint8_t *)sizeof(buf)) ) {
            Serial.println((char *)buf);
    
            delay(200);

            /* Two-way communication to be tested - don't want it to hang here
            buf = "$ALACK,";
            rf95.send(buf, sizeof(buf));
            rf95.waitPacketSent();
            */
        }
    }
}
