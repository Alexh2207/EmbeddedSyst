#include "mbed.h"
#include "TCS_Master/TCS_Master.h"
#include "humidity_and_temp/si7021_control.h"
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

typedef struct{
    float x;
    float y;
    float z;
}accel_vector;

int range_store = 0;

const char mma_addr = 0x3A;

void mma_config(uint8_t addr, int range, int oversampling);

accel_vector mma_read_vector();


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
    int max_valor;
    
    char tiempo[10];
    char *latitude;
    char *longitude;
    //Variables de datos:

    //RHT_data humid_meas;*
    // Color_data color; *
    accel_vector acceleration;

    //Inicialización de sensores:

    mma_config(mma_addr, 2, 2);

    tcs_control tcs;

    si7021_control temp(0x80);

    temp.write_user_reg(0);

    //configuarcion de la interrupcion en el main
    uButton.mode(PullUp);
    uButton.rise(user_button_int );
    
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

             

             color_dominante(color_data.c,color_data.r, color_data.g,color_data.b);
             
             Temp_hum(humid_meas.temp ,humid_meas.humid);
            
             F_light( Light.read_u16());
             F_Soil(Soil.read_u16());
             
            
            
             


             sprintf(message,"Tiempo: %s\n", gpsData.formattedTime);
             pc.write(message, strlen(message));
             
             sprintf(message, "\n\n");
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

void mma_config(uint8_t addr, int range, int oversampling){

    char reset[2] = {0x2B, 0x40};

    char whoami = 0x0D;
    range_store = (range);
    int range_store_config = range_store >> 2;
    char range_config[2] = {0x0E, (char)range_store_config}; 
    char oversampling_config[2] = {0x0E, (char)(oversampling & 0x03)};
    char activate[2] = {0x2A, 0x05};
    char fifo_off[2] = {0x09, 0x00};
    char orientation[2] = {0x11, 0x40};

    char read_reset[1];

    if(i2c_driver.write(mma_addr, &(whoami), 1, true) != 0)
        printf("shiiiiit");

    i2c_driver.read(mma_addr, read_reset, 1);

    printf("%x", read_reset[0]);

    i2c_driver.write(mma_addr, reset, 2);

    i2c_driver.write(mma_addr, &(reset[0]), 1);
    do{
        i2c_driver.read(mma_addr, read_reset, 1);
    }while(read_reset[0] == 0x40);

    i2c_driver.write(mma_addr, range_config, 2);

    i2c_driver.write(mma_addr, oversampling_config, 2);

    i2c_driver.write(mma_addr, activate, 2);
    
    i2c_driver.write(mma_addr, fifo_off, 2);

}

accel_vector mma_read_vector(){

    accel_vector data;

    char data_addr = 0x01;

    char data_raw[6];

    i2c_driver.write(mma_addr, &data_addr, 1);

    i2c_driver.read(mma_addr, data_raw, 6);

    data.x = ((data_raw[0] << 8 | data_raw[1]) >> 2)/((4096.0*2)/range_store);
    data.y = ((data_raw[2] << 8 | data_raw[3]) >> 2)/((4096.0*2)/range_store);
    data.z = ((data_raw[4] << 8 | data_raw[5]) >> 2)/((4096.0*2)/range_store);

    return data;
}