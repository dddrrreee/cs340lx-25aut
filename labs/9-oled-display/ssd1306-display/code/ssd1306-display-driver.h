#ifndef __SSD1306_DISPLAY_DRIVER__
#define __SSD1306_DISPLAY_DRIVER__

#include "rpi.h"

// This is declared here as extern and defined in standard-ascii-font.c
extern const unsigned char standard_ascii_font[];

// Macro to read byte
#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif

// Macro to swap two values
#ifndef SWAP
#define SWAP(x, y)                                                             \
  do {                                                                         \
    typeof(x) SWAP = x;                                                        \
    x = y;                                                                     \
    y = SWAP;                                                                  \
  } while (0)
#endif

// Enum of color values
typedef enum {
  COLOR_WHITE,
  COLOR_BLACK,
  COLOR_INVERT,
} color_t;

enum {
  SSD1306_DISPLAY_ADDRESS = 0X3C,

  SSD1306_DISPLAY_WIDTH = 128,
  SSD1306_DISPLAY_HEIGHT = 64,
  SSD1306_DISPLAY_BUFFER_SIZE =
      SSD1306_DISPLAY_WIDTH * ((SSD1306_DISPLAY_HEIGHT + 7) / 8),

  SSD1306_I2C_BUFFER_SIZE = SSD1306_DISPLAY_BUFFER_SIZE + 1,
};

void ssd1306_display_init(void);

void ssd1306_display_show(void);

void ssd1306_display_clear(void);

void ssd1306_display_fill_white(void);

void ssd1306_display_draw_pixel(uint16_t x, uint16_t y, color_t color);

void ssd1306_display_draw_horizontal_line(int16_t x_start, int16_t x_end,
                                          int16_t y, color_t color);

void ssd1306_display_draw_vertical_line(int16_t y_start, int16_t y_end,
                                        int16_t x, color_t color);

void ssd1306_display_draw_fill_rect(int16_t x, int16_t y, uint16_t w,
                                    uint16_t h, color_t color);

void ssd1306_display_draw_character_size(uint16_t x, uint16_t y,
                                         unsigned char c, color_t color,
                                         uint8_t size_x, uint8_t size_y);

#endif
