#include "NMEA.h"
#include <Arduino.h>


char* make_mda(char* out, float pres, float temp, float hum) {
    // Take atmospheric pressure in hPa, temperature in C and humidity in %
    // and build an NMEA (MDA) sentence to "out" (should be of length 82)
    // Return the pointer to "out"

    strcpy(out, "$ARMDA,");
    char buf[9];

    if(pres > 0) {
        dtostrf(pres / 3386.0, 6, 4, buf); // Pa to inHg
        strcat(out, buf);
        strcat(out, ",I,");
        dtostrf(pres / 100000.0, 6, 4, buf); // Pa to bar
        strcat(out, buf);
        strcat(out, ",B,");
    } else {
        strcat(out, ",,,,");
    }

    if(temp > 0) {
        dtostrf(temp, 6, 4, buf);
        strcat(out, buf);
        strcat(out, ",C,,,");
    } else {
        strcat(out, ",,,,");
    }

    strcat(out, ",,");

    if(hum > 0) {
        dtostrf(hum, 6, 4, buf);
        strcat(out, buf);
        strcat(out, ",");
    } else {
        strcat(out, ",");
    }

    strcat(out, ",,,,,,,,,,");

    strcat(out, "\r\n");
    return out;
}


char* make_txt(char* out, const char* txt) {
    // Take some text and build an NMEA (TXT) sentence to "out" (should be of length 82)
    // Return the pointer to "out"
    strcpy(out, "$ARTXT,");
    strcat(out, txt);
    strcat(out, "\r\n");
    return out;
}

