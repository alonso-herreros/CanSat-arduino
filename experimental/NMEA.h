#ifndef NMEA_BUILDER_H
#define NMEA_BUILDER_H

char* make_mda(char* out, float pres, float temp, float hum);
char* make_txt(char* out, const char* txt);

#endif