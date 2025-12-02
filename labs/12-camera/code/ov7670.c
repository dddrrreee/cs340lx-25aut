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

  // set hardware window
  imu_wr(SLAVE_ADDR, REG_HSTART, 0x16);
  imu_wr(SLAVE_ADDR, REG_HSTOP, 0x04);
  imu_wr(SLAVE_ADDR, REG_HREF, 0x24);
  imu_wr(SLAVE_ADDR, REG_VSTART, 0x02);
  imu_wr(SLAVE_ADDR, REG_VSTOP, 0x7a);
  imu_wr(SLAVE_ADDR, REG_VREF, 0x0a);

  // no downscaling
  imu_wr(SLAVE_ADDR, REG_SCALEDCW, 0x00);

  // set internal clock to (input clock) / data + (0x00001)
  imu_wr(SLAVE_ADDR, REG_CLKRC, 0x00);

  // enable dcw (downsampling, crop, window)
  // Adding this will quantize all pixels, don't know why now
  // imu_wr(SLAVE_ADDR, REG_COM3, COM3_DCWEN);
  // delay_ms(WAIT_TIME_MS);

  // scaling pclk enable, pclk very slow
  imu_wr(SLAVE_ADDR, REG_SCALEPCLK, 0xF8);
  imu_wr(SLAVE_ADDR, REG_COM14, 0x18);

  // hsync rising/falling edge delay
  imu_wr(SLAVE_ADDR, REG_HSYST, 0x00);
  imu_wr(SLAVE_ADDR, REG_HSYEN, 0x00);

  // data format: RGB444
  imu_wr(SLAVE_ADDR, REG_COM7, COM7_RGB);
  imu_wr(SLAVE_ADDR, REG_COM15, COM15_RGB565 | COM15_R00FF);
  imu_wr(SLAVE_ADDR, REG_RGB444, 0x00);

  // Edge enhancement (was 0xFF=max, try moderate level)
  imu_wr(SLAVE_ADDR, REG_COM16, 0x28);
  imu_wr(SLAVE_ADDR, REG_EDGE, 0x00);  // Start with 0 (no enhancement), can increase if needed

  // DNSTH
  imu_wr(SLAVE_ADDR, 0x4C, 0x01);

  // REG76 (black/white pixel correction)
  imu_wr(SLAVE_ADDR, 0x76, 0xD0);

  imu_wr(SLAVE_ADDR, 0x77, 0x00);

  // matrix coefficient that I don't understand
  imu_wr(SLAVE_ADDR, REG_CMATRIX_BASE, 0xFF);
  imu_wr(SLAVE_ADDR, 0x50, 0x00);  // MTX2 - increased for more saturation
  imu_wr(SLAVE_ADDR, 0x51, 0x00);  // MTX3
  imu_wr(SLAVE_ADDR, 0x52, 0x10);  // MTX4 - controls color balance
  imu_wr(SLAVE_ADDR, 0x53, 0x72);  // MTX5 - controls color balance
  // imu_wr(SLAVE_ADDR, 0x54, 0x60);  // MTX6 - increased for more saturation
  // imu_wr(SLAVE_ADDR, 0x58, 0x9E);  // MTXS - matrix sign bits
  
  // Brightness and Contrast
  imu_wr(SLAVE_ADDR, REG_BRIGHT, 0x10);  // Brightness (0x00 = default)
  imu_wr(SLAVE_ADDR, REG_CONTRAS, 0x50);  // Contrast (0x40 = default)

  // AEC/AGC
  imu_wr(SLAVE_ADDR, REG_COM8, 0xC7);
  imu_wr(SLAVE_ADDR, REG_HAECC1, 0xC5);
  imu_wr(SLAVE_ADDR, REG_HAECC2, 0x68);
  imu_wr(SLAVE_ADDR, REG_HAECC3, 0xD8);
  imu_wr(SLAVE_ADDR, REG_HAECC4, 0xD8);
  imu_wr(SLAVE_ADDR, REG_HAECC5, 0xF0);
  imu_wr(SLAVE_ADDR, REG_HAECC6, 0x90);
  imu_wr(SLAVE_ADDR, REG_HAECC7, 0x94);

   // 128x gain ceiling + magic bits
  imu_wr(SLAVE_ADDR, REG_COM9, 0x0A);
  
  // Enable gamma and UV saturation for better color
  imu_wr(SLAVE_ADDR, REG_COM13, 0xC0);
  // imu_wr(SLAVE_ADDR, REG_COM13, COM13_GAMMA | COM13_UVSAT);
  
  trace("i2c setup done\n");
}

uint32_t img_raw_buffer_1[IMAGE_HEIGHT][IMAGE_WIDTH] = {0};
uint32_t img_raw_buffer_2[IMAGE_HEIGHT][IMAGE_WIDTH] = {0};

void wait_pos_edge(uint32_t pin) {
  while (gpio_read(pin))
    ;
  while (!gpio_read(pin))
    ;
}

void wait_neg_edge(uint32_t pin) {
  // trace("waiting high signal\n");
  while (!gpio_read(pin))
    ;
  // trace("waiting low signal\n");
  while (gpio_read(pin))
    ;
}

uint8_t read_byte(void) {
  uint8_t pixel = 0;
  pixel = gpio_read(D7) << 7 | gpio_read(D6) << 6 | gpio_read(D5) << 5 |
          gpio_read(D4) << 4 | gpio_read(D3) << 3 | gpio_read(D2) << 2 |
          gpio_read(D1) << 1 | gpio_read(D0);
  return pixel;
}

void Camera_pclk_test(void) {
  uint32_t t_start, t_end;
  // trace("Start capture\n");
  wait_neg_edge(VS);
  t_start = timer_get_usec();
  wait_neg_edge(VS);
  t_end = timer_get_usec();
  trace("Time spent in a camera frame: %d usec\n", t_end - t_start);
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
  // printk("%d", byte1 & 0b10000000);
}

void Camera_Tx_pixel(uint8_t *fb, uint32_t *height_offset) {
  uint32_t Stat;
  uint32_t t_start = 0, t_end = 0;
  const uint32_t WAIT_TIME_MS = 100;
  uint32_t pclk_mask = (1<<PCLK);
  uint8_t brightness = 0x10;
  uint8_t contrast = 0x50;
  uint32_t gpio_read = (*(unsigned int *)GPIO_READ_BANK0);

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
        
        while(!(gpio_read & pclk_mask)) {
          Stat = gpio_read; 
          if(Stat & pclk_mask) break;
        }
        gpio1 = gpio_read;
        
        while(!(gpio_read & pclk_mask)) {
          Stat = gpio_read; 
          if(Stat & pclk_mask) break;
        }
        gpio2 = gpio_read;
        
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

