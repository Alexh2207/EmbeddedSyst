#include "mbed.h"
#include "TCS_Master/TCS_Master.h"
#include "humidity_and_temp/si7021_control.h"
#include "MMA_Master/MMA_Master.h"
#include "GPS_funcion.h"
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
char message [100];

I2C Tempeartura_humidity(PB_9,PB_8 );
I2C i2c_driver(PB_9, PB_8);


struct H1_data{

   int temp[120];
   int humid[120];
   int Light[120];
   int soil[120];
   
};

int cont_h=0;
H1_data MyData;


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


//variables de sensor
int red;
int blue;
int green;
int clear;


void color_dominante(int clear ,int red, int green, int blue) {
    
    int max_valor;
    std::string dominant_color = "red";
    printf("1");
             sprintf(message,"Color sensor: clear: %d ,Red: %d ,Green:%d blue:%d--",clear,red,green,blue);
             pc.write(message , strlen(message) );
             max_valor= red;
             Led_red=1;
             Led_blue=0;
             Led_green=0;
            if ( green > max_valor) {
                    max_valor = green;
                    dominant_color ="green";
                    Led_red=0;
                    Led_blue=0;
                    Led_green=1;
                }
             if (blue > max_valor ) {
                    max_valor = blue;
                    dominant_color ="blue";
                    Led_red=0;
                    Led_blue=1;
                    Led_green=0;
                }
            sprintf(message,"Dominant color: %s \n" , dominant_color.c_str());
            pc.write(message, strlen(message));

          
    
}

void  Temp_hum(int t , int h){

    sprintf(message,"Temp/Hum Temperature:%d °C , Relative Humidty:%d % \n",t,h);
    pc.write(message , strlen(message) );

}

void  F_light(float valor){
   int brightness=100- (valor /65535)* 100; //o puede ser 100 -(valor /65535)* 100;
   sprintf(message,"LIGHT: %d % \n" , brightness);
    pc.write(message, strlen(message));
}

void F_Soil (float valor){
   int moisture= (valor /65535)* 100; //o puede ser 100 -(valor /65535)* 100;
   sprintf(message,"Soil Moisture: %d % \n" , moisture);
    pc.write(message, strlen(message));
}


void Max_min(){
   int i;
   int Max_num;
   int Max_min; 
      

}

// main() runs in its own thread in the OS
int main()
{

    pc.set_baud(115200);
    pc.set_format(8, pc.None, 1);
    pc.write("STARTING PROGRAM\n", 17);

    int max_valor;
    
    char tiempo[10];
    char *latitude;
    char *longitude;
    //Variables de datos:

    //RHT_data humid_meas;*
    // Color_data color; *
    accel_vector acceleration;

    //Inicialización de sensores:

    mma_control mma;

    tcs_control tcs;

    si7021_control temp(0x80);

    temp.write_user_reg(0);

    //configuarcion de la interrupcion en el main
    uButton.mode(PullUp);
    uButton.rise(user_button_int );

    char string [50];
    
     //iniciacion de led 
      led1=1;
      led2=0;
     
    while (true) {
        

        //funciones de lectura de los sensores:
        //acceleration = mma_read_vector();
        //color = tcs.color_meas();
        //humid_meas = temp.data_meas();
        
        // llama a las funciones de sensor
        Color_data color_data = tcs_sensor.color_meas();
        RHT_data humid_meas =si7021_sensor.data_meas();
         gps_t gpsData = read_data();
       

        switch ( state_machine ) {
             
            case TEST:
             
             sprintf(message,"Esto es modo TEST \n");
             pc.write(message , strlen(message) );

             acceleration = mma.mma_read_vector();

             color_dominante(color_data.c,color_data.r, color_data.g,color_data.b);
             
             Temp_hum(humid_meas.temp ,humid_meas.humid);
            
             F_light( Light.read_u16());
             F_Soil(Soil.read_u16());

            sprintf(string,"X: %f, Y: %f, Z: %f \n", acceleration.x, acceleration.y, acceleration.z);

            pc.write(string, strlen(string));
            
            
             


            sprintf(message,"Tiempo: %s, Latitud: %s, Longitud: %s\n", gpsData.formattedTime, gpsData.latitude, gpsData.longitude);
            pc.write(message, strlen(message));
             
            ThisThread::sleep_for(2s);
            break;
            
             


            case NORMAL:
              
                sprintf(message,"esto es modo Normal \n");
                pc.write(message , strlen(message) );
                 
                //llamada de funciones delos sensor

              color_dominante(color_data.c,color_data.r, color_data.g,color_data.b);
              Temp_hum(humid_meas.temp ,humid_meas.humid);
               MyData.temp[cont_h]=humid_meas.temp;
               MyData.humid[cont_h]=humid_meas.humid;
              F_light( Light.read_u16());
                MyData.Light[cont_h]=Light.read_u16();
              F_Soil(Soil.read_u16());
                 MyData.soil[cont_h]=Soil.read_u16();
                 cont_h++;
                 sprintf(message,"Esto es la %d medida \n",cont_h);
                 pc.write(message , strlen(message) );
              ThisThread::sleep_for(30s);
                   
                
                break;
        }
    }
}
