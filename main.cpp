#include "mbed.h"
#include <cstdint>

I2C i2c_driver(D14, D15);
BufferedSerial pc(USBTX, USBRX);

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

    pc.set_baud(115200);
    pc.set_format(8, pc.None, 1);
    pc.write("STARTING PROGRAM\n", 17);

    mma_config(mma_addr, 2, 2);

    ThisThread::sleep_for(1s);

    accel_vector data = mma_read_vector();

    ThisThread::sleep_for(1s);

    data = mma_read_vector();

    ThisThread::sleep_for(1s);

    data = mma_read_vector();

    printf("%f", data.x);

    char string [50];

    while (true) {
        ThisThread::sleep_for(1s);

        data = mma_read_vector();

        sprintf(string,"X: %f, Y: %f, Z: %f \n", data.x, data.y, data.z);

        pc.write(string, strlen(string));
    }
}

void mma_config(uint8_t addr, int range, int oversampling){

    char reset[2] = {0x2B, 0x40};

    char whoami = 0x0D;
    range_store = (range);
    int range_store_config = range_store >> 2;
    char range_config[2] = {0x0E, (char)range_store_config}; 
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

    i2c_driver.write(mma_addr, activate, 2);
    
    i2c_driver.write(mma_addr, fifo_off, 2);

}

accel_vector mma_read_vector(){

    accel_vector data;

    char data_addr = 0x01;

    char status = 0x00;

    char data_raw[7];

    i2c_driver.write(mma_addr, &status, 1);

    i2c_driver.read(mma_addr, data_raw, 7);

 //   i2c_driver.write(mma_addr, &data_addr, 1);

 //   i2c_driver.read(mma_addr, data_raw, 6);
/*
    int16_t unsigned_x = ((data_raw[1] >> 7) == 0) ? ((data_raw[1] << 8 | data_raw[2]) >> 2) : (((data_raw[1] << 8 | data_raw[2]) >> 2) | 0xC000);
    int16_t unsigned_y = ((data_raw[3] >> 7) == 0) ? ((data_raw[3] << 8 | data_raw[4]) >> 2) : (((data_raw[3] << 8 | data_raw[4]) >> 2) | 0xC000);
    int16_t unsigned_z = ((data_raw[5] >> 7) == 0) ? ((data_raw[5] << 8 | data_raw[6]) >> 2) : (((data_raw[5] << 8 | data_raw[6]) >> 2) | 0xC000);
*/

    (((int16_t)(data_raw[1] << 8 | data_raw[2])) >> 2)

    int16_t unsigned_x = ((data_raw[1] << 8 | data_raw[2]));
    unsigned_x >>= 2; 
    int16_t unsigned_y = (data_raw[3] << 8 | data_raw[4]);
    unsigned_y >>= 2;
    int16_t unsigned_z = (data_raw[5] << 8 | data_raw[6]);
    unsigned_z >>= 2;

    printf("%d", unsigned_z);

    data.x = (float)(unsigned_x)/((4096*2)/range_store);
    data.y = (float)(unsigned_y)/((4096*2)/range_store);
    data.z = (float)(unsigned_z)/((4096*2)/range_store);

    return data;
}