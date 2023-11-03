#include "mbed.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "GPS_funcion.h"

BufferedSerial gps(PA_9, PA_10,9600);
char gpsData[128];

gps_t processNMEASentence(const char* sentence) {
    // Analiza el mensaje NMEA y extrae la información de latitud, longitud y tiempo
    gps_t data;
    
    if (strstr(sentence, "$GPGGA") != NULL) {
        char *token = strtok((char*)sentence, ",");
        int count = 1;
        char *time;

        
        while (token != NULL) {
            if (count == 2) {
                time = token;
            } else if (count == 3) {
                data.latitude = token;
            } else if (count == 4) {
                // Hemisferio latitud (N o S)
                char hemisphereLat = *token;
            } else if (count == 5) {
                data.longitude = token;
            } else if (count == 6) {
                // Hemisferio longitud (E o W)
                char hemisphereLon = *token;
            }
            token = strtok(NULL, ",");
            count++;
        }

        // regular
        data.formattedTime[0] = time[0];
        data.formattedTime[1] = time[1];
        data.formattedTime[2] = ':';
        data.formattedTime[3] = time[2];
        data.formattedTime[4] = time[3];
        data.formattedTime[5] = ':';
        data.formattedTime[6] = time[4];
        data.formattedTime[7] = time[5];
        data.formattedTime[8] = '\0';

        //  la latitud y la longitud 
        // Puedes convertir las coordenadas de grados, minutos y segundos a grados decimales si es necesario
        printf("Tiempo: %s, Latitud: %s, Longitud: %s\n", data.formattedTime, data.latitude, data.longitude);
    }

    return data;
    // Puedes agregar más condiciones para procesar otros tipos de mensajes NMEA (como GPRMC) si es necesario
}

gps_t read_data(){
    int index = 0;
    char c;
    char gpsData[128];
    if (gps.read(&c, 1)) {
        if (c == '$') {
            index = 0;
            gpsData[index++] = c;
        } else if (c == '\n') {
            gpsData[index] = '\0';  // Agrega el carácter nulo al final del mensaje
            return processNMEASentence(gpsData);
        } else if (index < sizeof(gpsData) - 1) {
            gpsData[index++] = c;
        }
    }

    gps_t gps_void;

    return gps_void;
}

