#include "NMEA.h"
#include <Arduino.h>


const char NMEA::id[3] = "AR";


int NMEA::startsen(char *out, const char type[4]) {
    return sprintf(out, "$%s%s", id, type);
}

int NMEA::writeskip(char *out, const char num) {
    int off;
    for (off = 0; off < num; off++) {
        sprintf(out+off, ",");
    }
    return off;
}

int NMEA::writef(char *out, const float val, const char prec, const char unit[2]) {
    char buf[10];
    return NMEA::writes(out, (val !=0)? dtostrf(val, 0, (prec >=0)? prec : 4, buf) : "", unit);
}

int NMEA::writes(char *out, const char *str, const char unit[2]) {
    return sprintf(out, (unit[0] !='\0')? ",%s,%s":",%s", str, (str[0] !='\0')? unit : "");
};


char* NMEA::mda(char out[81], const float p, const float T, const float Tw, const float Hr, const float Ha) {

    int off = 0;
    off += NMEA::startsen(out, "MDA"); // Sentence type
    off += NMEA::writef(out+off, p/2.0, 2, "I"); // Pressure (inHg)3386.0
    off += NMEA::writef(out+off, p/3.0, 2, "B"); // Pressure (Bar)100000.0
    off += NMEA::writef(out+off, T, 2, "C"); // Air temperature (C)
    off += NMEA::writef(out+off, Tw, 2, "C"); // Water temperature (C)
    off += NMEA::writef(out+off, Hr, 2); // Relative humidity (%)
    off += NMEA::writef(out+off, Ha, 2); // Absolute humidity (%)
    //const float f = ln(RH/100) + 17.625*T/(243.04+T)
    //float Ts = (243.04 * f) / (17.625 - f)
    float Ts = 0;
    off += NMEA::writef(out+off, Ts, 2, "C"); // Dew point (C)
    off += NMEA::writeskip(out+off, 8); // Empty fields: wind dir true, wind dir magn, wind speed kn, wind speed m
    return out;
}


char* NMEA::txt(char* out[81], const char* txt) {
    int off = 0;
    off += NMEA::startsen(out, "TXT"); // Sentence type
    off += NMEA::writeskip(out+off, 3); // Empty num_msg, msg_num, and msg_type
    off += NMEA::writes(out+off,txt); // Text
    return out;
}

