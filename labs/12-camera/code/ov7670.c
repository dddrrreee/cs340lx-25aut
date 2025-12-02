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
  uint8_t byte = 0;
  byte = gpio_read(D7) << 7 | gpio_read(D6) << 6 | gpio_read(D5) << 5 |
         gpio_read(D4) << 4 | gpio_read(D3) << 3 | gpio_read(D2) << 2 |
         gpio_read(D1) << 1 | gpio_read(D0);
  return byte;
}

// Get pixel values from gpio inputs
static inline struct RGB extract_rgb565() {
  struct RGB pixel;
  uint8_t byte1, byte2;

  // Get two bytes from gpio inputs
  // When you see a pclk rising edge, get a byte
  // Use wait_pos_edge() and wait_neg_edge(), or use GPIO_READ_BANK0 defined in ov7670.h
  // to make it faster
  todo("get byte1 and byte2");

  // Assign pixel's value (r, g, b) by byte 1 and byte 2 
  // RGB565 format:
  // Byte 1: D7-D3 = Red[4:0], D2-D0 = Green[5:3]
  // Byte 2: D7-D5 = Green[2:0], D4-D0 = Blue[4:0]
  todo("assign pixel.r, pixel.g, and pixel.b");

  return pixel;
}

void camera_display(uint8_t *fb, uint32_t *height_offset) {
  while(1) {
    wait_neg_edge(VS);
    
    // Update offset for double buffering
    if (*height_offset == 0) {
      *height_offset = PHY_HEIGHT;
    } else {
      *height_offset = 0;
    }
    
    // Get pixels from camera
    for (int i = 0; i < IMAGE_HEIGHT; i++) {
      wait_pos_edge(HS);
      int line_offset = (i + *height_offset) * BYTES_PER_ROW;
      
      for (int j = 0; j < IMAGE_WIDTH; j++) {
        struct RGB pixel = extract_rgb565();
        int fb_idx = line_offset + 4*j;
        // assign pixel to frame buffer
        fb[fb_idx] = pixel.b;
        fb[fb_idx + 1] = pixel.g;
        fb[fb_idx + 2] = pixel.r;
        fb[fb_idx + 3] = 0xF0;
      }
    }
    
    rpi_set_virtual_offset(0, *height_offset);
  }
}

// Print pixels' red component. 
// You can use the printed numbers to generate a .jpg image. 
void print_camera_value() {
  uint8_t img_raw_buffer[(IMAGE_HEIGHT+1)/20 * (IMAGE_WIDTH+1)/20] = {0};
  wait_neg_edge(VS);
  
  // Get pixels from camera
  for (int i = 0; i < IMAGE_HEIGHT; i++) {
    wait_pos_edge(HS);
    
    for (int j = 0; j < IMAGE_WIDTH; j++) {
      struct RGB pixel = extract_rgb565();
      // assign pixel to frame buffer
      if ((j % 20) == 0 && (i % 20) == 0) {
        img_raw_buffer[i/20*(IMAGE_WIDTH/20) + j/20] = pixel.r;
      }
    }
  }

  // print pixel
  for (int i = 0; i < (IMAGE_HEIGHT+1)/20; i++) {
    for (int j = 0; j < (IMAGE_WIDTH+1)/20; j++) {
      printk("%d ,", img_raw_buffer[i * (IMAGE_WIDTH/20) + j]);
    }
    printk("\n");
  }
}