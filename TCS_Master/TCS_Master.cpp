#include "TCS_Master.h"
#include <cstdint>

tcs_control::tcs_control(uint8_t address){
    
    tcs_addr = address;

    char tcs_id[1];
    tcs_id[0] = 0;
    
    i2c_driver.write(tcs_addr, &tcs_whoami, 1);

    i2c_driver.read(tcs_addr, tcs_id, 1);

    char tcs_conf[2] = {tcs_config, 0x03};

    i2c_driver.write(tcs_addr, tcs_conf, 2);

    i2c_driver.write(tcs_addr, &tcs_config, 1);

    i2c_driver.read(tcs_addr, tcs_id, 1);

    printf("%x", tcs_id[0]);

}

Color_data tcs_control::color_meas(){
    
    Color_data data;

    char read_bytes[8];

    i2c_driver.write(tcs_addr, &tcs_data, 1);
    
    i2c_driver.read(tcs_addr, read_bytes, 8);

    data.c = (read_bytes[1] << 8) | read_bytes[0];

    data.g = (read_bytes[5] << 8) | read_bytes[4];

    data.r = (read_bytes[3] << 8) | read_bytes[2];	

    data.b = (read_bytes[7] << 8) | read_bytes[6];
 
    return data;
}