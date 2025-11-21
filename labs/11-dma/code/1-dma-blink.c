// use DMA to blink pin 27 on parthiv board.
//
// NOTE: 
//  - you'll have to implement <phys_to_bus>
//  - probably should use your <gpio-raw.h>
//
#include "rpi.h"
#include "dma-bcm2835.h"

enum { GPIO_BASE  = 0x20200000 };

void gpio_set_on_dma(dma_ch_t *dma, unsigned pin) {
    todo("do a dma write to SET0\n");
}

void gpio_set_off_dma(dma_ch_t *dma, unsigned pin) {
    todo("do a dma write to CLR0\n");
}

void notmain(void) {
    // parthiv board led
    enum { pin = 27 };
    gpio_set_output(pin);

    enum { N = 4 };

#if 0
    // if you want to test with normal GPIO to make sure
    // works.
    output("going to blink parthiv board\n");
    for(int i = 0; i < N; i++) {
        gpio_set_on(pin);
        delay_ms(1000);
        gpio_set_off(pin);
        delay_ms(1000);
    }
#endif

    enum { dma_ch = 4 };
    let dma = dma_init(dma_ch);

    output("going to blink with DMA\n");
    for(int i = 0; i < N; i++) {
        gpio_set_on_dma(dma, pin);
        delay_ms(1000);
        gpio_set_off_dma(dma, pin);
        delay_ms(1000);
    }
}
