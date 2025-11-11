#include "rpi.h"
#include "i2c.h"
#include "sh1106-display-driver.h"

void notmain(void) {
  // Initialize I2C with some settling time
  delay_ms(100);
  i2c_init_clk_div(1500);
  delay_ms(100);

  // Initialize the display with some settling time
  sh1106_display_init();
  delay_ms(100);

  sh1106_display_fill_white();

  sh1106_display_show();

  while (1) {}
}
