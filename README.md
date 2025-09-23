### CS340lx: (even more) advanced systems labs

Context:
 - Final class in the 140e/240lx pipeline (as of rn :).
 - We only offer it every few years when there is an unusual 
   group.  Lots of great alumni!
 - This is the 3rd offering.

Structure:
 - 140e and 240lx have to fulfill class requirements, so content somewhat
   constrained.  

 - 340lx we build whatever seems interesting --- only rule is we build and
   you have a working example of a cool trick or deep method every lab.

   In general: we go broader on fun stuff (devices), go deeper on
   interesting stuff than the simple examples in previous labs.  

 - One fantastic difference: Around half of the labs are usually
   student-written.  You have already suffered through over 30 labs
   as consumers, so now is your chance to buld the labs you would have 
   wanted.  

   This is my favorite part of the class.  I always learn alot.  The new
   labs give tricks useful for 140e/240lx.  And, as we saw, (eg Stuart's
   elf lab) these labs can >>> better than our staff ones :)

What's the big picture goal: 

 - Do a bunch of cool devices for fun.  

   Since we have a limited enrollment, can afford more expensive ones
   (lidar, screens, kilometer capable LoRa RF)

 - Do a bunch of new sbc's --- arm, riscv, whatever.  

   Often bootup is the most difficult part, so it's good to have
   a portfolio.  Also once you see more, you start to get a feel for
   what is an arbitrary choice and which have a

 - Now that you have a solid grasp of low-level hardware and code, we 
   build more advanced stuff, and go much deeper.  

   Some examples from the past:
     - A couple of labs to build a riscv simulator that can simulate 
       itself and is register equivalent with the hardware.
     - Doing Turing complete DMA (based on Max Cura's hack)
     - Speed up interrupts/exceptions by 50x.

     - Build a SAT solver (Matthew)
     - Build a constraint solver (Matthew)
     - Static analysis bug finders (Matthew and Manya),
     - Verilog FPGA examples (Zack)


Some possible good labs from 240lx final projects

 - Stuart's elf debug: use to make a real profiler.
 - Quake (sai and james): will do screen and keyboard labs
 - ox64 (Aditya et al): cheap, real riscv
 - Pi zero 2 (Benji): 4 armv7 processor, faster, still bcm2835.
 - handwriting rec (Chia-Hsiang, Irene, Kaylee, Weixin)
 - fuse?

Devices:
  - i2s microphone, 
  - class D amplifier + speaker
  - HDMI screen
  - Lidar
  - sbcs: pico 2, pico, ox64, pi zero 2

Tentative things I'd like to do (won't do all)
  - a solid network boot loader that works over RF, sound, light, IR.
  - a few projects putting together several devices.
    (e.g., accel controlled lights, acoustically reactive displays)
  - in honor of Joe: chip8 video game emulator.
  - make a bunch of stuff really, really fast.  e.g., beat your macos
    on things like exceptions using pi.  make tiny processes so you can
    quickly fork 100,000.
  - build the runtime tools we were discussing: eraser race detector, 
    volatile checker.
  - do much better versions of 140e labs so can pull them in: ideally
    a simple complete OS.  definitely a better fast FAT32

The basic play: we've spend a couple quarters learning alot of low-level
stuff the hard way.  Now reap the rewards using it to build the cool
stuff.
