
#ifndef MMA_MASTER_H
#define MMA_MASTER_H

#include "mbed.h"
#include <cstdint>

extern I2C i2c_driver;

typedef struct{
    float x;
    float y;
    float z;
}accel_vector;

class mma_control{
    private:

        int mma_addr;
        int range_store;

    public:

        mma_control(uint8_t address = 0x3A, int range=2);
        
        accel_vector mma_read_vector();

};


#endif
