/*
TO DO: integrate with the rest of the code to send the data to the server.
Connect SCL to pin A5 and SDA to pin A4. Power the BMP280 with 5V.
*/

// Don't run any of this is there's another main script.
#ifndef SERIAL_BAUD
#define SERIAL_BAUD 19200

// BMP280 pre-setup
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp; // Use I2C connection


#include "NMEA.h"
char msg[81] = ""; // Buffer for NMEA sentences


void setup() {
    Serial.begin(115200);
    start_bmp();
}


void loop() {
    if (!bmp.getStatus() == 243) {
        start_bmp();
    }
    Serial.println(read_to_nmea(msg));

    delay(1000);
}


bool start_bmp() {
    // Try to start the BMP280. Print the result (as NMEA text sentence)
    NMEA::txt(msg,
        (!bmp.begin(0x76)) ?
            "Could not find a valid BMP280 sensor!" :
            "BMP280 started successfully!"
    );
    Serial.println(msg);
}

#endif
