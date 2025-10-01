# Prelab for PCB Lab

This lab will be focused on designing printed circuit boards (PCBs), which many students in 140e/240lx/340lx have found useful over the years in final projects to help reduce wiring complexity and increase hardware reliability. The focus of the lab will be on how to replace a bundle of wires with an inexpensive PCB, and not on how to actually design electrical circuits (which is a much more complicated discussion). 

## Install KiCAD

KiCAD is the software we'll be using in order to design the PCBs for the lab. It's very popular in the hobby electronics community. Please follow the instructions at [https://www.kicad.org/download/](https://www.kicad.org/download/) for your OS. It is open source so it should be possible but potentially annoying to build if your OS isn't listed.

### General Keyboard Shortcuts
I assume `Ctrl` becomes `Cmd` on mac.

* `Ctrl+S`: Save (do often!)
* `Ctrl+Z`: Undo
* `Ctrl+Y`: Redo


## Select Hardware

Search the internet for a few interesting sensors or peripherals that you want to try and integrate on to your board for the lab. It's probably early to think about final projects, but it would be cool to think about what you might want to do and start working on a way to save yourself wiring headaches (and get some revisions in before the end of the quarter).

If you can find a "breakout" board, that will make things quite a bit easier as those tend to have the more complicated support circuitry for the component onboard. However, I can also help you read a datasheet if there is something unique you want to try. 

Some vendors I like for their easy-to-use breakout boards and other hardware are [Adafruit](https://www.adafruit.com/) and [Sparkfun](https://www.sparkfun.com/). Their documentation and guides are fantastic, and you can take a look at their Arduino libraries for inspiration for writing your own device drivers.

Vendors like [DigiKey](https://www.digikey.com/) or [Arrow](https://www.arrow.com/) have much more variety but the search can be a bit daunting. They are more suited to raw components.

If you are feeling adventurous, there's a lot of cheap stuff on Amazon, though the documentation is going to be lacking. A lot of it is probably straight off AliExpress which is another option if you're feeling both adventurous and patient. 

Some ideas for interesting hardware:
- [I2S Microphone](https://www.adafruit.com/product/3421) and/or [I2S DAC](https://www.adafruit.com/product/3678) (real-time audio effects?)
- [NRF24L01](https://www.amazon.com/HiLetgo-NRF24L01-Wireless-Transceiver-Module/dp/B00LX47OCY) wireless transceivers (you can support many on the same board as long as each gets its own CS pin, which would be very awkward with just wires).
- [RFID Reader](https://www.sparkfun.com/sparkfun-rfid-qwiic-reader.html)
- [OLED breakout](https://www.adafruit.com/product/1431) or [tiny OLED screens](https://www.amazon.com/Hosyond-Display-Self-Luminous-Compatible-Raspberry/dp/B09T6SJBV5)
- [LoRa radio](https://www.adafruit.com/product/3072)
- [2D LIDAR](https://www.amazon.com/WayPonDEV-RPLIDAR-C1-Avoidance-Navigation/dp/B0CMTXV5RC) (you can design a PCB to connect directly to the LIDAR and bypass the complexity of USB)

Of course, the world is your oyster so choose something that you'll find interesting!
