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

  // Draw individual pixel
  sh1106_display_draw_pixel(0, 0, COLOR_WHITE);

  // Draw horizontal line
  sh1106_display_draw_horizontal_line(0, 100, 20, COLOR_WHITE);

  // Draw vertical line
  sh1106_display_draw_vertical_line(0, 50, 127, COLOR_WHITE);

  // Draw filled rectangle
  sh1106_display_draw_fill_rect(40, 30, 50, 20, COLOR_WHITE);

  // Draw some text
  char *text = "hello cs340lx!";
  for (int i = 0; i < strlen(text); i++) {
    sh1106_display_draw_character_size(5 + 5 * i, 10, text[i], COLOR_WHITE, 1,
                                        1);
  }

  // Draw some bigger text
  sh1106_display_draw_character_size(10, 40, 'A', COLOR_WHITE, 2, 2);

  sh1106_display_show();

  while (1) {
  }
}
