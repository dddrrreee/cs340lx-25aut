### Bare-metal Framebuffer on the pi.

----------------------------------------------------------------
#### Housekeeping

This week:
  - we'll do a couple open ended labs (hdmi + lidar) with minimal starter 
    code or prose and see how it goes.
  - One you get the basic parts working, you should do a mini-project
    that does something fun.  We'll grade these on a check, check-plus, 
    plus-plus.  If you have a few plus-plus extensions you'll definitely
    get an A+.  (If they are good enough, they sub for a final project.)
  - Report back next week!
  - There is now a [cs340lx-25aut-contrib repository](https://github.com/dddrrreee/cs340lx-25aut-contrib) --- if you do a cute trick or build a cool hack, make a directory with your
    name in the contrib repository and push so other people can use them.

----------------------------------------------------------------
#### Overview

Today we'll write simple bare metal driver for the HDMI display using
the [BCM Mailbox interface](https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface).
 
I brought a set of monitors to class that you are welcome to take:
  - If you're really into graphics, there are some 15 inch monitors.
  - If you're kinda into graphics, there are some 10 inch.
  - If you want to experiment with small displays there are some 7 inch.

Using these we can make some pretty pictures. A few fun things
to do:
  - Turn all your device labs into interesting visualizations.
  - Make your own embedded video picture frame and display favorite
    photos.  You can either use a bitmap or port one of the small
    JPG decoder libraries out there.  (Someone please do this so
    we can all use it!)
  - Write your own visualization library. Given how horribly complex
    GUI libraries are, this can give a fast, fluent way to do 
    interesting things.
  - Write your own window manager for your pi.

We used to do this way back in 107e, so it's not hard to get a simple
version up.  Making it fast can be technically interesting.  Figuring out
what to display is where a lot of the action is.  

Readings:
  - [BCM Mailbox interface](https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface)
  - [Review the 240lx Mailbox lab](https://github.com/dddrrreee/cs240lx-25spr/tree/main/labs/1-mailboxes)
  - The old 2016 107e slides on the display are a quick skim: [docs/Framebuffer.pdf](./docs/Framebuffer.pdf)

One problem with the display --- as far as I know, the only official
documentation is the mailbox interface.  While there are many code
implementations and tutorials out there, many of the authors 
are actually fairly naive:
  - *A quick heuristic*: scan any tutorial first to see if/how they
    discuss the virtual frame buffer.  If they don't say it's for
    double-buffering display, they don't have a good grasp of the
    hardware.

  - [This valver's writeup is a reasonable tutorial](https://github.com/BrianSidebotham/arm-tutorial-rpi/blob/master/part-5/readme.md).  There's some cute simple animations.

Checkoff:
  1. Make some kind of pretty picture on your screen.  Fractal, etc.
     If it's cute, check it into 340lx-25aut-contrib!
  2. Display the lidar output from the next companion lab.

----------------------------------------------------------------------
### Step 1: get the device size.

To start with, write the mailbox code to get your device size and 
sanity check that it makes some sense.  This will detect if your
mailbox code is wrong, the hardware is broken, or there was a hookup
issue.

```
Get physical (display) width/height:
 - Tag: 0x00040003
 - Request:
   - Length: 0
 - Response:
   - Length: 8
 - Value:
   - u32: width in pixels
   - u32: height in pixels
```

If the returned width and height are zero, something went wrong (or:
potential code bug).  

----------------------------------------------------------------------
### Step 2: initialize the device

Unfortunately, this is a all-at-once operation.  I had assumed that you
could initialize the device in pieces and check as you went, but after
wasting several hours, that *appears* to not be true.    (If I'm wrong
let me know for extension credit!)

In the mailbox lab, we sent one mailbox command at a time.  However, you
can batch multiple commands into a single message by:
  1. Concatenating them into the same buffer;
  2. Setting the mailbox message size to this size plus two words for
     the message header.
  3. You should definitely re-read the mailbox document to see the
     format!

For initialization, I made the following big message with the 
mailbox commands in the specified order:

  1. Set physical (display) width/height (Tag: 0x00048003).  Order:
     width, then height.  You must check the returned width and height.
     If they are different something could be wrong (e.g., if tiny),
     or your physical screen might not be that large.
  2. Set virtual (buffer) width/height (Tag: 0x00048004).  You must
     check this result as in step 1.  If it's different, something
     went wrong!   FWIW, when I started breaking the messsage into pieces,
     these numbers were always 2.
  3. Set depth (Tag: 0x00048005).  Depth is bits per pixel.  You must check
     the result since hardware can force what is possible: 0 means unsupported.
  4. Set pixel order (Tag: 0x00048006).  Confusing name to denote:
     0 = colors ordered BGR (blue green red), 1 = colors ordered RGB.
     You must check because the hardware can force this.
  5. Allocate (frame) buffer (Tag: 0x00040001).  Returns the framebuffer
     address and size in bytes.  You must check.  If the previous values
     have problems, you get back 0.  If you get an address it will be 
     in GPU memory, so you have to mask off the upper bits as we have
     done before: `addr & 0x3FFFFFFF`.
  6. Get pitch (Tag: 0x00040008).  Confusing name for "bytes per row."
     This is crucial to check since the hardware can add padding.
     When you're iterating from one row to another, you'll need to
     advance by this many bytes to get to the next.

After you define the above message, you send it using your `mbox_send`
routine from 240lx and pull the returned results from the message
memory after.

Note: 
  - It's very easy to mess up offsets!  You'll probably need a bunch
    of prints.

Hint: My message was 128 bytes declared as follows:
```
   volatile unsigned msg[32] = {
       128,               // [0] total size in bytes
       0,                 // [1] request = 0 (becomes 0x80000000 on success)

       // SET_PHYSICAL_WH (0x00048003): width,height (8 bytes)
       0x00048003, 8, 0,  // [2..4]
       width, height,     // [5..6]: width and height for display

       // ...  the other tags from steps 2-6 ...

       0,                 // [29] end tag
    };
    mbox_send(MBOX_CH, msg);
    output("status=%x\n", msg[1]);

    uint32_t fb_raw = msg[23];
    volatile uint32_t *fb = (void*)(fb_raw & 0x3FFFFFFF);
    output("fb raw address=%x, pi address = %x\n", fb_raw, fb);
    ...
```

----------------------------------------------------------------------
### Step 3: draw a square in the middle of the screen

One you have the frame pointer, draw a square.  Note: make sure
you use the pitch value (bytes per row) to compute where the different
rows are.

----------------------------------------------------------------------
### Step 4: draw something pretty

Fractals are always fun!  Or some simple animations.  You can make
animations fast by double buffering.

----------------------------------------------------------------------
### Micro-project options

Some simple ideas:
  - There's many other commands for the framebuffer.   Use them to do 
    interesting hacks (e.g., fast animations).
  - See how fast you can make a fractal go in the style of lab 1.  
    Make a record of each optimization and the speedup.  Am curious
    how far this can be pushed.  

Moderate fun:
  - Display your IMU orientation in real time.
  - Turn any of the other device labs into an interesting visualizations.
  - Make your own embedded video picture frame and display favorite
    photos.  You can either use a bitmap or port one of the small
    JPG decoder libraries out there.  (Someone please do this so
    we can all use it!)
  - Write your own visualization library. Given how horribly complex
    GUI libraries are, this can give a fast, fluent way to do 
    interesting things.
  - Make a logic analyzer based on lab 1. (Decent chance we'll  do 
    something like this in a week or so: might as well get credit early!)

Some bigger ideas:
  - Take your 240lx GPU code and use it to write directly to the 
    frame buffer (that's what it's for!)
  - Even more ambitious: use the shader, etc in the GPU to display
    something interesting.  This is definitely possible, but I have
    no idea how to do so, so am very interested :)
  - Use the VPU (not the GPU) to compute and display.  There's a 
    secondary but weird processor on the bcm side that we've not
    been using --- perhaps something can be done with it.
  - Write your own window manager for your pi.  

This is a tiny set of possibles --- lmk if you come up w/ other cute ones.
