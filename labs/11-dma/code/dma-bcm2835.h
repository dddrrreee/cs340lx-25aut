#ifndef __DMA_BCM2835_H__
#define __DMA_BCM2835_H__
// example of wrapping up the BCM2835 DMA.  feel free to do it however
// you want.
//
// Missing:
//  1. you'll have to add support for multiple blocks.  make sure
//     the next pointer is non-allocating, coherent.
//  2. see the note below on address translation to bus addresses.
//     the code does this automatically to try to reduce errors,
//     but this might lead to more later: your call to remove or keep.
#include "bit-support.h"

//**************************************************************
// helpers for manual translation of addresses:
//
//   The BCM2835 usually translates all physical addresses
//   to have upper bits needed by the bus.  However, DMA 
//   is on the other side of this translation so we have to 
//   do it manually.
//
// We need:
//   1. bus addresses for GPIO (<BUS_MASK> below).
//   2. not-allocating L2 Coherent for physical memory.
// 
// NOTE: there's a lot of wrong information out there.
// you would think that using uncached memory instead of
// non-allocating L2 would always be safe, but unfortunately 
// it breaks.
//  - the GPU uses the L2 cache for its memory.
//  1. if the GPU puts data X in the L2 and X is not
//     yet flushed to memory, then:
//  2. a dma transfer that accesses X using uncached addresses
//     will not see it.

enum { 
    // bcm peripheral base addr
    BCM_BASE  = 0x20000000,     
    // non-allocating L2 coherent
    // bitwise-or w/ phys addr to make it 
    PHYS_MASK = 0x40000000,
    // Peripheral bus mask
    BUS_MASK =  0x7E000000  
};

typedef struct {
    uint32_t addr;
} dma_addr_t;

static inline dma_addr_t dma_addr_mk(uint32_t addr) {
    return (dma_addr_t){ .addr = addr };
}


static dma_addr_t bcm_to_bus(uint32_t addr) { 
    todo("implement this!");
}
static int phys_addr(const volatile void * addr) 
    { return addr < (void*)(512*1024*1024); }

static dma_addr_t phys_to_bus(const volatile void *addr) { 
    assert(phys_addr(addr));
    return dma_addr_mk((uint32_t)addr | PHYS_MASK); 
}

//*******************************************************
// actual DMA structures.

// the DMA channel itself: p39, 
typedef struct {
    // all addrs must be manually translated or will get tricky
    // errors (see note below on l2, non-allocating)
    volatile uint32_t   
                CS;         //  0: control and status (p47)
    volatile dma_addr_t
                CB_ADDR;    //  4: control block address (p50)

    volatile uint32_t   
                TI,         //  8: transfer info: lots of fields (p50)
                SRC_ADDR,   //  c: src addr: updated during dma p53
                DST_ADDR,   // 10: dst addr: updated during dma p53
                TXFR_LEN,   // 14: transfer in bytes p53
                ignore0;    // 18
    
    volatile dma_addr_t
                NEXT_CB;    // 1c: note this must be a bus addr.

    volatile uint32_t
                DEBUG;      // 20
} dma_ch_t;

_Static_assert(offsetof(dma_ch_t, CS) == 0,       "Bad offset");
_Static_assert(offsetof(dma_ch_t, CB_ADDR) == 4, "Bad offset");
_Static_assert(offsetof(dma_ch_t, TI) == 8, "Bad offset");
_Static_assert(offsetof(dma_ch_t, SRC_ADDR) == 0xc, "Bad offset");
_Static_assert(offsetof(dma_ch_t, DST_ADDR) == 0x10, "Bad offset");
_Static_assert(offsetof(dma_ch_t, TXFR_LEN) == 0x14, "Bad offset");
_Static_assert(offsetof(dma_ch_t, NEXT_CB) == 0x1c, "Bad offset");
_Static_assert(offsetof(dma_ch_t, DEBUG) == 0x20, "Bad offset");


// DMA control block (p40).  You feed a list of one or more
// of these to a DMA engine and wait until its done.
//
// NOTE: 
//  - CRUCIAL: must be 256-bit aligned
//  - we ignore STRIDE usually (slow). 
typedef struct {
    // we make the fields volatile so the pointer doesn't
    // have to be. 
    volatile uint32_t
             TI;            //  0: CB word 0 (transform info)
    volatile dma_addr_t
             SRC_ADDR,     //  4: cb word 1 (source addr)
             DST_ADDR;       //  8: cb word 2 (dest addr)
    volatile uint32_t
             TXFR_LEN,      //  c: cb word 3 (transfer length)
             STRIDE;        // 10: cb word 4 (2d stride)
    volatile dma_addr_t
             NEXT_CB;     // 14: cb word 5 (next cb addr)
    volatile uint32_t
             ignore0,
             ignore1;
#if 0
             DEBUG;         // 18: debug
#endif
} __attribute__((aligned(256/8))) dma_cblk_t;

// p40
// must be 256-bit aligned (byte?)
#define cblk_aligned(addr) ((uint32_t)addr % 32 == 0)

_Static_assert(offsetof(dma_cblk_t, TI) ==  0x0,       "Bad offset");
_Static_assert(offsetof(dma_cblk_t, SRC_ADDR) == 0x4, "Bad offset");
_Static_assert(offsetof(dma_cblk_t, DST_ADDR) == 0x8, "Bad offset");
_Static_assert(offsetof(dma_cblk_t, TXFR_LEN) == 0xc, "Bad offset");
_Static_assert(offsetof(dma_cblk_t, STRIDE) == 0x10, "Bad offset");
_Static_assert(offsetof(dma_cblk_t, NEXT_CB) == 0x14, "Bad offset");
// _Static_assert(offsetof(dma_cblk_t, DEBUG) == 0x18, "Bad offset");


// make a control block with simple defaults: you probably
// want to make other versions of this.
static inline dma_cblk_t 
cblk_mk(dma_addr_t dst, dma_addr_t src, uint32_t n) {
    dma_cblk_t cb = {0};

    memset(&cb,0,sizeof cb);

    cb.TI = 1<<8  // p51 src_inc = 4
        |   1<<4  // p52 dst_inc = 4
        ;

    // if evenly divides by 32 use 128-bit (32*8) xfers
    if(n % 32 == 0) {
        cb.TI |= 1<<9; // src transfer width 128 bits
        cb.TI |= 1<<5; // dst transfer width 128 bits
    }

    // translate the addresses here: too easy make 
    // mistakes otherwise
    cb.SRC_ADDR = src;
    cb.DST_ADDR = dst; 

    cb.TXFR_LEN = n;
    return cb;
}



// checks for different errors: currently panic's but in 
// a real system you'd want to handle and keep going.
static inline int 
dma_detect_errors(dma_ch_t *r) {
    // p55
    let d = r->DEBUG;
    if(bit_is_on(d,2))
        panic("read error!\n");
    if(bit_is_on(d,1))
        panic("FIFO error!\n");
    if(bit_is_on(d,0))
        panic("READ NOT SET error!\n");

    let cb = r->CS;
    if((cb>> 8)&1)
        panic("DMA error set\n");
    if((cb>> 6)&1)
        panic("waiting for last write to be receied\n");
    if((cb>> 5)&1)
        panic("paused by DREQ state\n");

    if((cb>>28)&1)
        panic("waiting for outstanding writes?\n");

    return 0;
}

static inline int dma_active(dma_ch_t *dma) {
    // if active=0, we are done.
    return (dma->CS&1) != 0;
}

static inline int dma_done(dma_ch_t *dma) {
    // we aren't sure what they were doing, so 
    // use dev barrier
    dev_barrier();

    if(!dma_active(dma)) {
        assert(dma->CB_ADDR.addr == 0);
        return 1;
    }

    // not done: so check for errors
    dma_detect_errors(dma);
    return 0;
}


// initialize DMA channel <ch>.  see p41.  
//
// NOTE: while there are 15 DMA channels we can't use 
// many of them --- DMA is useful, so it turns out 
// different periphs are using it [not mentioned in 
// datasheet]!
//
// Seems to be avail: 4, 5, 8, 9, 10.
// Good:
// - Individual IRQ lines
// - Not used by firmware
// - No special restrictions
// - Full DMA capabilities
// Can also use 11, 12, 13, 14 but they have more 
// limits.
static inline dma_ch_t *
dma_init(unsigned ch) {
    enum { DMA_BASE = 0x20007000 };

    uint32_t ch_addr;
    switch(ch) {
    case 4:  ch_addr =  0x400 + DMA_BASE; break;
    case 5:  ch_addr =  0x500 + DMA_BASE; break;
    case 8:  todo("add ch8");
    case 9:  todo("add ch9");
    case 10: todo("add ch10");
    case 15: todo("add ch15: note it's not contig!");
    default:
        panic("illegal ch=%d\n", ch);
    }

    // R-M-W DMA enable so we don't trash other channels.
    enum { DMA_ENABLE = DMA_BASE + 0xff0 };
    dev_barrier();
    OR32(DMA_ENABLE, 1<<ch);
    dev_barrier();

    // NOTE: <dma_ch_t> fields are <volatile>, so it's ok the pointer
    // is not.
    return (void*)ch_addr;
}


static inline void 
dma_initiate_raw(dma_ch_t *dma, dma_cblk_t *cb) {
    // CRUCIAL: we use memory barriers to make sure
    // make sure the writes to <cb> (from 
    // the caller) don't get pushed below the 
    // assignment.  
    //
    // [With that said: since the fields are currently
    // volatile, shouldn't matter.]
    gcc_mb();
    dev_barrier();

    enum {
        DMA_CS_START = 1,
        DMA_CS_RESET = 1<<31,
    };

    dma->CB_ADDR = phys_to_bus(cb);
    dma->CS      = DMA_CS_START;       // activate.

    dev_barrier();
    gcc_mb();
}

// Run <cb> on DMA engine <dma>. Does device barriers 
// and (should use) error checking.
static inline void 
dma_initiate(dma_ch_t *dma, dma_cblk_t *cb) {

    if(dma_active(dma))
        panic("DMA is still running!\n");

    if(!cblk_aligned(cb))
        panic("bad alignment for control block %x\n", cb);

    if(!phys_addr(cb))
        panic("cb should be a physical addr! <%x>\n", cb);
    dma_initiate_raw(dma,cb);
}

// Wait for the current DMA transfer running on <dma> 
// to complete.
//
// Returns 1 on success, 0 on timeout.
static inline int dma_wait(dma_ch_t *dma, int timeout) {
    while(timeout-- > 0)
        if(dma_done(dma))
            return 1;

    // you should get rid of this: we use for lab so can't miss 
    // timeout.
    panic("timed out!\n");

    // timeout.
    return 0;
}

// Run <cb> on DMA engine <dma> with <timeout>.
//
// Returns 1 if completed without errors, 0 otherwise.
//
// NOTE: always easy to make an async interface sync
static inline int 
dma_run(dma_ch_t *dma, dma_cblk_t *cb, int timeout) {
    dma_initiate(dma, cb);
    return dma_wait(dma, timeout);
}

#endif
