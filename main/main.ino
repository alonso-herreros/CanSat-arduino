/*
TODO: Implement DHT11 and GPS readings, packing data, and sending it
both over serial and radio
*/


#include <Adafruit_BMP280.h>
#include "NMEA.h"


#define SERIAL_BAUD 115200
#define BMP_ADDR 0x76


Adafruit_BMP280 bmp; // Using I2C connection

char msg[81] = ""; // Buffer for NMEA sentences

//float p_at_sea_level = 1013.25; // Should be updated to local forecast. Not in use (GPS)


void (*resetBoard)(void) = 0;

void setup() {
    Serial.begin(SERIAL_BAUD);

    NMEA::txt(msg, (!bmp.begin(BMP_ADDR)) ? "BMP280 Unable to start!" : "BMP280 Started" );
    Serial.println(msg);
};


void loop() {
    // Try to start the BMP280 again if it isn't in OK state
    if (bmp.getStatus() != 243) { bmp.begin(BMP_ADDR); }

    // Read the temperature, pressure and approximate altitude
    float temp = bmp.readTemperature();
    float pres = bmp.readPressure();
    //float alt = bmp.readAltitude(p_at_sea_level); // We're getting altitude from GPS

    Serial.println(NMEA::mda(msg, pres, temp));
    delay(1000);
}
