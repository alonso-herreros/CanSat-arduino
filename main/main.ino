/*
TODO: Implement DHT11 and GPS readings, packing data, and sending it
both over serial and radio
*/


#include <RH_RF95.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <SoftwareSerial.h>
#include "NMEA.h" // Custom built for basic NMEA sentence building


#define SERIAL_BAUD 19200 // For wired serial connection
#define GPS_RX 4 // Blue
#define GPS_TX 3 // Green
#define GPS_BAUD 9600 // Can't be changed. This is the module's set baudrate
#define GPS_TIMEOUT 700 // Timeout for gpsread function
#define RF95_FREQ 868.0 // For radio connection
#define RF95_POW 13 // Expected dBm
#define BMP_ADDR 0x76 // Default address for BMP280
#define BMP_OK 12 // Code that .getStatus() returns when BMP280 is OK
#define DHT_PIN A0 // Goes to the data line on the DHT11
//define P_AT_SEA = 1013.25; // Should be updated to local forecast. Not in use (GPS)

RH_RF95 rf95; // Will occupy pins 11, 12 and 13
Adafruit_BMP280 bmp; // Will use I2C: A5 is SCL (orange), A4 is SDA (yellow)
DHT dht(DHT_PIN, DHT22); // DHT sensor for humidity
SoftwareSerial gps_ser(GPS_RX, GPS_TX); // SoftwareSerial with RX at 4 (blue), TX at 3 (green)


bool rf95_status; // So we don't try communication if it's not working
char msg[NMEA_LEN] = ""; // Buffer for NMEA sentences


void (*resetBoard)(void) = 0; // Call this function to reset the board ;)


void setup() {
    Serial.begin(SERIAL_BAUD); // We'll check with (Serial) so see if connection is set up
    gps_ser.begin(GPS_BAUD); // We'll check with .available() to see if GPS is sending data
    dht.begin(); // Returns void
    bool bmp_status = bmp.begin(BMP_ADDR); // Returns bool, we'll use that
    rf95_status = rf95.init(); // Save the result so we don't try sending if it's not working

    rf95.setFrequency(RF95_FREQ);
    rf95.setTxPower(RF95_POW);

    delay(1200); // Wait to get some GPS data in buffer, so .available() may return something

    send(NMEA::txt(msg, (Serial) ? "Serial Started" : "Serial not working!"));
    send(NMEA::txt(msg, (gps_ser.available()) ? "GPS    Started" : "GPS    not working!"));
    // Add some check that dht is working??
    send(NMEA::txt(msg, (bmp_status) ? "BMP280 Started" : "BMP280 unable to start!"));
    send(NMEA::txt(msg, (rf95_status) ? "RF95   Started" : "RF95   unable to start!"));
};


void loop() {
    // BMP280 READING AND SENDING
    float pres = bmp.readPressure();
    float temp = bmp.readTemperature();
    //float alt = bmp.readAltitude(P_AT_SEA);

    if (pres!=pres || temp!=temp) { // If both values are NaN, then the sensor is not working
        bmp.begin(BMP_ADDR); // Try to restart the BMP280
        send(NMEA::txt(msg, "BMP280 unable to read! Trying to restart it."));
    }

    send(NMEA::mda(msg, pres, temp));
    

    // GPS READING AND RELAYING
    while (gps_ser.available()) //Read and relay all available data from GPS
        send(gpsread(msg));

    delay(1000);
}


// buf is going to store an NMEA sentece so it must be NMEA_LEN long!!!
char* gpsread(char *buf) {
    unsigned short i = -1; // Index for buffer chars. Won't work until $ is found

    for (unsigned long start = millis(); millis() - start < GPS_TIMEOUT;) {
        while (gps_ser.available()) { // While there is data to be read
            char c = gps_ser.read(); // Get a character from the GPS serial
            switch (c) {
            case '\n': // EOL: terminate reading
                goto end;
            case '\r': // Discard the carriage return
                continue;
            case '$': // Start of a new sentence: now we can start writing to the buffer
                i = 0;
            default: // If there's space, add character to buffer
                if (i < NMEA_LEN - 1)
                    buf[i++] = c;

                else {
                    // Sometimes we get lines that are bigger than the buffer.
                    // This happens when a new sentence starts being received before the end
                    // of the original line is reached, resulting in a line that is too long.
                    // This data isn't valid, so we'll discard it.
                    i = 0;
                    goto end;
                }
            }
        }
    }
    end:
    buf[i] = '\0'; // Null-terminate the string
    return buf;
}


// Send the message over both serial and radio
char* send(char *msg) {
    char buf[NMEA_LEN] = ""; // Move the contents to a clean ntca of known length, just in case
    strncpy(buf, msg, sizeof(buf)); 

    Serial.println(buf); // Print it to serial for logging

    if (rf95_status) { // If radio is working, send it over radio
        rf95.send((uint8_t *)buf, sizeof(buf));
        rf95.waitPacketSent();
    }

    return buf;
}
