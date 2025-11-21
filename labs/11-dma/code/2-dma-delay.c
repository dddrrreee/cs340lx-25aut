// Goal: make a DMA chain that takes a precise number of cycles.
// Why: we need to do 6076 cycles per write.
//
// ideally you use your scope to work out exactly how long to 
// delay.  for today if you want to do something quick and dirty
// you can try to make a two block chain that 
//  - block 1: dma write of 4 bytes [similating gpio write]
//  - block 2: does a delay by writing N bytes + delays
//
// NOTE: if you just measure the whole DMA process from start to 
// finish the number gets inflated b/c you're also measuring
// DMA inititation and completion overhead.  My weak attempt to
// seperate that was to measure the cost of copying 4 bytes as 
// the baseline and then measure how much more each additional 
// 4 block added.  This is not the right way, but for slow UART
// might not matter.

#include "rpi.h"
#include "dma-bcm2835.h"
#include "cycle-count.h"


// helper to chain blocks together.
static void dma_chain(dma_cblk_t *p, dma_cblk_t *next) {
    p->NEXT_CB = phys_to_bus(next);
}

// you can do this however you want.  Probably what
// you should do is look at the TI register (p50) and
// play around with the different fields to see how
// to (1) slow down a transfer and (2) reduce memory
// traffic.
// 
// Useful: 
//   - delay: WAITS bits(21:25)
//   - wide bursts: bit(26)
//   - ignore src or dst
//   - don't increment source (bit(8)
//   - don't increment dst (bit(4))
//    - wait for response (bit(3))
//  - some others: i didn't mess around too much.
//
// Also useful: 
//   - trim out some of the fat for initiation and wait.
//
// NOTE: 
//   1. you probably don't want to increment source and dst.
//   2. to reduce bus traffic it's good to either ignore
//      dst or src (but not both)
//   3. I couldn't get delays to add enough so I copied bytes
//      too.
//
static uint32_t measure_two_blk(dma_ch_t *dma, uint32_t n) {
    todo("implement\n");
}

void notmain(void) {
    // caches_enable();

    enum { dma_ch = 4 };
    let dma = dma_init(dma_ch);

    // for me: baseline was about 1580 cycles

    // warmup
    output("4byte = %d\n", measure_two_blk(dma,4));
    output("4byte = %d\n", measure_two_blk(dma,4));
    let baseline = measure_two_blk(dma,4);
    output("baseline=%d\n", baseline);

    for(unsigned i = 1, n = 4; n < 512; n += 4, i++) {
        output("running DMA with nbytes = %d\n", n);
        let c = measure_two_blk(dma,n);
        output("copied %d bytes took %d cycles, incremental=%d\n", 
            n, 
            c, 
            (c-baseline)/i);
    }
}
