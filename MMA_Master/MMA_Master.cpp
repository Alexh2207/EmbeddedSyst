#include "MMA_Master.h"

//Funcion de configración del acelerómetro

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

    //Lectura del who am I
    if(i2c_driver.write(mma_addr, &(whoami), 1, true) != 0)
        printf("NACK");

    i2c_driver.read(mma_addr, read_reset, 1);

    //comprobación de who am I
    printf("%x", read_reset[0]);

    //reset
    i2c_driver.write(mma_addr, reset, 2);

    i2c_driver.write(mma_addr, &(reset[0]), 1);
    
    //espera hasta que el reset se complete
    do{
        i2c_driver.read(mma_addr, read_reset, 1);
    }while(read_reset[0] == 0x40);

    //Configuración de rango de medida (2g,4g,8g)
    i2c_driver.write(mma_addr, range_config, 2);

    //Encendido del acelerómetro
    i2c_driver.write(mma_addr, activate, 2);
    
    //Apagado de la cola FIFO (no la usamos)
    i2c_driver.write(mma_addr, fifo_off, 2);

}

accel_vector mma_control::mma_read_vector(){
    
    accel_vector data;

    char data_addr = 0x01;

    char status = 0x00;

    char data_raw[7];

    i2c_driver.write(mma_addr, &status, 1);

    //Lectura de los datos en bruto
    i2c_driver.read(mma_addr, data_raw, 7);

    //Operaciones para sacar los datos de forma correcta

    int16_t unsigned_x = (((data_raw[1] << 8 | data_raw[2])) >> 2);
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
