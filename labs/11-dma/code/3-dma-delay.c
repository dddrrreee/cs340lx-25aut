// same as 2-dma-delay.c except we use an arena for allocation:
// in general we wouldn't want to do stack allocation since if
// we return from the routine that did it, we'll have memory 
// corruption.
//
// NOTE: if you have memory corruption now, make sure you are 
// disabling auto-increment on the source/destination.
#include "rpi.h"
#include "dma-bcm2835.h"
#include "cycle-count.h"

#include "arena.h"

static arena_t *cb_arena;

static dma_cblk_t *cblk_new(
    volatile const void *dst,
    volatile const void *src,
    unsigned nbytes) {
    
    let cb = cblk_mk(phys_to_bus(dst), phys_to_bus(src), nbytes);

    dma_cblk_t *p = arena_alloc_aligned(cb_arena, sizeof cb, 32);
    assert((uint32_t)p%32 == 0);
    *p = cb;

    return p;
}


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
//
// NOTE: 
//   1. you probably don't want to increment source and dst.
//   2. to reduce bus traffic it's good to either ignore
//      dst or src (but not both)
//   3. I couldn't get delays to add enough so I copied bytes
//      too.
//
static uint32_t measure_two_blk(dma_ch_t *dma, uint32_t nbytes) {
    // dma->CS |= 0b111 << 16;

    assert(nbytes%4 == 0);
    volatile uint32_t dummy1 = 0, dummy2 = ~0;
    volatile uint32_t v = ~0, zero = 0;
        
    dma_cblk_t *cb1 = cblk_new(&v, &zero, 4);

    dma_cblk_t *cb2 = cblk_new(&dummy1, &dummy2, nbytes);


    cb2->TI = 0;            // reset TI to not increment src and dst.
    cb2->TI = (0b11111) << 21;     // max delay p 51
    todo("fill this in and try to hit our number\n");

    dma_chain(cb1, cb2);

    // measure about how long it takes to do the write.
    // best is if you do your scope to see the actual results.
    let s = cycle_cnt_read();
    dma_initiate_raw(dma, cb1);
    while(!dma_done(dma))
        ;
    let e = cycle_cnt_read();
    return e-s;
}


void notmain(void) {
    kmalloc_init(64);
    cb_arena = arena_mk(32*1024*1024);

    caches_enable();

    enum { dma_ch = 4 };
    let dma = dma_init(dma_ch);

    // baseline is about 1580 cycles
    output("about to run\n");

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


        // deallocate the arena
        arena_reset(cb_arena);
    }
}
