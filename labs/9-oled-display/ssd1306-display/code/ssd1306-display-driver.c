#include "rpi.h"
#include "ssd1306-display-driver.h"
#include "i2c.h"

static uint8_t i2c_buffer[SSD1306_I2C_BUFFER_SIZE];
static uint8_t *display_buffer = i2c_buffer + 1;

// Helper function to send a byte over I2C
void ssd1306_display_send_command(uint8_t cmd) {
  uint8_t cmd_buf[2] = {0x00, cmd};
  i2c_write(SSD1306_DISPLAY_ADDRESS, cmd_buf, 2);
}

// Initialize the display.
// Requirement: I2C should have been initialized beforehand.
void ssd1306_display_init(void) {
  /* Display initialization flow [SSD1306 datasheet pg 64] */

  // 0. Turn the display off to be safe [SSD1306 pg 28]
  ssd1306_display_send_command(0xAE);

  // 1. Set multiplex ratio

  // 2. Set display offset [SSD1306 pg 37]

  // 3. Set display start line [SSD1306 pg 36]

  // 4. Set segment re-map [SSD1306 pg 36]

  // 5. Set COM output scan direction

  // 6. Set COM pins hardware configuration [SSD1306 pg 40]

  // 7. Set contrast control [SSD1306 pg 36]

  // 8. Display output according to GDDRAM contents [SSD1306 pg 37]

  // 9. Set normal or inverse display [SSD1306 pg 37]

  // 10. Set display clock divide ratio/oscillator frequency [SSD1306 pg 40]

  // 11. Enable charge pump regulator [SSD1306 pg 62]

  // 12. Specify HORIZONTAL addressing mode [SSD1306 pg 35]

  // 13. Display on [SSD1306 pg 62]

  // 14. Clear the screen to black and call display_show()
}

// Send display buffer to screen via I2C
// Must be called to actually update the display!
void ssd1306_display_show(void) {
  i2c_buffer[0] = 0x40; // control byte to indicate data
  i2c_write(SSD1306_DISPLAY_ADDRESS, i2c_buffer, sizeof(i2c_buffer));
}

// Clears the screen to black; no change until display_show() is called
void ssd1306_display_clear(void) {
  i2c_buffer[0] = 0x40; // control byte to indicate data
  memset(display_buffer, 0, SSD1306_DISPLAY_BUFFER_SIZE);
}

// Fills the display completely with white
void ssd1306_display_fill_white(void) {
  i2c_buffer[0] = 0x40; // control byte to indicate data
  memset(display_buffer, 0xFF, SSD1306_DISPLAY_BUFFER_SIZE);
}

void ssd1306_display_draw_pixel(uint16_t x, uint16_t y, color_t color) {
  // https://github.com/adafruit/Adafruit_SSD1306/blob/master/Adafruit_SSD1306.cpp#L648
  // May need to perform additional coordinate transforms,
  // depending on what coordinate system you want to use with
  // the display.

  switch (color) {
  case COLOR_WHITE:
    display_buffer[(y / 8) * SSD1306_DISPLAY_WIDTH + x] |= (1 << (y & 7));
    break;
  case COLOR_BLACK:
    display_buffer[(y / 8) * SSD1306_DISPLAY_WIDTH + x] &= ~(1 << (y & 7));
    break;
  case COLOR_INVERT:
    display_buffer[(y / 8) * SSD1306_DISPLAY_WIDTH + x] ^= (1 << (y & 7));
    break;
  }
}

void ssd1306_display_draw_horizontal_line(int16_t x_start, int16_t x_end,
                                          int16_t y, color_t color) {

  // https://github.com/adafruit/Adafruit_SSD1306/blob/master/Adafruit_SSD1306.cpp#L706

  if (y < 0 || y >= SSD1306_DISPLAY_HEIGHT) {
    return;
  }
}

void ssd1306_display_draw_vertical_line(int16_t y_start, int16_t y_end,
                                        int16_t x, color_t color) {

  // https://github.com/adafruit/Adafruit_SSD1306/blob/master/Adafruit_SSD1306.cpp#L806

  if (x < 0 || x >= SSD1306_DISPLAY_WIDTH) {
    return;
  }
}

void ssd1306_display_draw_fill_rect(int16_t x, int16_t y, uint16_t w,
                                    uint16_t h, color_t color) {

  // https://github.com/adafruit/Adafruit-GFX-Library/blob/master/Adafruit_GFX.cpp#L300
  for (int16_t i = x; i < x + w; i++) {
    ssd1306_display_draw_vertical_line(y, y + h, i, color);
  }
}

void ssd1306_display_draw_character_size(uint16_t x, uint16_t y,
                                         unsigned char c, color_t color,
                                         uint8_t size_x, uint8_t size_y) {

  // https://github.com/adafruit/Adafruit-GFX-Library/blob/master/Adafruit_GFX.cpp#L1249

  if ((x >= SSD1306_DISPLAY_WIDTH) ||  // Clip right
      (y >= SSD1306_DISPLAY_HEIGHT) || // Clip bottom
      ((x + 6 * size_x - 1) < 0) ||    // Clip left
      ((y + 8 * size_y - 1) < 0)) {    // Clip top
    return;
  }
}
