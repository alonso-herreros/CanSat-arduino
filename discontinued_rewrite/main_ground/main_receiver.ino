/*
This is the code that we tested and worked fine for the receiver.
TO DO:
- figure out two-way communication (something like req - ack)

Code created by Alonso Herreros Copete for CanSat group Delta River
*/


#include <SPI.h> // I don't know if we need this, but we'll keep it here until we check
#include <RH_RF95.h> // This is obviously necessary
#include "NMEA_L.h" // Custom built for basic NMEA sentence building (local variant)


#define RECV_LEN 50 // Must match the length of the message in the transmitter
#define SERIAL_BAUD 9600 // For wired serial connection
#define RF95_FREQ 868.0 // For radio connection
#define RF95_POW 13 // Expected dBm

RH_RF95 rf95; // Will occupy pins 11, 12 and 13 (or so I've heard...)

bool rf95_status; // We want to be able to use it in all scopes. Don't know how we'll update it though.
char msg[NMEA_LEN] = ""; // Buffer for printing out, should be able to hold NMEA sentences


void setup() {
    Serial.begin(SERIAL_BAUD); // We'll check with (Serial) to see if connection is set up
    rf95_status = rf95.init(); // Save the result for checking if it's initialized properly later

    rf95.setFrequency(RF95_FREQ);
    rf95.setTxPower(RF95_POW);

    Serial.println(NMEA::txt(msg, (Serial) ? "[I] Ground Serial started" : "[W] Ground Serial not working!"));
    Serial.println(NMEA::txt(msg, (rf95_status) ? "[I] Ground RF95 initialized" : "[W] Ground RF95 failed to initalize!"));
}


void loop() {
    uint8_t buf[RECV_LEN]; // Must match the length of the message in the transmitter
    uint8_t len = sizeof(buf); // Couldn't get .recv() to accept sizeof(buf)...
    for (int i = 0; i < RECV_LEN; i++) // Clear the buffer! .recv() doesn't add a null terminator!
        buf[i] = 0;

    if (rf95.available()) {
        if ( rf95.recv(buf, &len) ) { // I don't think this add the null terminator...
            Serial.println((char *)buf); // That's why we cleared the buffer before :)

            delay(100);

            // Two-way communication. How about sending back an ACK or something?
        }
        delay(100);
    }
    delay(100);
}
