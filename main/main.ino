/*
TODO: Implement DHT11 and GPS readings, packing data, and sending it
both over serial and radio
*/


#include <RH_RF95.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <SoftwareSerial.h>
#include "NMEA.h"


#define SERIAL_BAUD 19200 // For wired serial connection
#define GPS_RX 4 // Blue
#define GPS_TX 3 // Green
#define GPS_BAUD 9600
#define GPS_TIMEOUT 1200
#define FREQUENCY 868.0 // For radio connection
#define BMP_ADDR 0x76
#define BMP_OK 12
#define DHT_PIN A0
//define P_AT_SEA = 1013.25; // Should be updated to local forecast. Not in use (GPS)

RH_RF95 rf95; // Will occupy pins 11, 12 and 13
Adafruit_BMP280 bmp; // Will use I2C: A5 is SCL (orange), A4 is SDA (yellow)
DHT dht(DHT_PIN, DHT22); // DHT sensor for humidity
SoftwareSerial gps_ser(GPS_RX, GPS_TX); // SoftwareSerial with RX at 4 (blue), TX at 3 (green)

char msg[101] = ""; // Buffer for NMEA sentences


void (*resetBoard)(void) = 0;

void setup() {
    Serial.begin(SERIAL_BAUD);
    gps_ser.begin(GPS_BAUD);
    dht.begin(); // Returns void
    bool bmp_status = bmp.begin(BMP_ADDR);
    bool rf95_status = rf95.init();
    rf95.setFrequency(868);
    rf95.setTxPower(13);

    delay(1200);

    NMEA::txt(msg, (Serial) ?              "Serial Started" : "Serial not working!");
    Serial.println(msg);
    NMEA::txt(msg, (gps_ser.available()) ? "GPS    Started" : "GPS    not working!");
    Serial.println(msg);
    NMEA::txt(msg, (bmp_status) ?          "BMP280 Started" : "BMP280 unable to start!");
    Serial.println(msg);
    NMEA::txt(msg, (rf95_status) ?         "RF95   Started" : "RF95   unable to start!");
    Serial.println(msg);
};


void loop() {

    if (bmp.getStatus() == BMP_OK) {
        // Read the temperature, pressure and approximate altitude
        //float alt = bmp.readAltitude(P_AT_SEA); // We're getting altitude from GPS
        float pres = bmp.readPressure();
        float temp = bmp.readTemperature();
        NMEA::mda(msg, pres, temp);
    }
    else {
        // Try to start the BMP280 again if it isn't in OK state
        bmp.begin(BMP_ADDR);
        NMEA::txt(msg, "BMP280 unable to read!");
    }

    Serial.println(msg);
    
    while (gps_ser.available()) {
        gpsline(msg, sizeof(msg));
        Serial.println(msg);
        delay(50);
    }

    delay(1000);
}

char* gpsline(char *buf, size_t size) {
    int i = 0; // Index for the buffer

    for (unsigned long start = millis(); millis() - start < GPS_TIMEOUT;){ // Timeout
        while (gps_ser.available()) { // While there is data in the serial buffer
            char c = gps_ser.read(); // Get a character from the GPS serial
            switch (c) {
                case '\n': // EOL: terminate reading
                    goto eol;
                case '\r': // Don't keep the carriage return
                    break;
                case '$': // Start of a new sentence
                    i = 0;
                default: // Add the character to the buffer
                    if (i < size - 1) // If there is space in the buffer
                        buf[i++] = c;
            }
        }
    }
    eol:
    buf[i++] = '\0'; // Null-terminate the string
    return buf;
}