#include "mbed.h"
#include "TCS_Master/TCS_Master.h"
#include "humidity_and_temp/si7021_control.h"
#include "GPS_funcion.h"


I2C i2c_driver(D14, D15);

typedef struct{
    float x;
    float y;
    float z;
}accel_vector;

int range_store = 0;

const char mma_addr = 0x3A;

void mma_config(uint8_t addr, int range, int oversampling);

accel_vector mma_read_vector();

// main() runs in its own thread in the OS
int main()
{

    //Variables de datos:

    RHT_data humid_meas;
    Color_data color;
    accel_vector acceleration;

    //Inicialización de sensores:

    mma_config(mma_addr, 2, 2);

    tcs_control tcs;

    si7021_control temp(0x80);

    temp.write_user_reg(0);

    while (true) {
        read_data();

        //funciones de lectura de los sensores:
        acceleration = mma_read_vector();
        color = tcs.color_meas();
        humid_meas = temp.data_meas();
    }
}

/*
// main() runs in its own thread in the OS
int main()
{

    mma_config(mma_addr, 2, 2);

    accel_vector data = mma_read_vector();

    ThisThread::sleep_for(1s);

    data = mma_read_vector();

    ThisThread::sleep_for(1s);

    data = mma_read_vector();

    printf("%f", data.x);

    while (true) {
        
    }
}*/

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