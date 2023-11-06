
#ifndef TCS_MASTER_H
#define TCS_MASTER_H

#include "mbed.h"
#include <cstdint>
#include <cstdio>

extern I2C i2c_driver;

const char tcs_whoami = 0x92;
const char tcs_config = 0x80;
const char tcs_data = 0xa0 | 0x14;
//const char si_meas_humid = 0xF5;
//const char si_meas_temp = 0xE0;

typedef struct{
    int r,g,b,c;
} Color_data;

class tcs_control{
    private:

        int tcs_addr;

    public:

        tcs_control(uint8_t address = 0x52);
        
        Color_data color_meas();

};


#endif
