#include "NMEA.h"
#include <Arduino.h>


const char NMEA::id[3] = "DR";


int NMEA::startsen(char *out, const char type[4]) { // Write ${id}{type} to "out"
    return sprintf(out, "$%s%s", id, type);
}

int NMEA::writeskip(char *out, const char num) { // Skip this many fields
    int off;
    for (off = 0; off < num; off++)
        sprintf(out+off, ",");
    return off;
}

int NMEA::writef(char *out, const float val, const char prec, const char unit[2]) {
    // Ok so since Arduino doesn't support %f we have to use dtostrf instead.
    // We also check for NaN and 0 values to filter out garbage data (skip the units if it's bad data)
    char buf[10];
    return NMEA::writes(out, (val!=0 and val==val)? dtostrf(val, 0, (prec >=0)? prec : 4, buf) : "", unit);
}

int NMEA::writes(char *out, const char *str, const char unit[2]) {
    // Write a string to the output buffer, and optionally add a unit
    // If it's an empty string, don't write anything at all (just the commas)
    return sprintf(out, (unit[0] !='\0')? ",%s,%s":",%s", str, (str[0] !='\0')? unit : "");
};


char* NMEA::mda(char *out, const float p, const float T, const float Tw, const float Hr) {
    // p in Pa, T in C, Tw in C, Hr in %

    int off = 0;
    off += NMEA::startsen(out, "MDA"); // Sentence type
    off += NMEA::writef(out+off, p/3386.0, 2, "I"); // Pressure (inHg)
    off += NMEA::writef(out+off, p/100000.0, 2, "B"); // Pressure (Bar)
    off += NMEA::writef(out+off, T, 2, "C"); // Air temperature (C)
    off += NMEA::writef(out+off, Tw, 2, "C"); // Water temperature (C)
    off += NMEA::writef(out+off, Hr, 2); // Relative humidity (%)
    off += NMEA::writeskip(out+off, 11); // Empty fields
    return out;
}


char* NMEA::txt(char *out, const char* txt) {
    int off = 0;
    off += NMEA::startsen(out, "TXT"); // Sentence type
    off += NMEA::writeskip(out+off, 3); // Empty num_msg, msg_num, and msg_type
    off += NMEA::writes(out+off,txt); // Text
    return out;
}

