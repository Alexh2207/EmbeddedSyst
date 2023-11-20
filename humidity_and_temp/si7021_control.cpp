#include "si7021_control.h"

si7021_control::si7021_control(uint8_t address){
    si7021_addr = address;
}

void si7021_control::write_user_reg(uint8_t value){

    char usr_val[1];

    i2c_driver.write(si7021_addr, &si_usr_reg_rd, 1);

    i2c_driver.read(si7021_addr, usr_val, 1);

    if(usr_val[0] != value){
        
        char write_val[2] = {si_usr_reg_wr, value};
        
        i2c_driver.write(si7021_addr, write_val, 2);
        
    }

}

void si7021_control::write_heater_reg(uint8_t value){

    char usr_val[2] = {si_heat_reg_wr, value};

    i2c_driver.write(si7021_addr, &si_usr_reg_rd, 2);

}

RHT_data si7021_control::data_meas(){

    char humid_val[2];

    char temp_val[2];

    RHT_data result;

    i2c_driver.write(si7021_addr, &si_meas_humid, 1);

    while(i2c_driver.read(si7021_addr, humid_val, 2) != 0){
        ThisThread::sleep_for(5ms);
    }

    i2c_driver.write(si7021_addr, &si_meas_temp, 1);

    i2c_driver.read(si7021_addr, temp_val, 2);

    result.temp = ((175.72 * (temp_val[0] << 8 | temp_val[1]))/65536) - 46.85;

    result.humid = ((125.0 * (temp_val[0] << 8 | temp_val[1]))/65536) - 6;

    return result;

}
