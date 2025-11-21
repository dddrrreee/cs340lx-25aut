// an example: copies 4 bytes.  
// todo:
//  1. you should generalize so it copies more!
//  2. so it can do multiple blocks.
#include "rpi.h"
#include "dma-bcm2835.h"
#include "pi-random.h"

void notmain(void) {
    // 1. setup channel 4.  see <bcm2835-dma.h>
    // 
    // NOTE:
    //  - while there are 15 channels, many are used by firmware and
    //    periphs.  see <bcm2835-dma.h>
    // 
    //  - don't have to init before setting up control blocks.
    //  - don't have to do more than once: can keep reusing.
    enum { dma_ch = 4 };
    let dma = dma_init(dma_ch);

    uint32_t src, dst;

    // 2. make a single control block: 4-byte copy src->dst.
    // 
    // NOTE: 
    //   - will change the upper bits of memory.
    dma_cblk_t cb = cblk_mk(phys_to_bus(&dst), 
                                phys_to_bus(&src), 4);

    enum { val = 0xdeadbeef };
    src = val;

    // 3. Initiate the DMA transfer.  
    dma_initiate(dma, &cb);
    
    // 4. Transfer is not instantaneous so we have to poll 
    //    waiting.  We use an arbitrary timeout --- might
    //    have to grow as your data size does.
    //
    // NOTE: 
    //  - should also try with interrupts!
    enum { timeout = 1024 };
    if(!dma_wait(dma, timeout))
        panic("dma timed out!\n");

    // 5. check that it worked.
    if(dst != src || dst != val)
        panic("did not copy?  dst=%x, src=%x expected=%x\n",
            dst, src, val);

    // 6. done!
    output("SUCCESS!  copied 4 bytes.\n");
        
    enum { N = 128 };
    for(int i = 0; i < N; i++) {
        uint32_t exp = src = pi_random();

        let cb = cblk_mk(phys_to_bus(&dst),phys_to_bus(&src),4);
        if(!dma_run(dma, &cb, timeout))
            panic("timed out\n");

        if(dst != src || dst != exp)
            panic("did not copy?  dst=%x, src=%x expected=%x\n",
                dst, src, exp);
    }
    output("SUCCESS!  copied 4 bytes %d times.\n", N);
}
