/*
TO DO: integrate with the rest of the code to send the data to the server.
Connect SCL to pin A5 and SDA to pin A4. Power the BMP280 with 5V.
*/

// BMP280 pre-setup
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp; // Use I2C connection


#include "NMEA.h"
char msg[82] = "\0"; // Buffer for NMEA sentences


void setup() {
    Serial.begin(115200);
    start_bmp();
}


void loop() {
    char msg[82] = "\0";
    if (!bmp.getStatus() == 243) {
        start_bmp();
    }
    float temp = bmp.readTemperature();
    float pres = bmp.readPressure();
    float alt = bmp.readAltitude(1013.25); // Adjusted to local forecast

    make_mda(msg, pres, temp, 0);
    Serial.println(msg);

    delay(1000);
}


bool start_bmp() {
    // Try to start the BMP280. Print the result (as NMEA text sentence)
    char msg[82] = "\0";
    make_txt(msg,
        (!bmp.begin(0x76)) ?
            "Could not find a valid BMP280 sensor!" :
            "BMP280 started successfully!"
    );
    Serial.println(msg);
}
