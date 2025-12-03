#include "rpi.h"
#include "ov7670.h"
#include "mbox.h"

void notmain(void) { 
    
    // camera initialize
    camera_pins_setup();
    camera_i2c_setup();
    camera_register_setup();
    trace("Camera Setup Finished\n");

    caches_enable();

    uint32_t height_offset = PHY_HEIGHT;
    uint8_t* fb;
    fb = device_initialize(PHY_WIDTH, PHY_HEIGHT, VIR_WIDTH, VIR_HEIGHT, DEPTH, BGR);
    camera_display(fb, &height_offset);

    // If you don't have a display, directly print the pixel values.
    // print_camera_value();
}
