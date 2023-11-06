#include "MMA_Master.h"

mma_control::mma_control(uint8_t address, int range){

    char reset[2] = {0x2B, 0x40};

    mma_addr = address;

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

accel_vector mma_control::mma_read_vector(){
    
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
