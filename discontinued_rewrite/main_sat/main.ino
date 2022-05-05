/*
TODO:
- (!)Handle disconnection. BMP status checks hang if there's no device connected at all
- Check radio communication
- Implement some kind of handshake?
- Maybe switch to a lower level communication protocol (i.e. sending data as bytes)

Code created by Alonso Herreros Copete for local CanSat project
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
#define GPS_TIMEOUT 1100 // Timeout for gpsread function
#define BMP_ADDR 0x76 // Default address for BMP280
#define BMP_OK 12 // Code that .getStatus() returns when BMP280 is OK
#define DHT_PIN A0 // Goes to the data line on the DHT11
#define RF95_FREQ 868.0 // For radio connection
#define RF95_POW 13 // Expected dBm
//define P_AT_SEA = 1013.25; // Should be updated to local forecast. Not in use (GPS)

RH_RF95 rf95; // Will occupy pins 11, 12 and 13
Adafruit_BMP280 bmp; // Will use I2C: A5 is SCL (orange), A4 is SDA (yellow)
DHT dht(DHT_PIN, DHT22); // DHT sensor for humidity
SoftwareSerial gps_ser(GPS_RX, GPS_TX); // SoftwareSerial, check definitions for pins


bool gps_status;
bool bmp_status;
bool dht_status;
bool rf95_status;
char msg[NMEA_LEN] = ""; // Buffer for NMEA sentences


void (*restart)(void) = 0; // Call this function to reset the board ;)


void setup() {
    Serial.begin(SERIAL_BAUD); // We'll check with (Serial) to see if connection is set up
    gps_ser.begin(GPS_BAUD); // We'll check with .available() to see if GPS is sending data
    dht.begin(); // We'll check for NaN readings to see if it's working
    bmp.begin(BMP_ADDR); // Returns bool, but we'll use .getStatus() to see if it's working
    rf95_status = rf95.init(); // Save the result so we don't try sending if it's not working

    gps_status = (gpsread(msg) != NULL); // gpsread return NULL when it times out
    dht_status = !isnan(dht.readTemperature()); // readTemperature returns NaN if it's not working
    bmp_status = (bmp.getStatus() == BMP_OK); // This is what we'll use to check BMP status

    // (!) for warnings. May want to do something with them later
    send(NMEA::txt(msg, "Good morning! Delta River's CanSat on-board arduino speaking!"));
    send(NMEA::txt(msg, (Serial) ? "Serial started" : "(!) Serial not working!"));
    send(NMEA::txt(msg, (gps_status) ? "GPS getting data" : "(!) GPS can't get data!"));
    send(NMEA::txt(msg, (bmp_status) ? "BMP280 is OK" : "(!) BMP280 is not OK!"));
    send(NMEA::txt(msg, (dht_status) ? "DHT11 can read" : "(!) DHT11 can't read!"));
    send(NMEA::txt(msg, (rf95_status) ? "RF95 initialized" : "(!) RF95 failed to initalize!"));

    if (rf95_status) {
        rf95.setFrequency(RF95_FREQ);
        rf95.setTxPower(RF95_POW);
    }
};


void loop() {
    // If we get a 0x7F, wich is [DEL] in ascii, reset the board
    while (Serial.available()) {
        if (Serial.read() == 0x7F) {
            send(NMEA::txt(msg, "Got reset instruction. Resetting board..."));
            restart();
        }
    }


    /* BMP280 READING - transferred to Raspberry Pi
    if (bmp_status != (bmp.getStatus() == BMP_OK)) { // If status changed, update and notify
        bmp_status = !bmp_status; // This is a bit... eh. Should we just read the status again?
        send(NMEA::txt(msg, (bmp_status) ? "BMP280 is OK" : "(!) BMP280 is not OK!!"));
    }

    float pres = NAN, temp = NAN; //alt = NAN;
    if (bmp_status) { // If BMP280 is working, read data
        pres = bmp.readPressure(); // Pa
        temp = bmp.readTemperature(); // C
        //alt = bmp.readAltitude(P_AT_SEA); // m
    }*/


    // DHT11 READING - theoretical code (untested)
    if (dht_status != !isnan(dht.readHumidity())) { // If status changed, update and notify
        dht_status = !dht_status; // Again, this is't great. Should we update to the actual check?
        send(NMEA::txt(msg, (dht_status) ? "DHT11 can read" : "(!) DHT11 can't read!!"));
    }

    float hum = NAN;
    if (dht_status) // If the DHT11 is working, read data
        hum = dht.readHumidity(); // %


    // SEND DATA FROM BMP280 AND DHT11 - this worked before (half-tested)
    send(NMEA::mda(msg, pres, temp, 0, hum));
    

    // GPS READING AND RELAYING - this code works, and gpsread should too (mostly tested)
    if (gps_status != (gpsread(msg) != NULL)) { // If status changed, update and notify
        gps_status = !gps_status; // This again. Maybe we should update based on an actual check.
        NMEA::txt(msg, (gps_status) ? "GPS getting data" : "(!) GPS can't get data!!")
        if (!gps_status) // Only send it if it broke. If it's ok, it will send it on its own
            send(msg);
    }

    if (gps_status) { // If the GPS is working, read and relay data
        send(msg); // First message is already in the buffer from when we checked
        while (gps_ser.available()) // Read and relay all available data from GPS
            send(gpsread(msg));
    }

    delay(1000);
}


// Reads an available sentence from the GPS and writes it to buf.
// Returns NULL pointer if it times out or gets a (very) bad sentence.
// Will replace talker ID with "DR" for "Delta River"! :D
// buf is going to store an NMEA sentece so it must be NMEA_LEN long!!!
char* gpsread(char *buf) { // This should work like 90% sure (half-tested)
    start:
    uint8_t i = -1; // Index for buffer chars. Won't work until $ is found
    for (uint32_t start = millis(); millis() - start < GPS_TIMEOUT;) {
        while (gps_ser.available()) { // While there is data to be read
            char c = gps_ser.read(); // Get a character from the GPS serial
            switch (c) {
            case '\n': // EOL: Terminate reading
                buf[(i < NMEA_LEN)? i : 0] = '\0'; // Add null terminator
                buf[1] = 'D'; buf[2] = 'R'; // We're going to replace talker id with DR! :D
                return buf; // Return the buffer
            case '\r': // Discard the carriage returns
                continue;
            case '$': // Start of a sentence: now we can start writing to the buffer
                i = 0;
            default: // If there's space, add character to buffer
                if (i < NMEA_LEN - 1)
                    buf[i++] = c;
                else // If the sentence is longer than expected, that's a bad reading
                    goto start;
            }
        }
    }
    bad: // If we get here, we timed out or got something really wrong
    buf[0] = '\0'; // Make the given buffer a 0-length string
    return NULL; // Return null - this will be useful later
}


// Send the message over both serial and radio
void send(char *data) {
    strncpy(msg, data, NMEA_LEN); // Move the contents to our global buffer of known length
    
    Serial.println(msg); // Print it to serial for logging

    if (rf95_status) { // If radio is working, send it over radio too
        rf95.send((uint8_t *)msg, NMEA_LEN);
        rf95.waitPacketSent();
    }
}
