#include "rpi.h"

void i2c_init(void);

// shortest will be 130 for i2c accel.
void i2c_init_clk_div(unsigned clk_div);

// can call N times, will only initialize once (the first time)
// void i2c_init_once(void);

// write <nbytes> of <data> to i2c device address <addr>
int i2c_write(unsigned addr, uint8_t data[], unsigned nbytes);
// read <nbytes> of <data> from i2c device address <addr>
int i2c_read(unsigned addr, uint8_t data[], unsigned nbytes);
