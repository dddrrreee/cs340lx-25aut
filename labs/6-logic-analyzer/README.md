## Lab: building your own logic analyzer

In lab 1 you made interrupts as fast as possible when no trick was out of
bounds, and there were no rules besides correctness.  You'll now apply
similar methodology --- using some of the same tricks and some new ones
--- to generalize this code into a useful tool.

For this lab, we give you a slow, trivial interrupt-based logic analyzer
--- a tool that records when the output from a pi  or sensor transitions
from 1-to-0 or 0-to-1 --- and you make it as fast as possible.

As in lab 1, while making a simple analyzer is simple --- less than 20
lines --- but then we will spend a fair amount of time making it as
accurate as possible.  Accuracy will require looking at the machine
code `gcc` generates from your C code, diagnosing what is messing up
your accuracy, understanding why `gcc` is doing what it is doing, and
devising ways to work around it.  (Or, alternatively, simply writing
code in assembly.)

If you do things right, you'll be able to reliably measure transitions
down to the 10ns of nanoseconds with very low jitter (< 5ns).  The result
of this process will be developing a not-novice understanding of the
underlying ARM hardware and of the gcc compiler.  This understanding will
serve you well elsewhere, even on non-ARM machines (in the same way that
understanding one programming language helps you understand others).

Why do we do this lab when we already did lab 1?
  - You'll have a deeper understanding of speed tricks.   Most people
    are bad at optimization, so it's good to be good since when you need
    speed, you really do.    Lab 1 had enough commentary that you could
    kind of copy and paste ideas; this lab has less structure so you'll
    have to think a bit more.   With that said, lab 1 should let you
    speed-run a lot of this lab by quickly doing the same stuff, just in
    a slightly different context.

    At a pedogogical level: You can view of this lab as a "spaced
    repetition" version of the first, where we waited a few weeks and
    revisited the topic.

  - As discussed more below: you can look at a logic analyzer as a way
    to do printk for electricity --- recording when a pin goes from
    0-to-1 or 1-to-0 and at what time.  As you've seen many times:
    its hard to figure out what is going wrong when you can't see it,
    especially with devices.

    You can use your analyzer to debug hardware devices by looking
    at their output.  Similarly, it also lets easily monitor when
    bit-banged versions of a digital protocol (such as software UART)
    have a mistake because you can see exactly when pins go high and low.
    (In fact: if you use it on our ws2812b lab you'll see a pretty big
    mistake in how we did things.)

    Because it is interrupt based, you can do this monitoring on the
    same system you are running on, which is always the easiest method
    (versus setting up a second machine, coordinating when to run etc).

  - As a funny duality: once you get a fast logic analyzer, you'll realize
    it is is essentially a universal bit-banged receiver
    for any digital protocol.  

    Since it generates generates a log of highly accurate pin transitions,
    you just have to walk over the log to reconstruct the signal.   And,
    given this, it also brings you close to a universal transmitter
    since very similar code let's you write (and monitor) fast, accurate
    bit-banged transmitters.

  - A final but massively huge reason: because everyone did lidar using
    a bit-banged UART receive, no one was able to record more than
    a handful of packets (less than 1%?).  Even a moderately sped up
    version of today's code will let you easily make a sw-uart receive
    that loses no packets and yet still leaves time to do other stuff
    (e.g., display their readings on the screen).

Building a fast accurate analyzer is one of those interesting cases
where, because we write code bare-metal and don't have to fight a large
operating system, we can easily do something that is well beyond the
abilities of a system such as Linux or MacOS: there is no way you can
make these systems reliably take these kind of timings.

Interestingly, it appears with a bit of code and a $15 pi, we can also
easily beat specialized hardware logic analyzers.  For example, when
we first started doing variants of this lab in 2020 the widely-used
$400 Saleae analyzer had an order of magnitude less resolution (at least
as far as I could tell as a novice user).  Also, their claim to fame
was measuriing multiple inputs (8-16) but required custom FPGA logic
to do so.  However, we can get the same effect, with no work, because
reading up to 32 pi pins can be done with a single load.


#### Starter code: `code/timed-read.c` and `code/interrupt-asm.S`


The starter code in the `code` directory.  It closely mirrors the code
from lab 1 other than two changes:

  1. It stores both the GPIO level and the cycle count in a log.
     For our code today we want to monitor multiple pins, so we need
     a read of GPIO_LEV0 so that we can later disambiguate which pins
     flipped.  (Of course you can make a faster version that monitors
     one pin.)

  2. It depends more on reading the cycle counter as early as possible
     in the interrupt handler so that it can have the lowest variance
     possible.  The lower the variance, the more resolution we have
     and the more protocols we can find bugs in.  For example, if your
     variance (error) is comparable to the timing requirements of the
     protocol you can't say anything about it.

As with lab 1, there two files:
  - `code/timed-read.c`: C code that sets up interrupts on rising and falling
    edges of a single GPIO pin, and then uses a simple loop to trigger
    them by alternatively writing 1 and 0 and measure the overhead.

  - `code/interrupts-asm.S`: assembly code to define the interrupt/exception
    table and forward interrupts to C code.

The code is simple but slow.  Your job is to speed it up as much
as possible.  The code is roughly the same as examples you've seen in
140e's device interrupt lab (lab 8) and 240lx's IR lab (lab 3).


#### Checkoff

Checkoff (tuesday):
  1. Make the analyzer fast.  
  2. Use any method you want.  However, make sure you do use the new 
     tricks we discuss (they will be annotated in the README below, 
     though it won't be today:).
  3. Turn the same code into a software UART receive (very little difference)
     and show you can use it to not drop any lidar packets.

Since we're all about fun: 
  - Just as in lab 1, if you can beat my code (checked in no later than
    this weekend) I'll give you $100.

There's a ton of interesting extensions:
  - turn your scope into a network transmit and receive and 
    see how much bandwidth you can get.
  - use more than one wire and see how much bandwidth you can get.
  - jit your test signal code.
  - pull in your virtual memory from 140e, turn on caches, and see how
    much faster.
  - use the pi PWM hardware to generate a test signal.  i have no idea
    how accurate it is, but am curious!
  - change monitoring pi so that you can upload code to it and have the
    code send results back to the monitored pi.
  - there are tons.

-----------------------------------------------------------------
### Background: Why are we making an analyzer?

After working with many sensors, you've no doubt had numerous cases
where:
  - you've misunderstood a sensor datasheet;
  - you've understood the datasheet just fine but it was incorrect;
  - or the senor was counterfeit (and didn't adhere exactly to 
    the sensor contract);
  - or was just broken (all of these happen more than you think!).   

The most common result of these errors is that the sensor quietly does
nothing, which isn't a particularly informative clue as to the distal,
root cause of the error.

Your logic analyzer will help you find a path forward in these cases,
by giving you visibility into what signals were sent (and when) from
the pi to the sensor and vice versa.  Without too much distortion,
you can think of your analyzer as a way to add `printk` to electricity.
Example errors it can diagnose:

   - On the pi side: where you send bits out backwards (an easy mistake),
     at the wrong times (same), or with the wrong values (even more same).
     Or when the compiler has removed or reordered stores to device
     memory (e.g., if you make mistakes with `volatile`).  Or the
     compiler has broken what should have been a single store into a
     read-modify-write, as can easily happen when you use structures
     to describe device memory and fields have sizes that cannot be
     atomically stored on the ARM.  It especially helps when we go to
     fancier device protocols such as I2C, where it will give you an
     easy way to compare your results to someone else.

   - On the sensor side: when the sensor is simply broken and is not
     doing anything at all.    When the sensor is behaving slightly
     differently than what the specification claims, but enough to mess
     up our code.  Such cases can easily arise because we often live
     in sin and buy cheap knock-off sensor copies rather then get them
     from the original manufacturer.  Or, worse, sellers take advantage
     of us and send counterfeits.

More  generally, your pi-analyzer can be used to check / debug
timing-sensitive parts of your OS code.  For example, when we 
build a simple real-time scheduler, you can check it by:
  1. Associate each thread with a GPIO pin and each pin with the thread's
     scheduling constraints.
  2. As the system runs, your analyzer can monitor that each pin goes
     high or low at the correct times.

This method is a harsh, difficult to fool method for immediately
detecting mistakes.

Once you get this working, you can put it on a second pi to monitor
the first one.  The fact that our pi-analyzer is an "air-gapped" system
separate from the pi we check makes it much easier to guarantee it detects
any timing mistake. Putting these checks in the checked-pi itself gives
much less confidence because the checks will be influenced by the system
they monitor.

  - Footnote: at the risk of typing farcical bullshit, I think you can
  look at the problems of checking timing constraints using the same
  system you are monitoring as a sort-of dual of Heisenberg's principle
  --- rather than our measurements perturbing the measured phenomena, the
  measured phenomena can perturb our measurements.  Or, as anticipated
  by Nietzsche, as our system looks into the abyss, the abyss looks into
  it as well.  It's easier to stick an air-gap between them, not worry
  about new age philosophical stuff, and go to the next problem.

-----------------------------------------------------------------
### Step 0: our starting point


As with lab 1, our first run is awful:
```
0: rising  [lev=1]	= 2775 total cycles [461 until int ran]
1: falling [lev=0]	= 2591 total cycles [450 until int ran]
2: rising  [lev=1]	= 2565 total cycles [453 until int ran]
3: falling [lev=0]	= 2585 total cycles [450 until int ran]
4: rising  [lev=1]	= 2588 total cycles [450 until int ran]
5: falling [lev=0]	= 2594 total cycles [453 until int ran]
6: rising  [lev=1]	= 2585 total cycles [450 until int ran]
7: falling [lev=0]	= 2586 total cycles [453 until int ran]
8: rising  [lev=1]	= 2588 total cycles [450 until int ran]
9: falling [lev=0]	= 2591 total cycles [453 until int ran]
10: rising  [lev=1]	= 2551 total cycles [453 until int ran]
11: falling [lev=0]	= 2585 total cycles [450 until int ran]
12: rising  [lev=1]	= 2585 total cycles [450 until int ran]
13: falling [lev=0]	= 2574 total cycles [452 until int ran]
14: rising  [lev=1]	= 2591 total cycles [450 until int ran]
15: falling [lev=0]	= 2586 total cycles [453 until int ran]
16: rising  [lev=1]	= 2579 total cycles [452 until int ran]
17: falling [lev=0]	= 2583 total cycles [453 until int ran]
18: rising  [lev=1]	= 2593 total cycles [452 until int ran]
19: falling [lev=0]	= 2590 total cycles [458 until int ran]
ave cost = 2593.483647
```

Bad things:
  1. Interrupt cost is huge: average = 2593 cycles.  By default
     the pi runs at 700MHz cycles per second, so we can do *at best*
     about 269,957 interrupts per second (700 * 1000 * 1000 / 2593).

     We can't even handle the lidar UART speed at this rate!

  2. The time to the first reading (which will be the value the 
     logic analyzer records)  bounces around significantly between
     461 and 450 cycles.  Jitter = error, and more error = less useful.



----------------------------------------------------------------------
### Step 1: inline and compiler optimize

Since we've already optimized device interrupts we speed-run the 
first part:
  1. Fix optimization in the Makefile: I used -Ofast, but maybe others 
     work better for you.
  2. Inline all GPIO routines.

We get a nice speedup of 1000 cycles!

```
0: rising	= 1691 total cycles [366 until int ran]
1: falling	= 1510 total cycles [356 until int ran]
2: rising	= 1512 total cycles [358 until int ran]
3: falling	= 1510 total cycles [359 until int ran]
4: rising	= 1499 total cycles [358 until int ran]
5: falling	= 1505 total cycles [362 until int ran]
6: rising	= 1501 total cycles [361 until int ran]
7: falling	= 1510 total cycles [356 until int ran]
8: rising	= 1501 total cycles [361 until int ran]
9: falling	= 1496 total cycles [356 until int ran]
10: rising	= 1498 total cycles [358 until int ran]
11: falling	= 1499 total cycles [359 until int ran]
12: rising	= 1518 total cycles [358 until int ran]
13: falling	= 1514 total cycles [359 until int ran]
14: rising	= 1501 total cycles [361 until int ran]
15: falling	= 1505 total cycles [356 until int ran]
16: rising	= 1501 total cycles [361 until int ran]
17: falling	= 1496 total cycles [359 until int ran]
18: rising	= 1521 total cycles [361 until int ran]
19: falling	= 1502 total cycles [360 until int ran]
ave cost = 1514.500000
```

----------------------------------------------------------------------
### Step 2: replace `dev_barrier` with `dmb` inlined.


If you look in `libpi/staff-src/mem-barrier.S` you'll see that our device
barrier uses the DSB instruction.  If you recall from the virtual memory
lab in 140e, this is the most complicated barrier, and so unsurprisingly
is the most expensive.  But even if we leave it as a `dsb`: as we've
already seen, function calls kill performance so inline it.


We can instead use DMB (as seen in `mem-barrier.S`):
```
.globl dmb
dmb:
    mov r0, #0
    mcr p15, 0, r0, c7, c10, 5
    bx lr
```

By changing it to a static inline routine:

```
static inline void dmb_raw(void) {
    uint32_t r0 = 0;
    asm volatile("mcr p15, 0, %0, c7, c10, 5" :: "r"(r0));
}
```

Which gives us another few hundred cycles without trying:

```
0: rising	= 1321 total cycles [348 until int ran]
1: falling	= 1133 total cycles [342 until int ran]
2: rising	= 1216 total cycles [343 until int ran]
3: falling	= 1130 total cycles [339 until int ran]
4: rising	= 1131 total cycles [344 until int ran]
5: falling	= 1129 total cycles [339 until int ran]
6: rising	= 1131 total cycles [343 until int ran]
7: falling	= 1132 total cycles [342 until int ran]
8: rising	= 1134 total cycles [343 until int ran]
9: falling	= 1130 total cycles [339 until int ran]
10: rising	= 1134 total cycles [343 until int ran]
11: falling	= 1129 total cycles [342 until int ran]
12: rising	= 1137 total cycles [343 until int ran]
13: falling	= 1129 total cycles [342 until int ran]
14: rising	= 1131 total cycles [343 until int ran]
15: falling	= 1133 total cycles [342 until int ran]
16: rising	= 1131 total cycles [340 until int ran]
17: falling	= 1133 total cycles [342 until int ran]
18: rising	= 1131 total cycles [343 until int ran]
19: falling	= 1129 total cycles [342 until int ran]
ave cost = 1145.200073
```


NOTE:
  - Maybe try with dsb to see if any difference?  I'm curious
    after the last lab but forgot to try.

  - Weird thing: if we eliminate the DMB between GPIO and event clear
    my code didn't get faster. I'm not sure what is going on.  For the
    moment we are going to cut so much additional stuff that I didn't
    worry about it (but it does make me uneasy).

----------------------------------------------------------------------
### Step 3: [NEW] replace interrupt sanity check with `assert`

If you look at `int_vector`'s machine code it's huge:

```
    8060:   e92d4070    push    {r4, r5, r6, lr}
    8064:   ee1f4f3c    mrc 15, 0, r4, cr15, cr12, {1}
    8068:   e3a01000    mov r1, #0
    806c:   ee071fba    mcr 15, 0, r1, cr7, cr10, {5}
    8070:   e59f2054    ldr r2, [pc, #84]   ; 80cc <int_vector+0x6c>
    8074:   e59f0054    ldr r0, [pc, #84]   ; 80d0 <int_vector+0x70>
    8078:   e59fe054    ldr lr, [pc, #84]   ; 80d4 <int_vector+0x74>
    807c:   e590c034    ldr ip, [r0, #52]   ; 0x34
    8080:   e5923000    ldr r3, [r2]
    8084:   e2835008    add r5, r3, #8
    8088:   e5825000    str r5, [r2]
    808c:   e5922000    ldr r2, [r2]
    8090:   e5834004    str r4, [r3, #4]
    8094:   e152000e    cmp r2, lr
    8098:   e583c000    str ip, [r3]
    809c:   8a000004    bhi 80b4 <int_vector+0x54>
    80a0:   ee071fba    mcr 15, 0, r1, cr7, cr10, {5}
    80a4:   e3a03302    mov r3, #134217728  ; 0x8000000
    80a8:   e5803040    str r3, [r0, #64]   ; 0x40
    80ac:   ee071fba    mcr 15, 0, r1, cr7, cr10, {5}
    80b0:   e8bd8070    pop {r4, r5, r6, pc}
    80b4:   e3a0303f    mov r3, #63 ; 0x3f
    80b8:   e59f2018    ldr r2, [pc, #24]   ; 80d8 <int_vector+0x78>
    80bc:   e59f1018    ldr r1, [pc, #24]   ; 80dc <int_vector+0x7c>
    80c0:   e59f0018    ldr r0, [pc, #24]   ; 80e0 <int_vector+0x80>
    80c4:   eb000123    bl  8558 <printk>
    80c8:   eb0002cd    bl  8c04 <clean_reboot>
    80cc:   000099e8    .word   0x000099e8
    80d0:   20200000    .word   0x20200000
    80d4:   00009df0    .word   0x00009df0
    80d8:   000095ec    .word   0x000095ec
    80dc:   000093e0    .word   0x000093e0
    80e0:   000093f0    .word   0x000093f0

```

And it's doing a bunch of stores and loads to the same address
that seems redundant:

    8088:   e5825000    str r5, [r2]
    808c:   e5922000    ldr r2, [r2]


what is going on?

Couple things:
  1. We have declared the current timed read pointer `tr` as a `volatile`
     pointer (i.e., the pointer itself can change, not just the contents).
     This means each access must reload it.  [Add link to Linus rant on 
     `volatile` versus memory barriers).

  2. One cause of the reload is that we have a sanity check in the
     interrupt handler:

            if(tr > t_end)
                panic("overflow\n");

     Sanity checks are great, and I encourage them!  I've been saved
     many times because of an "impossible" check that caught an error.

     But life is about tradeoffs, and error checks on critical paths
     cost you.  For this we use a standard satisficing trick that switches
     such checks to an `assert` macro, which is in turn can be enabled or
     disabled quickly by defining a preprocessor variable:



```
// libpi/libc/demand.h
#ifdef __NDEBUG__
#   define assert(bool) do { } while(0)
#else
    #   define assert(bool) do {                                   \
        if((bool) == 0) {                                       \
            debug("ERROR: Assertion `%s` failed.\n", #bool);      \
            clean_reboot();                                     \
        }                                                       \
    } while(0)
#endif

```
Here if you `#define __NDEBUG__` before the `#include "demand.h"
inclusion, `assert` will get replaced by an empty loop.


NOTE: 
 - We use an empty do-while loop because if you define it as
   an empty statement (e.g., `#define assert(bool) (void)0`), some
   compilers will give a warning.
 - If you have a side-effect in the assert (don't do this!)
   the side-effect will go away, breaking the code.


Doing this cuts down the size of the interrupt handler by almost
1/2, largely due to the weird constant loads the compiler emits
to get both global pointers (`tr` and `t_end`).
```
    8060:   e92d4010    push    {r4, lr}
    8064:   ee1fef3c    mrc 15, 0, lr, cr15, cr12, {1}
    8068:   e3a02000    mov r2, #0
    806c:   ee072fba    mcr 15, 0, r2, cr7, cr10, {5}
    8070:   e3a0c302    mov ip, #134217728  ; 0x8000000
    8074:   e59f1024    ldr r1, [pc, #36]   ; 80a0 <int_vector+0x40>
    8078:   e59f0024    ldr r0, [pc, #36]   ; 80a4 <int_vector+0x44>
    807c:   e5914034    ldr r4, [r1, #52]   ; 0x34
    8080:   e5903000    ldr r3, [r0]
    8084:   e583e004    str lr, [r3, #4]
    8088:   e283e008    add lr, r3, #8
    808c:   e5834000    str r4, [r3]
    8090:   e580e000    str lr, [r0]
    8094:   e581c040    str ip, [r1, #64]   ; 0x40
    8098:   ee072fba    mcr 15, 0, r2, cr7, cr10, {5}
    809c:   e8bd8010    pop {r4, pc}
    80a0:   20200000    .word   0x20200000
    80a4:   0000997c    .word   0x0000997c
```

This removes about 150 cycles.

```
0: rising   = 1241 total cycles [419 until int ran]
1: falling  = 1040 total cycles [342 until int ran]
2: rising   = 1038 total cycles [343 until int ran]
3: falling  = 1047 total cycles [342 until int ran]
4: rising   = 1044 total cycles [343 until int ran]
5: falling  = 1038 total cycles [342 until int ran]
6: rising   = 1045 total cycles [343 until int ran]
7: falling  = 1040 total cycles [342 until int ran]
8: rising   = 1035 total cycles [340 until int ran]
9: falling  = 1040 total cycles [342 until int ran]
10: rising  = 1038 total cycles [343 until int ran]
11: falling = 1035 total cycles [342 until int ran]
12: rising  = 1038 total cycles [343 until int ran]
13: falling = 1040 total cycles [342 until int ran]
14: rising  = 1035 total cycles [340 until int ran]
15: falling = 1040 total cycles [343 until int ran]
16: rising  = 1035 total cycles [340 until int ran]
17: falling = 1040 total cycles [342 until int ran]
18: rising  = 1041 total cycles [343 until int ran]
19: falling = 1046 total cycles [342 until int ran]
ave cost = 1049.800048
```



----------------------------------------------------------------------
### Step 4:  fix let performance bug


















NOTE: 
  -Iif you see this do a pull for more README updates (possibly by weekend :/)
