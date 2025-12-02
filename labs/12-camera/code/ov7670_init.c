#include "rpi.h"
#include "ov7670.h"
#include "bit-support.h"
#include "gpio.h"
#include "i2c.h"

// read a single device register <reg> from i2c device
// <addr> and return the result.
uint8_t imu_rd(uint8_t addr, uint8_t reg) {
  i2c_write(addr, &reg, 1);
  uint8_t v;
  i2c_read(addr, &v, 1);
  return v;
}

// Setup camera pins
// D0-D7: Data pins
// VS: Vertical Sync
// HS: Horizontal Sync
// PCLK: Pixel Clock
// RST: Reset (Active Low)
// SCL: Serial Clock (I2C)
// SDA: Serial Data (I2C)
// MCLK: Master Clock
void camera_pins_setup(void) {
  // Set gpio input/output
  gpio_set_input(D0);
  gpio_set_input(D1);
  gpio_set_input(D2);
  gpio_set_input(D3);
  gpio_set_input(D4);
  gpio_set_input(D5);
  gpio_set_input(D6);
  gpio_set_input(D7);
  gpio_set_input(VS);
  gpio_set_input(HS);
  gpio_set_input(PCLK);
  gpio_set_output(RST);
  gpio_set_on(RST);

  // TODO: Setup GPIO master clock
  // 1. Set MCLK to General Purpose Clock 1 (BCM2835 p.102)
  todo("Setup master clock pin");
  
  // 2. Set clock divisors (BCM2835 p.105, 108)
  // You can start from setting the integer part to 0x64
  todo("Setup Clock Divisor");

  // 3. Set clock source
  // First disable clock generator and wait until BUSY==0
  PUT32(GP1_CLK_CTRL, 0x5A000000);
  while(bit_is_on(GET32(GP1_CLK_CTRL), 7));
  // Set clock source to PLLD, then enable clock generator
  todo("Set clock source");

  // Check clock generator is running
  assert(bit_is_on(GET32(GP1_CLK_CTRL), 7));
}

void camera_i2c_setup(void) {
  i2c_init();
  delay_ms(100);
  trace("i2c init completed\n");
  uint32_t PID_MSB = imu_rd(SLAVE_ADDR, 0x0A);
  assert(PID_MSB == 0x76);
  uint32_t PID_LSB = imu_rd(SLAVE_ADDR, 0x0B);
  assert(PID_LSB == 0x73);
}