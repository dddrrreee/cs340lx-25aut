#include "rpi.h"
#include "sh1106-display-driver.h"
#include "i2c.h"

// Helper function to send a byte over I2C
void sh1106_display_send_command(uint8_t cmd) {
  uint8_t cmd_buf[2] = {0x00, cmd};
  i2c_write(SH1106_DISPLAY_ADDRESS, cmd_buf, 2);
}

// Initialize the display. Requirement: I2C must have been initialized
// beforehand
void sh1106_display_init(void) {
  // Display OFF [SH1106 pg 23]
  sh1106_display_send_command(0xAE);

  // Initialization sequence

  // Clear the screen
  sh1106_display_clear();

  // Show the cleared screen
  sh1106_display_show();
}

// Send display buffer to screen via I2C
// Must be called to actually update the display!
void sh1106_display_show(void) {
  // SH1106 only supports page-addressing mode,
  // so we must send 1 page at a time via I2C
  unsigned NUM_PAGES = 8;

  for (int i = 0; i < NUM_PAGES; i++) {
    // Reset the display data RAM column address to 0
    // Set Lower Column Address command [SH1106 pg 19]
    sh1106_display_send_command(0x00);
    // Set Higher Column Address command [SH1106 pg 19]
    sh1106_display_send_command(0x10);

    // Specify the page address to load display data RAM
    // Set Page Address command [SH1106 pg 23]

    // Send data for a single page.
    // First byte must be 0x40 to indicate data
  }

  return;
}

void sh1106_display_fill_white(void) {
  return;
}

void sh1106_display_clear(void) {
  return;
}

void sh1106_display_draw_pixel(uint16_t x, uint16_t y, color_t color) {
  // https://github.com/adafruit/Adafruit_SSD1306/blob/master/Adafruit_SSD1306.cpp#L648
}

void sh1106_display_draw_horizontal_line(int16_t x_start, int16_t x_end,
                                         int16_t y, color_t color) {
  // https://github.com/adafruit/Adafruit_SSD1306/blob/master/Adafruit_SSD1306.cpp#L706
}

void sh1106_display_draw_vertical_line(int16_t y_start, int16_t y_end,
                                       int16_t x, color_t color) {
  // https://github.com/adafruit/Adafruit_SSD1306/blob/master/Adafruit_SSD1306.cpp#L806
}

void sh1106_display_draw_fill_rect(int16_t x, int16_t y, uint16_t w, uint16_t h,
                                   color_t color) {
  // https://github.com/adafruit/Adafruit-GFX-Library/blob/master/Adafruit_GFX.cpp#L300
  for (int16_t i = x; i < x + w; i++) {
    sh1106_display_draw_vertical_line(y, y + h, i, color);
  }
}

void sh1106_display_draw_character_size(uint16_t x, uint16_t y, unsigned char c,
                                        color_t color, uint8_t size_x,
                                        uint8_t size_y) {
  // https://github.com/adafruit/Adafruit-GFX-Library/blob/master/Adafruit_GFX.cpp#L1249
}
