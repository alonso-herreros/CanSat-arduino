#ifndef NMEA_BUILDER_H
#define NMEA_BUILDER_H

class NMEA {
    static const char id[3];

public:
    static char* mda(char *out, const float p, const float T, const float Tw=0, const float Hr=0, const float Ha=0);
    
    static char* txt(char *out, const char *txt);

private:
    static int startsen(char *out, const char type[4]="TXT");
    static int writeskip(char *out, const char num=1);
    static int writef(char *out, const float val=0, const char prec=-1, const char unit[2]="");
    static int writes(char *out, const char *str="", const char unit[2]="");
};

#endif