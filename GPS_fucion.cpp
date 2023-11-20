#include "mbed.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "GPS_funcion.h"

BufferedSerial gps(PA_9, PA_10,9600);
char gpsData[128];
 int index = 0;
 char Time[10];
char c;

void sendPMTKCommand(const char *command) {
    gps.write(command, strlen(command));
}
void configureGPSToGPGGAyGPRMC() {
    // Comando PMTK para emitir solo la sentencia GPGGA
    const char *command = "$PMTK314,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";
    sendPMTKCommand(command);
}

gps_t processNMEASentence(const char* sentence) {
    gps_t data = { "", "", "", 0};  // Inicialización por defecto

    if (strstr(sentence, "GPGGA") != NULL) {
        char *token = strtok((char*)sentence, ",");
        int count = 0;
        data.ack = 1;

        while (token != NULL) {
            if (count == 1) {  // Tiempo
                strncpy(data.formattedTime, token, sizeof(data.formattedTime) - 1);
                data.formattedTime[sizeof(data.formattedTime) - 1] = '\0';
            } else if (count == 2) {  // Latitud
                strncpy(data.latitude, token, sizeof(data.latitude) - 1);
                data.latitude[sizeof(data.latitude) - 1] = '\0';
            } else if (count == 3) {  // Indicador N/S
                strncat(data.latitude, token, 1); // Añade N/S al final de la latitud
            } else if (count == 4) {  // Longitud
                strncpy(data.longitude, token, sizeof(data.longitude) - 1);
                data.longitude[sizeof(data.longitude) - 1] = '\0';
            } else if (count == 5) {  // Indicador E/W
                strncat(data.longitude, token, 1); // Añade E/W al final de la longitud
            }
            token = strtok(NULL, ",");
            count++;
        }
    }
    return data;
}

gps_t read_data() {
    gps_t gps_void = { "", "", "" };  // Inicialización por defecto

    while (gps.readable()) {
        if (gps.read(&c, 1)) {
            if (c == '$') {
                index = 0;
            } else if (c == '\n') {
                gpsData[index] = '\0';  // Termina la cadena
                return processNMEASentence(gpsData);
                printf("\nDatos GPS Crudos: %s\n", gpsData);  // Imprime la línea completa de datos GPS
            } else if (index < sizeof(gpsData) - 1) {
                gpsData[index++] = c;
            }
        }
    }

    return gps_void;
}

