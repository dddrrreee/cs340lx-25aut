#include "rpi.h"
#include "ov7670.h"

void notmain(void) { 
    camera_pins_setup();
    camera_i2c_setup();
    trace("I2C Setup Finished\n");
}