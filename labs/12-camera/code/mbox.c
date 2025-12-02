#include "rpi.h"
#include "mbox.h"

// dump out the entire messaage.  useful for debug.
void msg_dump(const char *msg, volatile uint32_t *u, unsigned nwords) {
    printk("%s\n", msg);
    for(int i = 0; i < nwords; i++)
        output("u[%d]=%x\n", i,u[i]);
}

/*
  This is given.

  Get board serial
    Tag: 0x00010004
    Request: Length: 0
    Response: Length: 8
    Value: u64: board serial
*/
uint64_t rpi_get_serialnum(void) {
    // 16-byte aligned 32-bit array
    volatile uint32_t msg[8] __attribute__((aligned(16)));

    // make sure aligned
    assert((unsigned)msg%16 == 0);

    msg[0] = 8*4;       // total size in bytes.
    msg[1] = 0;         // sender: always 0.
    msg[2] = 0x00010004;  // serial tag
    msg[3] = 8;           // total bytes avail for reply
    msg[4] = 0;           // request code [0].
    msg[5] = 0;           // space for 1st word of reply 
    msg[6] = 0;           // space for 2nd word of reply 
    msg[7] = 0;   // end tag

    // send and receive message
    mbox_send(MBOX_CH, msg);

    // output("got:\n");
    // for(int i = 0; i < 8; i++)
    //     output("msg[%d]=%x\n", i, msg[i]);

    // should have value for success: 1<<31
    if(msg[1] != 0x80000000)
		panic("invalid response: got %x\n", msg[1]);

    // high bit should be set and reply size
    assert(msg[4] == ((1<<31) | 8));

    // for me the upper 32 bits were never non-zero.  
    // not sure if always true?
    assert(msg[6] == 0);
    
    return msg[5];
}

uint32_t rpi_get_memsize(void) {
    // todo("get the pi's physical memory size");
    volatile uint32_t msg[8] __attribute__((aligned(16)));
    assert((unsigned)msg%16 == 0);

    msg[0] = 8*4;       // total size in bytes.
    msg[1] = 0;         // sender: always 0.
    msg[2] = 0x00010005;  // serial tag
    msg[3] = 8;           // total bytes avail for reply
    msg[4] = 0;           // request code [0].
    msg[5] = 0;           // space for 1st word of reply 
    msg[6] = 0;           // space for 2nd word of reply 
    msg[7] = 0;   // end tag

    mbox_send(MBOX_CH, msg);

    // output("got:\n");
    // for(int i = 0; i < 8; i++)
    //     output("msg[%d]=%x\n", i, msg[i]);

    if(msg[1] != 0x80000000)
		panic("invalid response: got %x\n", msg[1]);

    assert(msg[4] == ((1<<31) | 8));
    
    return msg[6];
}


uint32_t rpi_get_model(void) {
    // todo("get the pi's model number");
    volatile uint32_t msg[8] __attribute__((aligned(16)));
    assert((unsigned)msg%16 == 0);

    msg[0] = 8*4;       // total size in bytes.
    msg[1] = 0;         // sender: always 0.
    msg[2] = 0x00010001;  // serial tag
    msg[3] = 4;           // total bytes avail for reply
    msg[4] = 0;           // request code [0].
    msg[5] = 0;           // space for 1st word of reply 
    msg[6] = 0;           // space for 2nd word of reply 
    msg[7] = 0;   // end tag

    mbox_send(MBOX_CH, msg);

    // output("got:\n");
    // for(int i = 0; i < 8; i++)
    //     output("msg[%d]=%x\n", i, msg[i]);

    if(msg[1] != 0x80000000)
		panic("invalid response: got %x\n", msg[1]);

    assert(msg[4] == ((1<<31) | 4));
    assert(msg[6] == 0);
    
    return msg[5];
}

// https://www.raspberrypi-spy.co.uk/2012/09/checking-your-raspberry-pi-board-version/
uint32_t rpi_get_revision(void) {
    // todo("get the pi's revision number");
    volatile uint32_t msg[8] __attribute__((aligned(16)));
    assert((unsigned)msg%16 == 0);

    msg[0] = 8*4;       // total size in bytes.
    msg[1] = 0;         // sender: always 0.
    msg[2] = 0x00010002;  // serial tag
    msg[3] = 4;           // total bytes avail for reply
    msg[4] = 0;           // request code [0].
    msg[5] = 0;           // space for 1st word of reply 
    msg[6] = 0;           // space for 2nd word of reply 
    msg[7] = 0;   // end tag

    mbox_send(MBOX_CH, msg);

    // output("got:\n");
    // for(int i = 0; i < 8; i++)
    //     output("msg[%d]=%x\n", i, msg[i]);

    if(msg[1] != 0x80000000)
		panic("invalid response: got %x\n", msg[1]);

    assert(msg[4] == ((1<<31) | 4));
    assert(msg[6] == 0);
    
    return msg[5];
}

uint32_t rpi_temp_get(void) {
    // todo("get the pi's temperature");
    volatile uint32_t msg[8] __attribute__((aligned(16)));
    assert((unsigned)msg%16 == 0);

    msg[0] = 8*4;       // total size in bytes.
    msg[1] = 0;         // sender: always 0.
    msg[2] = 0x00030006;  // serial tag
    msg[3] = 8;           // total bytes avail for reply
    msg[4] = 0;           // request code [0].
    msg[5] = 0;           // space for 1st word of reply 
    msg[6] = 0;           // space for 2nd word of reply 
    msg[7] = 0;   // end tag

    mbox_send(MBOX_CH, msg);

    // output("got:\n");
    // for(int i = 0; i < 8; i++)
    //     output("msg[%d]=%x\n", i, msg[i]);

    if(msg[1] != 0x80000000)
		panic("invalid response: got %x\n", msg[1]);

    assert(msg[4] == ((1<<31) | 8));
    
    return msg[6];
}

uint32_t rpi_clock_curhz_get(uint32_t clock) {
    volatile uint32_t msg[8] __attribute__((aligned(16)));
    assert((unsigned)msg%16 == 0);

    msg[0] = 8*4;       // total size in bytes.
    msg[1] = 0;         // sender: always 0.
    msg[2] = 0x00030002;  // serial tag
    msg[3] = 8;           // total bytes avail for reply
    msg[4] = 0;           // request code. (always 0)
    msg[5] = clock;
    msg[6] = 0;           // space for 1st word of reply 
    msg[7] = 0;           // space for 2nd word of reply 

    mbox_send(MBOX_CH, msg);

    // output("got:\n");
    // for(int i = 0; i < 8; i++)
    //     output("msg[%d]=%x\n", i, msg[i]);

    if(msg[1] != 0x80000000)
		panic("invalid response: got %x\n", msg[1]);

    assert(msg[4] == ((1<<31) | 8));
    
    return msg[6];
}


uint32_t rpi_clock_realhz_get(uint32_t clock) {
    volatile uint32_t msg[8] __attribute__((aligned(16)));
    assert((unsigned)msg%16 == 0);

    msg[0] = 8*4;       // total size in bytes.
    msg[1] = 0;         // sender: always 0.
    msg[2] = 0x00030047;  // serial tag
    msg[3] = 8;           // total bytes avail for reply
    msg[4] = 0;           // request code. (always 0)
    msg[5] = clock;
    msg[6] = 0;           // space for 1st word of reply 
    msg[7] = 0;           // space for 2nd word of reply 

    mbox_send(MBOX_CH, msg);

    output("got:\n");
    for(int i = 0; i < 8; i++)
        output("msg[%d]=%x\n", i, msg[i]);

    if(msg[1] != 0x80000000)
		panic("invalid response: got %x\n", msg[1]);

    assert(msg[4] == ((1<<31) | 8));
    
    return msg[6];
}

uint32_t rpi_clock_maxhz_get(uint32_t clock) {
    volatile uint32_t msg[8] __attribute__((aligned(16)));
    assert((unsigned)msg%16 == 0);

    msg[0] = 8*4;       // total size in bytes.
    msg[1] = 0;         // sender: always 0.
    msg[2] = 0x00030004;  // serial tag
    msg[3] = 8;           // total bytes avail for reply
    msg[4] = 0;           // request code. (always 0)
    msg[5] = clock;
    msg[6] = 0;           // space for 1st word of reply 
    msg[7] = 0;           // space for 2nd word of reply 

    mbox_send(MBOX_CH, msg);

    // output("got:\n");
    // for(int i = 0; i < 8; i++)
    //     output("msg[%d]=%x\n", i, msg[i]);

    if(msg[1] != 0x80000000)
		panic("invalid response: got %x\n", msg[1]);

    assert(msg[4] == ((1<<31) | 8));
    
    return msg[6];
}

uint32_t rpi_clock_minhz_get(uint32_t clock) {
    volatile uint32_t msg[8] __attribute__((aligned(16)));
    assert((unsigned)msg%16 == 0);

    msg[0] = 8*4;       // total size in bytes.
    msg[1] = 0;         // sender: always 0.
    msg[2] = 0x00030007;  // serial tag
    msg[3] = 8;           // total bytes avail for reply
    msg[4] = 0;           // request code. (always 0)
    msg[5] = clock;
    msg[6] = 0;           // space for 1st word of reply 
    msg[7] = 0;           // space for 2nd word of reply 

    mbox_send(MBOX_CH, msg);

    // output("got:\n");
    // for(int i = 0; i < 8; i++)
    //     output("msg[%d]=%x\n", i, msg[i]);

    if(msg[1] != 0x80000000)
		panic("invalid response: got %x\n", msg[1]);

    assert(msg[4] == ((1<<31) | 8));
    
    return msg[6];
}

uint32_t rpi_clock_hz_set(uint32_t clock, uint32_t hz) {
    volatile uint32_t msg[8] __attribute__((aligned(16)));
    assert((unsigned)msg%16 == 0);

    msg[0] = 8*4;       // total size in bytes.
    msg[1] = 0;         // sender: always 0.
    msg[2] = 0x00038002;  // serial tag
    msg[3] = 8;           // total bytes avail for reply
    msg[4] = 0;           // request code. (always 0)
    msg[5] = clock;
    msg[6] = hz;           // space for 1st word of reply 
    msg[7] = 0;           // space for 2nd word of reply 

    mbox_send(MBOX_CH, msg);

    if(msg[1] != 0x80000000)
		panic("invalid response: got %x\n", msg[1]);

    assert(msg[4] == ((1<<31) | 8));
    
    return msg[6];
}

void rpi_get_display_width_height(uint32_t *width, uint32_t *height) {
    volatile uint32_t msg[8] __attribute__((aligned(16)));
    assert((unsigned)msg%16 == 0);

    msg[0] = 8*4;       // total size in bytes.
    msg[1] = 0;         // sender: always 0.
    msg[2] = 0x00040003;  // serial tag
    msg[3] = 8;           // total bytes avail for reply
    msg[4] = 0;           // request code. (always 0)
    msg[5] = 0;           // space for 1st word of reply 
    msg[6] = 0;           // space for 2nd word of reply 
    msg[7] = 0;

    mbox_send(MBOX_CH, msg);

    if(msg[1] != 0x80000000)
		panic("invalid response: got %x\n", msg[1]);

    assert(msg[4] == ((1<<31) | 8));
    *width = msg[5];
    *height = msg[6];
}

void rpi_set_display_width_height(uint32_t *width, uint32_t *height) {
    volatile uint32_t msg[8] __attribute__((aligned(16)));
    assert((unsigned)msg%16 == 0);

    msg[0] = 8*4;       // total size in bytes.
    msg[1] = 0;         // sender: always 0.
    msg[2] = 0x00048003;  // serial tag
    msg[3] = 8;           // total bytes avail for reply
    msg[4] = 0;           // request code. (always 0)
    msg[5] = *width;           // space for 1st word of reply 
    msg[6] = *height;           // space for 2nd word of reply 
    msg[7] = 0;

    mbox_send(MBOX_CH, msg);

    if(msg[1] != 0x80000000)
		panic("invalid response: got %x\n", msg[1]);

    assert(msg[4] == ((1<<31) | 8));
    *width = msg[5];
    *height = msg[6];
}

void rpi_set_virtual_width_height(uint32_t *width, uint32_t *height) {
    volatile uint32_t msg[8] __attribute__((aligned(16)));
    assert((unsigned)msg%16 == 0);

    msg[0] = 8*4;       // total size in bytes.
    msg[1] = 0;         // sender: always 0.
    msg[2] = 0x00048004;  // serial tag
    msg[3] = 8;           // total bytes avail for reply
    msg[4] = 0;           // request code. (always 0)
    msg[5] = *width;           // space for 1st word of reply 
    msg[6] = *height;           // space for 2nd word of reply 
    msg[7] = 0;

    mbox_send(MBOX_CH, msg);

    if(msg[1] != 0x80000000)
		panic("invalid response: got %x\n", msg[1]);

    assert(msg[4] == ((1<<31) | 8));
    *width = msg[5];
    *height = msg[6];
}

uint32_t rpi_get_depth(void) {
    volatile uint32_t msg[8] __attribute__((aligned(16)));
    assert((unsigned)msg%16 == 0);

    msg[0] = 8*4;       // total size in bytes.
    msg[1] = 0;         // sender: always 0.
    msg[2] = 0x00048005;  // serial tag
    msg[3] = 4;           // total bytes avail for reply
    msg[4] = 0;           // request code. (always 0)
    msg[5] = 0;           // space for 1st word of reply 
    msg[6] = 0;           // space for 2nd word of reply 
    msg[7] = 0;

    mbox_send(MBOX_CH, msg);

    if(msg[1] != 0x80000000)
		panic("invalid response: got %x\n", msg[1]);
    assert(msg[4] == ((1<<31) | 4));

    return msg[5];
}

void rpi_set_virtual_offset(uint32_t width_offset, uint32_t height_offset) {
  volatile uint32_t msg[8] __attribute__((aligned(16)));
    assert((unsigned)msg%16 == 0);

    msg[0] = 8*4;       // total size in bytes.
    msg[1] = 0;         // sender: always 0.
    msg[2] = 0x00048009;  // serial tag
    msg[3] = 8;           // total bytes avail for reply
    msg[4] = 0;           // request code. (always 0)
    msg[5] = width_offset;           // space for 1st word of reply 
    msg[6] = height_offset;           // space for 2nd word of reply 
    msg[7] = 0;

    mbox_send(MBOX_CH, msg);

    if(msg[1] != 0x80000000)
		panic("invalid response: got %x\n", msg[1]);
    // assert(msg[4] == ((1<<31) | 4));
    // output("width_off: %d, height_off: %d\n", msg[5], msg[6]);
}


uint8_t* device_initialize(uint32_t phy_width, uint32_t phy_height, 
                    uint32_t vir_width, uint32_t vir_height, 
                    uint32_t depth, uint32_t pixel_order) {
   __attribute__((aligned(16))) volatile uint32_t msg[32] = {
      128,               // [0] total size in bytes
      0,                 // [1] request = 0 (becomes 0x80000000 on success)

      // 1. Set physical width, height (8 bytes)
      0x00048003, 8, 0,  // [2..4]
      phy_width, phy_height,     // [5..6]: width and height for display

      // 2. Set buffer width, height
      0x00048004, 8, 0,  // [7..9]
      vir_width, vir_height,     // [10..11]: width and height for buffer

      // 3. Set depth
      0x00048005, 4, 0,  // [12..14]
      depth,     // [15]: depth

      // 4. Set pixel order
      0x00048006, 4, 0,  // [16..18]
      pixel_order,         // [19]: pixel order

      // 5. Allocate frame buffer 
      0x00040001, 8, 0,  // [20..22]
      4000, 0,      // [23..24]: buffer base address, buffer size

      // 6. Bytes per row
      0x00040008, 4, 0,  // [25..27]
      0,         // [28]: bytes per row


      // ...  the other tags from steps 2-6 ...

      0,                 // [29] end tag
    };

    mbox_send(MBOX_CH, msg);
    if(msg[1] != 0x80000000)
		panic("invalid response: got %x\n", msg[1]);

    // Double check physical width/height
    output("physical width = %d, physical height = %d\n", msg[5], msg[6]);

    // Double check buffer width/height
    output("buffer width = %d, buffer height = %d\n", msg[10], msg[11]);

    // Double check depth
    output("depth = %d\n", msg[15]);

    // Double check pixel order (should be RGB, 1)
    output("pixel order = %d\n", msg[19]);

    // Frame Buffer address
    uint32_t fb_raw = msg[23];
    uint8_t *fb = (void*)(fb_raw & 0x3FFFFFFF);
    output("fb raw address = %x, pi address = %x\n", fb_raw, fb);

    // Frame buffer size in byte
    output("Fb size = %d\n", msg[24]);

    // Number of bytes per row
    output("Bytes per row = %d\n", msg[28]);

    return fb;
}