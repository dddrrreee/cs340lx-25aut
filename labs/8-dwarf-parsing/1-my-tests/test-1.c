#include "rpi.h"

enum { act_led = 47 };
void act_init(void) { gpio_set_output(act_led); }
void act_on(void) { gpio_write(act_led,0); }
void act_off(void) { gpio_write(act_led,1); }

void notmain(void) {
    act_init();
    int i = 0;
    for(i = 0; i < 50; i++) {
        act_on();
        act_off();
    }
}
