#include "NMEA_L.h"
#include <Arduino.h>


const char NMEA::id[3] = "AL";


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

int NMEA::writes(char *out, const char *str, const char unit[2]) {
    return sprintf(out, (unit[0] !='\0')? ",%s,%s":",%s", str, (str[0] !='\0')? unit : "");
};


char* NMEA::txt(char *out, const char* txt) {
    int off = 0;
    off += NMEA::startsen(out, "TXT"); // Sentence type
    off += NMEA::writeskip(out+off, 3); // Empty num_msg, msg_num, and msg_type
    off += NMEA::writes(out+off,txt); // Text
    return out;
}

