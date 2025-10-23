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
