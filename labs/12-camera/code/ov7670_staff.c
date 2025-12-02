#include "rpi.h"
#include "ov7670.h"
#include "bit-support.h"
#include "gpio.h"
#include "i2c.h"
#include "mbox.h"

const uint32_t WAIT_TIME_MS = 100;
// read a single device register <reg> from i2c device
// <addr> and return the result.
uint8_t imu_rd(uint8_t addr, uint8_t reg) {
  i2c_write(addr, &reg, 1);
  uint8_t v;
  i2c_read(addr, &v, 1);
  return v;
}

// set a single device register <reg> on device
// <addr> to value <v>
//
// the operation is sent over i2c as two 8-bit values:
// (byte 0 = <reg>, byte 1 = <v>)
void imu_wr(uint8_t addr, uint8_t reg, uint8_t v) {
  uint8_t data[2];
  data[0] = reg;
  data[1] = v;
  i2c_write(addr, data, 2);
}

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

  // Set generated clock freq

  gpio_set_function(MCLK, GPIO_FUNC_ALT0);
  PUT32(GP1_CLK_DIV, 0x5A064000);
  PUT32(GP1_CLK_CTRL, 0x5A000000);
  while(bit_is_on(GET32(GP1_CLK_CTRL), 7));
  PUT32(GP1_CLK_CTRL, GET32(GP1_CLK_CTRL) | 0x5A000006);
  PUT32(GP1_CLK_CTRL, GET32(GP1_CLK_CTRL) | 0x5A000010);

  assert(bit_is_on(GET32(GP1_CLK_CTRL), 7));
}

void camera_i2c_setup(void) {
  i2c_init();
  delay_ms(WAIT_TIME_MS);
  output("i2c init completed\n");
  uint32_t PID_MSB = imu_rd(SLAVE_ADDR, 0x0A);
  assert(PID_MSB == 0x76);
  uint32_t PID_LSB = imu_rd(SLAVE_ADDR, 0x0B);
  assert(PID_LSB == 0x73);
}

void camera_register_setup(void) {
  output("Start camera register setup...\n");

  // Regster reset
  imu_wr(SLAVE_ADDR, REG_COM7, COM7_RESET);
  delay_ms(WAIT_TIME_MS);

  // set hardware window
  imu_wr(SLAVE_ADDR, REG_HSTART, 0x16);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_HSTOP, 0x04);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_HREF, 0x24);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_VSTART, 0x02);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_VSTOP, 0x7a);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_VREF, 0x0a);
  delay_ms(WAIT_TIME_MS);

  // no downscaling
  imu_wr(SLAVE_ADDR, REG_SCALEDCW, 0x00);
  delay_ms(WAIT_TIME_MS);

  // set internal clock to (input clock) / data + (0x00001)
  imu_wr(SLAVE_ADDR, REG_CLKRC, 0x00);
  delay_ms(WAIT_TIME_MS);

  // enable dcw (downsampling, crop, window)
  // Adding this will quantize all pixels, don't know why now
  // imu_wr(SLAVE_ADDR, REG_COM3, COM3_DCWEN);
  // delay_ms(WAIT_TIME_MS);

  // scaling pclk enable, pclk very slow
  imu_wr(SLAVE_ADDR, REG_SCALEPCLK, 0xF8);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_COM14, 0x18);
  delay_ms(WAIT_TIME_MS);

  // hsync rising/falling edge delay
  imu_wr(SLAVE_ADDR, REG_HSYST, 0x00);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_HSYEN, 0x00);
  delay_ms(WAIT_TIME_MS);

  // data format: RGB444
  imu_wr(SLAVE_ADDR, REG_COM7, COM7_RGB);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_COM15, COM15_RGB565 | COM15_R00FF);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_RGB444, 0x00);
  delay_ms(WAIT_TIME_MS);

  // Edge enhancement (was 0xFF=max, try moderate level)
  imu_wr(SLAVE_ADDR, REG_COM16, 0x28);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_EDGE, 0x00);  // Start with 0 (no enhancement), can increase if needed
  delay_ms(WAIT_TIME_MS);

  // DNSTH
  imu_wr(SLAVE_ADDR, 0x4C, 0x01);
  delay_ms(WAIT_TIME_MS);

  // REG76 (black/white pixel correction)
  imu_wr(SLAVE_ADDR, 0x76, 0xD0);
  delay_ms(WAIT_TIME_MS);

  imu_wr(SLAVE_ADDR, 0x77, 0x00);
  delay_ms(WAIT_TIME_MS);

  // matrix coefficient that I don't understand
  imu_wr(SLAVE_ADDR, REG_CMATRIX_BASE, 0xFF);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, 0x50, 0x00);  // MTX2 - increased for more saturation
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, 0x51, 0x00);  // MTX3
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, 0x52, 0x10);  // MTX4 - controls color balance
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, 0x53, 0x72);  // MTX5 - controls color balance
  delay_ms(WAIT_TIME_MS);
  // imu_wr(SLAVE_ADDR, 0x54, 0x60);  // MTX6 - increased for more saturation
  // delay_ms(WAIT_TIME_MS);
  // imu_wr(SLAVE_ADDR, 0x58, 0x9E);  // MTXS - matrix sign bits
  // delay_ms(WAIT_TIME_MS);
  
  // Brightness and Contrast
  imu_wr(SLAVE_ADDR, REG_BRIGHT, 0x10);  // Brightness (0x00 = default)
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_CONTRAS, 0x50);  // Contrast (0x40 = default)
  delay_ms(WAIT_TIME_MS);

  // I don't understand
  // imu_wr(SLAVE_ADDR, REG_TSLB, TSLB_YLAST);
  // delay_ms(WAIT_TIME_MS);
  // imu_wr(SLAVE_ADDR, REG_COM10, COM10_PCLK_HB);
  // delay_ms(WAIT_TIME_MS);

  // AEC/AGC
  imu_wr(SLAVE_ADDR, REG_COM8, 0xC7);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_HAECC1, 0xC5);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_HAECC2, 0x68);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_HAECC3, 0xD8);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_HAECC4, 0xD8);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_HAECC5, 0xF0);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_HAECC6, 0x90);
  delay_ms(WAIT_TIME_MS);
  imu_wr(SLAVE_ADDR, REG_HAECC7, 0x94);
  delay_ms(WAIT_TIME_MS);

   // 128x gain ceiling + magic bits
  imu_wr(SLAVE_ADDR, REG_COM9, 0x0A);
  delay_ms(WAIT_TIME_MS);
  
  // Enable gamma and UV saturation for better color
  imu_wr(SLAVE_ADDR, REG_COM13, 0xC0);
  // imu_wr(SLAVE_ADDR, REG_COM13, COM13_GAMMA | COM13_UVSAT);
  delay_ms(WAIT_TIME_MS);
  
  output("i2c setup done\n");
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

