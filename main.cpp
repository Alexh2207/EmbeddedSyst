//final proyect

#include "mbed.h"
#include "TCS_Master/TCS_Master.h"
#include "humidity_and_temp/si7021_control.h"
#include "MMA_Master/MMA_Master.h"
#include "GPS_funcion.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

// Led modo test y normal
DigitalOut led1(PB_5);
DigitalOut led2(PA_5);
DigitalOut Led_red(PH_0);
DigitalOut Led_green(PH_1);
DigitalOut Led_blue(PB_13);
AnalogIn Light(PA_4);
AnalogIn Soil(PA_0);


BufferedSerial pc(USBTX, USBRX);
// Declaracion del boton y el estado de la maquina
InterruptIn uButton (PB_2);
typedef enum {TEST , NORMAL} states_t;
states_t state_machine = TEST;
bool start = true;
char message [140];
char buffer[128];

I2C Tempeartura_humidity(PB_9,PB_8 );
I2C i2c_driver(PB_9, PB_8);


struct H1_data{

   float temp[120];
   float humid[120];
   float Light[120];
   float soil[120];
   float accel[3][120];
   float average[4];   // temp , humid , light y soil
   float maximum[4];
   float minimun[4];
   int dominant_color[3] = {0};  // en orden de red , green y blue
};

int cont_h=0;
H1_data MyData;

const char mma_addr = 0x3A;

//variables de sensor
int red;
int blue;
int green;
int clear;

//instancia de los sensor
tcs_control tcs_sensor; // sensor TCS
si7021_control si7021_sensor;

// funcion de interrrupcion 
void user_button_int(void){
    if(state_machine == NORMAL){
        state_machine =TEST;
        led1=1;
        led2=0;
        cont_h=0;
    }else if ( state_machine == TEST ) {
        state_machine = NORMAL;
        led1=0;
        led2=1;
        
    }
    start = true;
}


void color_dominante(int clear ,int red, int green, int blue) {
    
    int max_valor;
    std::string dominant_color = "red";
    
     sprintf(message,"Color sensor: clear: %d ,Red: %d ,Green:%d blue:%d--",clear,red,green,blue);
     pc.write(message , strlen(message) );
/*     max_valor= red;
     if(state_machine==TEST){
        Led_red = ((red > green) && (red > blue)) ? 1 : 0 ;
        Led_blue = ((blue > green) && (red < blue)) ? 1 : 0 ;
        Led_green = ((blue > green) && (red < green)) ? 1 : 0 ;
     }
     if(state_machine==NORMAL){
     MyData.dominant_color[0]++;
     }
    if ( green > max_valor) {
          max_valor = green;
          dominant_color ="green";/
          if(state_machine==TEST){
          Led_red=0;
          Led_blue=0;
          Led_green=1;
          }
          if(state_machine==NORMAL){
          MyData.dominant_color[1]++;
          MyData.dominant_color[0]--;
          }
        }
     if (blue > max_valor ) {
          max_valor = blue;
          dominant_color ="blue";
          if(state_machine==TEST){
          Led_red=0;
          Led_blue=1;
          Led_green=0;
          }
          if(state_machine==NORMAL){
           MyData.dominant_color[2]++;
            if ( MyData.dominant_color[1] >  MyData.dominant_color[0]) {
                MyData.dominant_color[1]--;
            } else {
                MyData.dominant_color[0]--;
            }
          }
        }*/
        if((red > green) && (red > blue)){
            if(state_machine == TEST){
                Led_red = 0;
                Led_blue = 1;
                Led_green = 1;
            }
            max_valor = red;
            dominant_color = "red";
            if(state_machine == NORMAL)
                MyData.dominant_color[0]++;
        }else if((blue > green) && (red < blue)){
            if(state_machine == TEST){
                Led_red =1;
                Led_blue = 0;
                Led_green = 1;
            }
            max_valor = blue;
            dominant_color = "blue";
            if(state_machine == NORMAL)
                MyData.dominant_color[2]++;
        }else if((blue < green) && (red < green)){
            if(state_machine == TEST){
                Led_red = 1;
                Led_blue = 1;
                Led_green = 0;
            }
            max_valor = green;
            if(state_machine == NORMAL)
                MyData.dominant_color[1]++;
            dominant_color = "green";
        }

         sprintf(message, "Color sensor: clear: %d, Red: %d, Green: %d, Blue: %d -- Dominant color: %s\n", clear, red, green, blue, dominant_color.c_str());
         pc.write(message, strlen(message));
         

          
    
}

void  Temp_hum(float t , float h){

    sprintf(message,"Temp/Hum Temperature:%f °C , Relative Humidty:%f\n",t,h);
    pc.write(message , strlen(message) );

    if(state_machine==NORMAL){
      MyData.temp[cont_h]=t;
      MyData.humid[cont_h]=h;
/*      if ( t < (-10) || t > 50 ) {
          Led_red=1;
          Led_blue=1;
          Led_green=0;
      }
      if ( h < 25 || t > 75 ) {
          Led_red=1;
          Led_blue=0;
          Led_green=1;
      }
*/

    }

}

void  F_light(uint16_t valor){
   float brightness; //o puede ser 100 -(valor /65535)* 100;
   brightness = ((float)valor/65535)*100;
   sprintf(message,"LIGHT: %f\n" , brightness);
   pc.write(message, strlen(message));
   if(state_machine==NORMAL){
      MyData.Light[cont_h] = brightness;
    }
}

void  F_accel(float x, float y, float z){
   
    sprintf(message,"X: %f, Y: %f, Z: %f \n", x, y, z);
    pc.write(message, strlen(message));
   
    if(state_machine==NORMAL){
        MyData.accel[0][cont_h] = x;
        MyData.accel[1][cont_h] = y;
        MyData.accel[2][cont_h] = z;
    }
}

void F_Soil (uint16_t valor){
   float moisture=  ((float)valor /65535)* 100; //o puede ser 100 -(valor /65535)* 100;
   sprintf(message,"Soil Moisture: %f\n" , moisture);
    pc.write(message, strlen(message));
    if(state_machine==NORMAL){
      MyData.soil[cont_h]=moisture;
    }
}

void F_GPS( char time[10],char latitude[15], char longitude[15]){
 sprintf(buffer, "GPS:Tiempo: %s, Latitud: %s, Longitud: %s\n", time,latitude,longitude);
 pc.write(buffer, strlen(buffer));
}

void F_calculo(){

    for (int j = 0; j < 4; j++) {
        float sum = 0;
        float max = INT_MIN;
        float min = INT_MAX;
        float *arr = (j == 0) ? MyData.temp : (j == 1) ? MyData.humid : (j == 2) ? MyData.Light : MyData.soil;

        for (int i = 0; i < cont_h; i++) {
            sum =sum + arr[i];
            if (arr[i] > max) max = arr[i];
            if (arr[i] < min) min = arr[i];
        }

        MyData.average[j]=sum / cont_h;
        MyData.maximum[j]=max;
        MyData.minimun[j]=min;

    }
      sprintf(buffer, " Total values %d,  Temperature : Array1 ,humidity: Array2 , Ambient Light : Array 3 , Soid moisture : Array4 \n",cont_h);
      pc.write(buffer, strlen(buffer));

   for (int i = 0; i < 4; i++) {
        sprintf(buffer,"Array %d - Promedio: %f, Máximo: %f, Mínimo: %f\n", i+1, MyData.average[i], MyData.maximum[i], MyData.minimun[i]);
        pc.write(buffer, strlen(buffer));
    }

    volatile float max_x = MyData.accel[0][0], max_y = MyData.accel[1][0], max_z = MyData.accel[2][0], min_x = MyData.accel[0][0], min_y = MyData.accel[1][0], min_z = MyData.accel[2][0];

    for(int j = 0; j < 120; j++){
        max_x = (max_x < MyData.accel[0][j]) ? MyData.accel[0][j] : max_x;
        min_x = (min_x > MyData.accel[0][j]) ? MyData.accel[0][j] : min_x;
        max_y = (max_y < MyData.accel[1][j]) ? MyData.accel[1][j] : max_y;
        min_y = (min_y > MyData.accel[1][j]) ? MyData.accel[1][j] : min_y;
        max_z = (max_z < MyData.accel[2][j]) ? MyData.accel[2][j] : max_z;
        min_z = (min_z > MyData.accel[2][j]) ? MyData.accel[2][j] : min_z;
    }

    sprintf(buffer,"X:\n\t Max: %f\n\tMin: %f\nY:\n\t Max: %f\n\tMin: %f\nZ:\n\t Max: %f\n\tMin: %f", max_x, min_x, max_y, min_y, max_z, min_z);
    pc.write(buffer, strlen(buffer));

    cont_h = 0;

}

void  F_color_Dominant(){
   std::string dominant_color;
   if(MyData.dominant_color[0] >= MyData.dominant_color[1] && MyData.dominant_color[0] >=MyData.dominant_color[2]){
       dominant_color ="red";
   } else if (MyData.dominant_color[1] >= MyData.dominant_color[0] && MyData.dominant_color[1] >= MyData.dominant_color[2]) {
       dominant_color = "green";
   }else {
       dominant_color ="blue";
   }  
  
    sprintf(message, "number of times it appears as a dominant color: red %d , green %d , blue %d . the dominant color with the most times is %s \n", MyData.dominant_color[0],MyData.dominant_color[1],MyData.dominant_color[2], dominant_color.c_str( ) );
    pc.write(message, strlen(message));

}

// main() runs in its own thread in the OS
int main()
{
    int max_valor;
    
    
    //Variables de datos:

  
    accel_vector acceleration;

    //Inicialización de sensores:

    mma_control MMA(mma_addr, 2);

    tcs_control tcs;

    si7021_control temp(0x80);

    temp.write_user_reg(0);

    //configuarcion de la interrupcion en el main
    uButton.mode(PullUp);
    uButton.rise(user_button_int );
    
     //iniciacion de led y configuraciones
      led1=1;
      led2=0;
       
    configureGPSToGPGGAyGPRMC();
    while (true) {
        

        //funciones de lectura de los sensores:
        //acceleration = mma_read_vector();
        //color = tcs.color_meas();
        //humid_meas = temp.data_meas();
        
        // llama a las funciones de sensor
        Color_data color_data = tcs_sensor.color_meas();
        RHT_data humid_meas =si7021_sensor.data_meas();
        gps_t gpsData = { "", "", "", 0};
         
        while(gpsData.ack == 0){
            gpsData = read_data();
        }
        acceleration = MMA.mma_read_vector();
        uint16_t amb_light = Light.read_u16();
        uint16_t soil_moisture = Soil.read_u16();
       
       
        switch ( state_machine ) {
             
            case TEST:
             
             sprintf(message,"Esto es modo TEST \n");
             pc.write(message , strlen(message) );

             

             color_dominante(color_data.c,color_data.r, color_data.g,color_data.b);
             
             Temp_hum(humid_meas.temp ,humid_meas.humid);
            
             F_light(amb_light);
             F_Soil(soil_moisture);
             F_GPS(gpsData.formattedTime, gpsData.latitude, gpsData.longitude);
            F_accel(acceleration.x,acceleration.y,acceleration.z);

             sprintf(message, "\n\n");
             pc.write(message, strlen(message));
             ThisThread::sleep_for(2s);
            break;
            
             


            case NORMAL:
              
              
                sprintf(message,"Esto es modo Normal \n");
                pc.write(message , strlen(message) );
                 
                //llamada de funciones delos sensor
                Temp_hum(humid_meas.temp ,humid_meas.humid);
                F_GPS(gpsData.formattedTime, gpsData.latitude, gpsData.longitude);
                F_light(amb_light);
                F_Soil(soil_moisture);
                color_dominante(color_data.c,color_data.r, color_data.g,color_data.b);
                F_accel(acceleration.x,acceleration.y,acceleration.z);
                cont_h++;

                if (cont_h == 10) {
                    F_calculo();
                    F_color_Dominant();
                }
               
              sprintf(message, "\n\n");
              pc.write(message, strlen(message));
              ThisThread::sleep_for(3s);

              if(humid_meas.temp > 50 || humid_meas.temp < -10){
                    Led_red=0;
                    Led_blue=1;
                    Led_green=1;
              }else if(humid_meas.humid > 75 || humid_meas.humid < 25){
                    Led_red=1;
                    Led_blue=0;
                    Led_green=1;
              }else if(amb_light < 25 || amb_light > 75){
                    Led_red=1;
                    Led_blue=1;
                    Led_green=0;
              }else if (soil_moisture < 25 || soil_moisture > 75) {
                    Led_red=0;
                    Led_blue=0;
                    Led_green=1;
              }else if (color_data.r > 0.5*color_data.c || color_data.b > 0.5*color_data.c) {
                    Led_red=1;
                    Led_blue=0;
                    Led_green=0;
              }else if ((acceleration.x > 0.5 || acceleration.y > 0.5) && acceleration.z < 0.7){
                    Led_red=0;
                    Led_blue=0;
                    Led_green=0;
              }
                   
                break;
        }
    }
}
