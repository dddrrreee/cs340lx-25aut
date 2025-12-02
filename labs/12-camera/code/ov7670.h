// Camera Pins
#define SDA 2 // should be the same as your i2c.c
#define SCL 3 // should be the same as your i2c.c
#define PCLK 4
#define MCLK 5
#define D0 6
#define D1 7
#define D2 8
#define D3 9
#define D4 10
#define D5 11
#define D6 12
#define D7 13
#define VS 17
#define RST 19
#define HS 27

// Master clock setup
#define GP1_CLK_CTRL 0x20101078
#define GP1_CLK_DIV 0x2010107c


#define IMAGE_HEIGHT 480
#define IMAGE_WIDTH 640

#define GPIO_READ_BANK0 (*(unsigned int *)0x20200034)

// OV7670 device address: 0x42 shift right by 1 (OV7670 p.11)
#define SLAVE_ADDR 0x21

// Registers
#define REG_GAIN	0x00	/* Gain lower 8 bits (rest in vref) */
#define REG_BLUE	0x01	/* blue gain */
#define REG_RED		0x02	/* red gain */
#define REG_VREF	0x03	/* Pieces of GAIN, VSTART, VSTOP */
#define REG_COM1	0x04	/* Control 1 */
#define REG_BAVE	0x05	/* U/B Average level */
#define REG_GbAVE	0x06	/* Y/Gb Average level */
#define REG_AECHH	0x07	/* AEC MS 5 bits */
#define REG_RAVE	0x08	/* V/R Average level */
#define REG_COM2	0x09	/* Control 2 */
#define REG_PID		0x0a	/* Product ID MSB */
#define REG_VER		0x0b	/* Product ID LSB */
#define REG_COM3	0x0c	/* Control 3 */
#define REG_COM4	0x0d	/* Control 4 */
#define REG_COM5	0x0e	/* All "reserved" */
#define REG_COM6	0x0f	/* Control 6 */
#define REG_AECH	0x10	/* More bits of AEC value */
#define REG_CLKRC	0x11	/* Clocl control */
#define REG_COM7	0x12	/* Control 7 */
#define   COM7_RESET	  0x80	  /* Register reset */
#define	  COM7_RGB	      0x04	  /* bits 0 and 2 - RGB format */
#define REG_COM8	0x13	/* Control 8 */
#define REG_COM9	0x14	/* Control 9  - gain ceiling */
#define REG_COM10	0x15	/* Control 10 */
#define REG_HSTART	0x17	/* Horiz start high bits */
#define REG_HSTOP	0x18	/* Horiz stop high bits */
#define REG_VSTART	0x19	/* Vert start high bits */
#define REG_VSTOP	0x1a	/* Vert stop high bits */
#define REG_PSHFT	0x1b	/* Pixel delay after HREF */
#define REG_MIDH	0x1c	/* Manuf. ID high */
#define REG_MIDL	0x1d	/* Manuf. ID low */
#define REG_MVFP	0x1e	/* Mirror / vflip */
#define ADCCTR0     0x20    /* ADC control*/
#define REG_AEW		0x24	/* AGC upper limit */
#define REG_AEB		0x25	/* AGC lower limit */
#define REG_VPT		0x26	/* AGC/AEC fast mode op region */
#define REG_HSYST	0x30	/* HSYNC rising edge delay */
#define REG_HSYEN	0x31	/* HSYNC falling edge delay */
#define REG_HREF	0x32	/* HREF pieces */
#define REG_TSLB	0x3a	/* lots of stuff */
#define REG_COM11	0x3b	/* Control 11 */
#define REG_COM12	0x3c	/* Control 12 */
#define REG_COM13	0x3d	/* Control 13 */
#define   COM13_GAMMA	  0x80	  /* Gamma enable */
#define	  COM13_UVSAT	  0x40	  /* UV saturation auto adjustment */
#define REG_COM14	0x3e	/* Control 14 */
#define REG_EDGE	0x3f	/* Edge enhancement factor */
#define REG_COM15	0x40	/* Control 15 */
#define   COM15_R00FF	  0xc0	  /* Data range 00 to FF */
#define   COM15_RGB565	  0x10	  /* RGB565 output */
#define REG_COM16	0x41	/* Control 16 */
#define REG_COM17	0x42	/* Control 17 */
#define REG_DNSTH   0x4C    /* Denoise Strength */
#define	REG_CMATRIX1  0x4f
#define	REG_CMATRIX2  0x50
#define	REG_CMATRIX3  0x51
#define	REG_CMATRIX4  0x52
#define	REG_CMATRIX5  0x53
#define	REG_CMATRIX6  0x54
#define REG_BRIGHT	0x55	/* Brightness */
#define REG_CONTRAS	0x56	/* Contrast control */
#define REG_CMATRIX_SIGN  0x58
#define REG_GFIX	0x69	/* Fix gain control */
#define REG_DBLV	0x6b	/* PLL control an debugging */
#define REG_SCALEDCW 0x72
#define REG_SCALEPCLK 0x73
#define REG_REG76	0x76   /* black/white pixel correction */
#define REG_REG77   0x77   /* denoise offset */
#define REG_RGB444	0x8c	/* RGB 444 control */
#define REG_HAECC1	0x9f	/* Hist AEC/AGC control 1 */
#define REG_HAECC2	0xa0	/* Hist AEC/AGC control 2 */
#define REG_BD50MAX	0xa5	/* 50hz banding step limit */
#define REG_HAECC3	0xa6	/* Hist AEC/AGC control 3 */
#define REG_HAECC4	0xa7	/* Hist AEC/AGC control 4 */
#define REG_HAECC5	0xa8	/* Hist AEC/AGC control 5 */
#define REG_HAECC6	0xa9	/* Hist AEC/AGC control 6 */
#define REG_HAECC7	0xaa	/* Hist AEC/AGC control 7 */
#define REG_BD60MAX	0xab	/* 60hz banding step limit */


extern uint32_t img_raw_buffer_1[IMAGE_HEIGHT][IMAGE_WIDTH];
extern uint32_t img_raw_buffer_2[IMAGE_HEIGHT][IMAGE_WIDTH];


// RGB565 format:
// Byte 1: D7-D3 = Red[4:0], D2-D0 = Green[5:3]
// Byte 2: D7-D5 = Green[2:0], D4-D0 = Blue[4:0]
struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

uint8_t imu_rd(uint8_t addr, uint8_t reg);
void imu_wr(uint8_t addr, uint8_t reg, uint8_t v);
void camera_pins_setup(void);
void camera_i2c_setup(void);
void camera_register_setup(void);
void wait_pos_edge(uint32_t pin);
void wait_neg_edge(uint32_t pin);
uint8_t read_byte(void);
void camera_display(uint8_t *fb, uint32_t *height_offset);
void print_camera_value(void);