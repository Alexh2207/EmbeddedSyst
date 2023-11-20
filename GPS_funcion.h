
#ifndef GPS_H
#define GPS_H

#include "mbed.h"
#include <cstdint>
#include <cstdio>


typedef struct {
    char formattedTime[10];
    char latitude[15];  // Tamaño ajustado para datos de latitud
    char longitude[15]; // Tamaño ajustado para datos de longitud
    int ack;
} gps_t;

void configureGPSToGPGGAyGPRMC();
gps_t read_data();

#endif
