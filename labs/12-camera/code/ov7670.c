#include "rpi.h"
#include "ov7670.h"
#include "bit-support.h"
#include "gpio.h"
#include "i2c.h"
#include "mbox.h"

const uint32_t WAIT_TIME_MS = 100;

// set a single device register <reg> on device
// <addr> to value <v>
void imu_wr(uint8_t addr, uint8_t reg, uint8_t v) {
  uint8_t data[2];
  data[0] = reg;
  data[1] = v;
  i2c_write(addr, data, 2);
  // wait for the register to be written
  delay_ms(WAIT_TIME_MS);
}

void camera_register_setup(void) {
  trace("Start camera register setup...\n");

  // Register reset
  imu_wr(SLAVE_ADDR, REG_COM7, COM7_RESET);

  // Set internal clock to input clock
  imu_wr(SLAVE_ADDR, REG_CLKRC, 0x00);
  // Scaling pclk enable, pclk very slow
  imu_wr(SLAVE_ADDR, REG_SCALEPCLK, 0xF8);
  imu_wr(SLAVE_ADDR, REG_COM14, 0x18);

  // Setup control registers
  // 1. No downscaling (we want 480*640)
  todo("Setup REG_SCALEDCW");
  // 2. Set hsync rising/falling edge delay to 0
  todo("Setup REG_HSYST, REG_HSYEN");
  // 3. Set data format to RGB565 and data range 00-FF
  todo("Setup REG_COM7, COMO15");

  // Setup other camera attributes you want! Read OV7670 p.11-26. 
  // I recommend setting following control register: 
  // 1. edge enhancement: REG_COM16, REG_EDGE
  // 2. denoise: REG_DNSTH
  // 3. black/white pixel correction: REG_REG76, REG_REG77
  // 4. Brightness and Contrast: REG_BRIGHT, REG_CONTRAS
  // 5. matrix coefficient: REG_CMATRIX1-6
  // 6. AEC/AGC control: REG_COM8, REG_COM9, REG_HAECC1-7
  // 7. gamma and UV saturation for better color: REG_COM13
  
  trace("i2c setup done\n");
}

void wait_pos_edge(uint32_t pin) {
  while (gpio_read(pin));
  while (!gpio_read(pin));
}

void wait_neg_edge(uint32_t pin) {
  while (!gpio_read(pin));
  while (gpio_read(pin));
}

uint8_t read_byte(void) {
  // Reaad D0-D7 as a byte. 
  // You can make this faster!
  uint8_t pixel = 0;
  pixel = gpio_read(D7) << 7 | gpio_read(D6) << 6 | gpio_read(D5) << 5 |
          gpio_read(D4) << 4 | gpio_read(D3) << 3 | gpio_read(D2) << 2 |
          gpio_read(D1) << 1 | gpio_read(D0);
  return pixel;
}

// Optimized version with bit masking instead of branching
static inline void extract_rgb565(uint32_t byte1, uint32_t byte2, 
                                   uint8_t *r, uint8_t *g, uint8_t *b) {
  // RGB565 format:
  // Byte 1: D7-D3 = Red[4:0], D2-D0 = Green[5:3]
  // Byte 2: D7-D5 = Green[2:0], D4-D0 = Blue[4:0]
  uint32_t d_mask_1 = (byte1 >> 6) & 0xFF;
  uint32_t d_mask_2 = (byte2 >> 6) & 0xFF;
  
  *r = (d_mask_1 & 0xF8);
  *g = ((d_mask_1 & 0x07) << 5) | ((d_mask_2 & 0xE0) >> 3);
  *b = (d_mask_2 & 0x1F) << 3;
}

void Camera_Tx_pixel(uint8_t *fb, uint32_t *height_offset) {
  uint32_t Stat;
  uint32_t t_start = 0, t_end = 0;
  const uint32_t WAIT_TIME_MS = 100;
  uint32_t pclk_mask = (1<<PCLK);
  uint8_t contrast = 0x50;
  

  while(1) {
    wait_neg_edge(VS);
    
    // Update offset for double buffering
    if (*height_offset == 0) {
      *height_offset = PHY_HEIGHT;
    } else {
      *height_offset = 0;
    }
    
    for (int i = 0; i < IMAGE_HEIGHT; i++) {
      wait_pos_edge(HS);
      int line_offset = (i + *height_offset) * BYTES_PER_ROW;
      
      for (int j = 0; j < IMAGE_WIDTH; j++) {
        uint32_t gpio1, gpio2;
        
        while(!(GPIO_READ_BANK0 & pclk_mask)) {
          Stat = GPIO_READ_BANK0; 
          if(Stat & pclk_mask) break;
        }
        gpio1 = GPIO_READ_BANK0;
        
        while(!(GPIO_READ_BANK0 & pclk_mask)) {
          Stat = GPIO_READ_BANK0; 
          if(Stat & pclk_mask) break;
        }
        gpio2 = GPIO_READ_BANK0;
        
        uint8_t r, g, b;
        extract_rgb565(gpio1, gpio2, &r, &g, &b);
        
        int fb_idx = line_offset + 4*j;
        fb[fb_idx] = b;
        fb[fb_idx + 1] = g;
        fb[fb_idx + 2] = r;
        fb[fb_idx + 3] = 0xF0;
      }
    }
    
    rpi_set_virtual_offset(0, *height_offset);
  }
}

void Camera_pclk_test(void) {
  uint32_t t_start, t_end;
  wait_neg_edge(VS);
  t_start = timer_get_usec();
  wait_neg_edge(VS);
  t_end = timer_get_usec();
  trace("Time spent in a camera frame: %d usec\n", t_end - t_start);
}