
#ifndef SI7021_H
#define SI7021_H

#include "mbed.h"
#include <cstdint>
#include <cstdio>

extern I2C i2c_driver;


const char si_usr_reg_wr = 0xE6;
const char si_usr_reg_rd = 0xE7;
const char si_heat_reg_wr = 0x51;
const char si_meas_humid = 0xF5;
const char si_meas_temp = 0xE0;

typedef struct{
    float temp;  // de float a int pase
    float humid;
} RHT_data;

class si7021_control{
    private:

        int si7021_addr;

    public:

        //constructor of the class, assigns the I2C address of the sensor
        si7021_control(uint8_t address = 0x80);

        //Writes the user register with the specified value
        void write_user_reg(uint8_t value);
        
        //Writes the heater register with the specified value
        void write_heater_reg(uint8_t value);
        
        //Reads the data offered by the sensor
        RHT_data data_meas();

};


#endif
