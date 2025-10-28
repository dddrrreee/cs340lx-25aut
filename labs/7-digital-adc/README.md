## How to turn a digital pin into an ADC with no external components.

Today's lab is another episode of the current sitcom I'm now paying real
money to particpate in roughly titled: "for years I've looked down on
the LLM fad and now I have to eat the L because they figured out some
crazy stuff."

Context: I spent some more time trying to replicate the Sonnet 4.5
claim from Lab 5 that if:
  1. You switch GPIO configurations (say from input to output);
  2. Do not perform a DSB.
  3. Immediately use the GPIO pin.

That step (1) may not be complete when you start doing step (3)
and you can get unexpected behavior, such as lost output writes.

In the end I still have not been able to replicate this claim.  However,
even this error turned out to be valuable in that it led me to an
interesting result.

During the many failed attempts to replicate it I stumbled across an
odd GPIO pin result (more below), and with a few tweaks was able to
turn it into a trick that can turn any "digital" pin on the pi
(a pin that only reads 0 or 1, not varying voltage levels)
into an analog-to-digital-converter (something that can convert
a varying voltage level into a typically many-bit integer reading).
I believe this trick will apply to most other SBCs out there.
Hopefully we get some experimental tests of this today!

Digital pins have a threshold: below the threshold you get zero,
above the threshold you get 1, and above that threshold you get smoke.

If you've done any work in the area, the goto response for anyone asking
to read an analog input with a pi is that you need an external ADC device
(usually followed by various complaints that you can do this with an
arduino or pico board).  If you do a cursory google search you will see
decades of this movie played out even more than a Marvel franchise.

In fact using a digital pin to read voltage levels is viewed as so
impossible that not only do people think you can't do it, they don't even
think reject it as an option and, depending on the venue's moderation
policy, will simply call you stupid if you proposed it. In 140e we
certainly would have (and have) told you that this doesn't work.

(You can quickly see this by searching anything on the lines of "how do
I read analog input on the pi".)   

Today's lab: 
  - Use the trick to read the levels of a potentiometer.
  - Make this reading more accurate.
  - Use the trick to sense light levels with a LED.
  - Ideally: use it to read the analog joystick we gave out, which
    otherwise will be useless without an ADC.
  - Some other things that are up to you.


Max(imum) Mode:
  - If you don't want to follow along in the README with the examples,
    you can go full Max mode and just 
    read the next background section for two odd GPIO features
    and then try to derive on your own.  

    This will definitely teach the most and I think you have all the
    background you need.

    With that said going through the lab and building the stuff described
    will let you get to new stuff that hasn't been done pretty quickly.

While we do some concrete hacks, the overall learning goal is to
understand hardware at a much deeper level by throwing alot of effort
and creativity at the question:
  - What kind of electrical hacks can we build with zero external components
    by figuring out how the pi hardware actually works, and exploiting
    tricks based on this.  
  - We've always taught a simple story of GPIO having input, output some
    ALT functions, and generally treating it like a software system.
    This abstraction has cost us power: it hides information, and 
    knowledge is a weapon.  We try to get rid of some of this.

FWIW, the results of a no-external-component approach so far:
  - I learned a bunch of new things about electrical hacks you can do
    with GPIO pins. Despite (1) GPIO being the most vanilla hardware
    there is (so basic we cover it in the first 140E lab with non-hardware
    people) and (2) despite thinking and teaching it for years.

  - This angle has produced some cute results that when I tell
    them to fairly famous EE people (and a harder test: to some of our
    best former TAs) they don't know how to do them.  The EE profs I
    mentioned them to couldn't figure out to do them until I told them :).

Obligatory disclaimer:
  - I loathe vibe coding and the general overclaim, oversell, vapid nature
    of a lot of LLM work, but with that said, with the right approach I've
    been able to use it in a way that slaps me in the face with results so
    surprising I think they are hallucinations.  Until I run the code.
  - With that said I still feel dirty using it, but I like to get 
    shocked with new tricks, so here we are.

### Background Some odd results.

#### Surprise 1: You can read back GPIO you wrote!

In a funny chaos-butterfly effect way, Javier (140e, 240lx, 340lx 2025)
kicked off the chain that led to this lab.  After politely listening to
me yap about using LLMs to construct datasheets for undocumented hardware
by extracting the needed facts kernel drivers and forum lore, he told
me to try getting bluetooth on the pi working since he was running into
some problems (ironically b/c of a bad pi!).

The right prompts were able to get me from nothing to proof-of-life pretty
quickly (in a day?).  Of course, there's always missteps, so part of the
process was getting the LLM to spit out code that would check its result.
The first odd semi-trivial but useful surprise came from this.

In the initial stage I wasn't getting any response from the BT so started
trying to get it to spit out actionable ways to test its step by probing
the hardware.  As one of the first steps you have to write a 1 to pin
45 to wakeup the bluetooth module.

IIRC the LLM stated I should read pin 45 with an logic analyzer to see
that it was on.  I pointed out that you can't do this because 45 is one
of the many pins not exposed on the GPIO pi header.  So it simply stated
I should simply read it with a `gpio_read`:

    // turn on BT by setting pin 
    gpio_output(45);
    gpio_write(1);

    // sanity check that check that 45=1
    gpio_input(45);
    if(!gpio_read(45)))
        panic("45 not set!\n");


Initially I figured it was hallucinating --- clearly there is no
loopback!  And we wrote it as output, how would we read it as input?
Well, it turns out that this works because (as far as I can tell),
read just looks at the voltage on underlying pin rail.    Ha!  Cool,
you can read back values without an external loopback.

The following code always passes assertions:
```c
    // note: 
    // - you should try different pins! (not 14,15 tho:)
    // - maybe worth randomizing.
    // - maybe worth doing other pins simultaneously
    enum { pin = 16};
    for(i = 0; i < 1024; i++) {
        // 1. set <pin>=v
        unsigned v = i%2;
        gpio_set_output(pin);
        gpio_write(pin, v);

        // 2. check <pin>==v
        gpio_set_input(pin);
        unsigned got = gpio_read(pin);
        if(got != v)
            panic("wrote %d, got %d\n", v, got);
    }
    output("success: %d trials\n", i);
```

Interesting, very basic example of how software abstraction hurts our
view of reality since oversimplying.   Even something this basic can
be used to our advantage --- for example, if you read about configuring
pullups and pulldowns on the pi, everything says (1) it's hard and (2)
you need a logic analyzer to check.  While (1) is certainly true, but
(2) for pullups is a matter of reading back the value (pulldowns can be
done using a variant of the trick below).

This started making me think about other stuff that wasn't what it seemed,
and start experimenting.  Which led to the more useful next result.

#### Surprise 2: You can read back GPIO you wrote!

After realizing that GPIO hardware didn't act like normal software
with types, I started playing around with different violations and measuring
how long transitions took. Which led to the following:

```c

    // set as input and disable pulldown/up [not necessary
    // default works too]
    gpio_set_input(pin);
    gpio_set_no_pull(pin);
    assert(!gpio_read(pin));

    // step 1: set as output and write.
    gpio_set_output(pin);
    gpio_set_on(pin);
    dev_barrier();      // make sure it sticks.

    // Step 2: speed critical (sort of): 
    //  - change to input and immediately start reading,
    //    count until it goes to 0.
    asm_align(5);
    gpio_set_function_raw(pin,GPIO_FUNC_INPUT);
    for(i = 0; gpio_read_raw(pin); i++)
        ;
    if(i != 0)
        output("iterations before <pin> off: i=%d\n",i);
```

Before last week, I would have said `i` should always be zero.
When you run this, you get a very not-zero value (for me: `i=2517506`).

After sitting with this weirdness a bit, I remembered a Mark Horowitz
description from a couple years back of how to build an ADC by measuring
discharge rate --- it turns out that the time it takes to discharge
(measured by the loop counter) is proportional to the input voltage
applied to the pin, so you can modify the above code to be an ADC.


***At this point you have all the information you need to do the rest
of the lab in Max(imum) mode if you want.  I'll add some blank lines
after this so you can ignore the rest if you want and figure stuff out
on your own.***





























### Part 1: a zero-component ADC to measure a potentiometer

<img src="./images/pot-pinout.jpg" width="450" />

Potentiometers  take in voltage and attenuate it proportionally to
how much you turn their nob.  If you connect a pot to a digital pin on
the pi and read it directly you will (at best) get 0 or 1 depending on
the position.  (In a typical case: You may well get only 0 or only 1
or smoke.)  

We will 

to measure it reasonably
well, which you can then use as a control.

For this part we will show how to measure it reasonably well  by using the
trick discussed above to build a zero-external-component ADC using only:
 - GPIO parasitic capacitance as your capacitor bank
 - GPIO input threshold as your comparator
 - Software timing loop as your counter
 - A potentiometer (which you needed to measure anyway)

We won't use:
 - external resistors, 
 - external capacitors, 
 - and definitely no external ADC chip.

Just exploit the analog properties hiding inside "digital" GPIO pins.

Our first pass, we hook it up as follows:
  - vcc to 3.3v.
  - out (middle pin) to gpio.
  - gnd is not connected (floating).

My code looked the same as the example code above except 
  1. My code set the output pin to off, waited 100 usec
     so it could drain (try other values!) and then counted how long it
     took to read non-zero.
  2. I put the measurement code in a loop with a 1 second
     delay so it didn't scroll off.

The result: 
  - pot turned all the way to the left = 3 iterations.
  - pot turned all the way to the right = 0 iterations.
  - in the middle read 1 or 2.

A successful ADC!  Though extremely crude at only 2-bits.

The fun really starts when you try to make it higher resolution,
which makes puzzles come at you from all angles :)

At a basic level: Why is are the bits so low?  Well, the voltage
builds up too quickly, triggering the GPIO to 1.

How do we make it take longer?
  1. Slow the buildup.  How do we do this without external resistors?
     (Hint: pulldowns.)
  2. We could reduce the input voltage. (You know how to do this.)
  3. I didn't try, but you could also flip things around and see
     how long it takes to make things go to zero (I didn't try this
     b/c ran out of time but I believe works better.)

You should mess around with it and see how many bits.


### Part 2: a zero-component photoresistor to measure light.

***DO A PULL***


Measuring a pot is a great hello world, but lets build something
more: a light intensity measuring photosensor.


For this we will use a single LED and an old cute trick: when you connect
an LED to power in the "regular" way (long leg to power, short to ground)
it emits light.  When you reverse it, it senses light: 
  1. The higher the light intensity, the more resistance, so the 
     longer it will take to drain.
  2. The lower the intensity, the less resistence so the shorter it 
     takes to drain.

The circuit is as follows:

```
Circuit: LED Photoresistor with GPIO Power Control

     GPIO_POWER (output)
       |
       |  (cathode - short side)
      ---
     / | \  LED (reversed!)
     \ | /
      ---
       |  (anode - long side)
       |
       +------- GPIO_SENSE (input, no pull)
       |
      [R]  (1-10MΩ optional discharge resistor)
       |
      GND
```

Measurement procedure:
 0. Set the GPIO_SENSE pin to input and no-pull.
 1. Charge Phase (LED powered)
   - Set LED_POWER as OUTPUT, drive HIGH
   - LED cathode at 3.3V, anode connected to SENSE pin
   - Wait 100ms for capacitor to charge through LED
   - Ambient light hitting LED generates photocurrent during this phase
 2. Power Off
   - Set LED_POWER LOW
   - This instantly kills the LED's photocurrent generation
 3. Measure Discharge
   - Set GPIO_SENSE as INPUT with no pull resistors
   - Start timer
   - Poll GPIO_SENSE until it reads LOW (crosses threshold ~1.8V)
   - Stop timer - discharge time is your measurement
 4. Result
   - Longer discharge time = brighter light was present during charge phase
   - Shorter discharge time = darker conditions

Key Points:
   - LED is "backwards" - cathode to power, anode to sense pin
   - No delay needed between power-off and measurement
   - Optional external resistor (1-10MΩ) speeds up discharge if needed
   - Can add capacitor bank (multiple GPIO pins in parallel) to slow it down



Cross check using a very very different approach.  Can slap you with
assumptions you've built up over the years that your habit never lead
you to challenging.  (Perhaps one reason that some of the biggest
discoveries in the physical discoveries in the physical sciences have
come from accidents that had odd results.)

The point isn't that this is 
This volated our
