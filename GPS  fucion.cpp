#include "mbed.h"



#include <cstdint>
#include <cstdio>
#include <cstring>








BufferedSerial gps(PA_9, PA_10);
char gpsData[128];







void processNMEASentence(const char* sentence) {
    // Analiza el mensaje NMEA y extrae la información de latitud, longitud y tiempo
    if (strstr(sentence, "$GPGGA") != NULL) {
        char *token = strtok((char*)sentence, ",");
        int count = 1;
        char *latitude, *longitude, *time;

        while (token != NULL) {
            if (count == 2) {
                time = token;
            } else if (count == 3) {
                latitude = token;
            } else if (count == 4) {
                // Hemisferio latitud (N o S)
                char hemisphereLat = *token;
            } else if (count == 5) {
                longitude = token;
            } else if (count == 6) {
                // Hemisferio longitud (E o W)
                char hemisphereLon = *token;
            }
            token = strtok(NULL, ",");
            count++;
        }

        // regular
         char formattedTime[10];
        formattedTime[0] = time[0];
        formattedTime[1] = time[1];
        formattedTime[2] = ':';
        formattedTime[3] = time[2];
        formattedTime[4] = time[3];
        formattedTime[5] = ':';
        formattedTime[6] = time[4];
        formattedTime[7] = time[5];
        formattedTime[8] = '\0';


   
        //  la latitud y la longitud 
        // Puedes convertir las coordenadas de grados, minutos y segundos a grados decimales si es necesario
        printf("Tiempo: %s, Latitud: %s, Longitud: %s\n", formattedTime, latitude, longitude);
    }
    // Puedes agregar más condiciones para procesar otros tipos de mensajes NMEA (como GPRMC) si es necesario
}



int main (){
  

 pc.write("programme start\n", strlen("programme start\n"));





gps.set_baud(9600);

    char gpsData[128];
    int index = 0;

    while (1) {
        char c;
        if (gps.read(&c, 1)) {
            if (c == '$') {
                index = 0;
                gpsData[index++] = c;
            } else if (c == '\n') {
                gpsData[index] = '\0';  // Agrega el carácter nulo al final del mensaje
                processNMEASentence(gpsData);
            } else if (index < sizeof(gpsData) - 1) {
                gpsData[index++] = c;
            }
        }
    }


 }

