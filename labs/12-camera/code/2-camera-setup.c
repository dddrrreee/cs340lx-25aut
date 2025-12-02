#include "rpi.h"
#include "ov7670.h"

void Camera_fps_test(void) {
    uint32_t t_start, t_end;
    wait_neg_edge(VS);
    t_start = timer_get_usec();
    wait_neg_edge(VS);
    t_end = timer_get_usec();
    trace("Time spent in a camera frame: %d usec\n", t_end - t_start);
  }

void notmain(void) { 
    // pins setup
    camera_pins_setup();
    // i2c setup
    camera_i2c_setup();
    // camera setup
    camera_register_setup();
    trace("Camera Setup Finished\n");
    // test camera frame rate (microsecond per frame)
    Camera_fps_test();
}
