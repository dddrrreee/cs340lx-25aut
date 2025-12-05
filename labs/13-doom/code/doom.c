#include "rpi.h"

// Import drivers for frame buffer, interrupts, DMA, etc.

#include "PureDOOM.h"

#define DOOM_IMPLEMENTATION 

void render_frame(uint32_t *frame) {
    todo("implement this\n");
}

void play_doom(void) {
    // Initialize DOOM
    int argc = 1;
    char* argv[] = {"doom", NULL};
    doom_init(argc, argv, 0);

    todo("initialize drivers\n");

    while (1) {
        // Perform game update
        doom_update();

        // NOTE: we changed doom_get_framebuffer to output pixels in BGR format to work with our frame buffer code
        uint32_t *doom_frame = (uint32_t *) doom_get_framebuffer(4);  
        render_frame(doom_frame);
    }
}

void notmain(void) {
    kmalloc_init_set_start((void *)(1024*1024*2), FAT32_HEAP_MB*1024*1024);
    play_doom();
}
