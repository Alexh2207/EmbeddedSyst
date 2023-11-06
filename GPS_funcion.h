
#ifndef GPS_H
#define GPS_H

#include "mbed.h"
#include <cstdint>
#include <cstdio>

typedef struct{
    char formattedTime[10];
   const char *latitude;
   const char *longitude;
} gps_t;

gps_t read_data();

#endif