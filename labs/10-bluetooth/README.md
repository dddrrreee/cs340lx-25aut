# Talk to your friends over Bluetooth

**Lab by: [Javier Nieto](mailto:jgnieto@cs.stanford.edu)**

<p align="center">
<img src="./images/rpi%20zero%20w%20bt.jpg" alt="RPi Zero W image with Bluetooth logo" width="800" />
</p>

## Useful links

* [BCM2835 peripherals](./docs/BCM2835-ARM-Peripherals.annot.pdf).
* [Infineon CYW43438 datasheet](./docs/Infineon%20CYW43438.pdf)  (formerly known as BCM43438).
* [Bluetooth 5.1 spec](./docs/Core_v5.1.pdf) - the full doc is very long so here are the relevant extracts:
  * [UART transport layer](./docs/BT%205.1%20H4%20UART.pdf) (aka H4 protocol).
  * [Host-Controller Interface](./docs/BT%205.1%20HCI.pdf).
* Linux code (if you're curious how we figured out how this device works):
  * [hci_bcm.c](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/drivers/bluetooth/hci_bcm.c) - Identifies and sets up device, implements H4 protocol, includes quirks.
  * [btbcm.c](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/drivers/bluetooth/btbcm.c) - Implements firmware patch upload process.
  * [bcm283x-rpi-wifi-bt.dtsi](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/arch/arm/boot/dts/broadcom/bcm283x-rpi-wifi-bt.dtsi) - Tells us our Bluetooth chip is called `brcm,bcm43438-bt` in the code above and that its on uart0.
  * [bcm2835-rpi-zero-w.dts](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/arch/arm/boot/dts/broadcom/bcm2835-rpi-zero-w.dts) - Device tree file for our Raspberry Pi Zero W. Tells us which pins uart0 is connected to. Interesting if you're curious in how the hardware is represented in a "real" OS (this file #includes many others at the top, which are also quite interesting).

## Introduction

Our humble Raspberry Pi Zero W has an Infineon CYW43438, a dual Bluetooth+WiFi chip on it, separate from the main Broadcom processor.
This external chip used to be manufactured by Broadcom too until they sold the rights to Cypress which in turn was acquired by Infineon (you will see references to all three companies, fun!).
In practice, the Bluetooth and WiFi aspects are completely independent for our purposes.

The Bluetooth spec is quite complicated (2,900+ pages), including specifications on which RF frequencies to use, how to encode and decode the data, etc.
Luckily, we need only be concerned with the "Host-Controller Interface" section of the spec, which describes a protocol for a host (our CPU) to communicate with a controller (the CYW43438) with a series of higher level commands such as "listen for incoming connections" and "send this data to that device."
These commands are sent over a simple UART interface which is wired to GPIO pins 30-33.

In this lab, you will use these commands to connect to a friend's Pi and send them a message.

## 1. PL011 UART setup

<p align="center">
<img src="./images/UART0 pins.png" alt="UART0 GPIOs" width="800" />
</p>

So far, we have been using the "mini UART" from the AUX device (`uart1`) for printk, whose register layout is based on the 16650 design. Since we also need a UART for the Bluetooth chip, we will use the `uart0` device which follows the ARM PL011 design.
As you can see, pins 30-33 actually have both UARTs, `uart0` on GPIO function ALT3 and `uart1` on ALT5.
Before we send anything, we also need to turn on GPIO45 for at least 800ms to power up the Bluetooth module.

**Warning: our original GPIO code ignores all calls with pin numbers higher than 31.
We have provided a few fixed functions in `gpio-high.h`**

Some important facts:

* Baud rate 115200 (from the [Infineon CYW43438 datasheet](./docs/Infineon%20CYW43438.pdf)).
* Use 8 data bits, no parity, 1 stop bit, AKA `8n1` (from the [Bluetooth UART transport layer](./docs/BT%205.1%20H4%20UART.pdf)).
* Make sure to enable RTS, or the device will think that the host is unable to receive data.

Open `pl011.c` and fill in the TODO's. You will notice that the implementation uses interrupts for receiving (so as not to miss any bytes) but writes synchronously. You will need to reference the [BCM2835 peripherals](./docs/BCM2835-ARM-Peripherals.annot.pdf).
Once test `1-test-pl011.c` passes, you may proceed to the next part.
The test sends a HCI_Reset command to the controller and expects a reply (more on how that works will follow shortly).

## Sending commands and receiving events

Now that we can send bytes back and forth, let's talk to it.
The main protocol that concerns us is the Host-Controller Interface, or HCI (keep the [spec extract](./docs/BT%205.1%20HCI.pdf) open as you work on the lab).
The basic structure is that the host (that is, us) sends the controller (the BT module) commands.
Then, the controller replies with events.
Most events happen in response to a command (for example, HCI_Command_Complete), but some can happen spontaneously (like HCI_Disconnection_Complete).
Once you establish a connection, data can be sent and received using so-called ACL data packets (we will ignore Synchronous/SCO data packets today).

Let's start by sending an HCI_Reset command, which should always be the first command that we send.
Right now, we have a full-duplex connection with the BT module through which we exchange streams of bytes, but there is no way of knowing when a new packet starts or what type it is.
The [UART transport layer section of the BT spec](./docs/BT%205.1%20H4%20UART.pdf) tells us how to identify each packet type:

<p align="center">
<img src="./images/H4 packet types.png" alt="Table from BLUETOOTH CORE SPECIFICATION Version 5.1 | Vol 4, Part A page 2528" width="600" />
</p>

To know how each packet type is laid out, let's take a look at the [HCI section of the Bluetooth spec](./docs/BT%205.1%20HCI.pdf).
Here is the format for a command:

<p align="center">
<img src="./images/HCI command format.png" alt="Command format - HCI pdf page 770" width="800" />
</p>

Notice that the first two octets (the spec uses the word "octet" to refer to bytes) form the OpCode.
In the spec, you can read
"The Opcode parameter is divided into two fields, called the OpCode Group Field (OGF) and OpCode Command Field (OCF).
The OGF occupies the upper 6 bits of the Opcode, while the OCF occupies the remaining 10 bits."
The OGF identifies roughly the group the command belongs to and the OCF identifies the command in particular.
You shouldn't have to worry too much about this today since we provide `hci-consts.h`, which contains the OpCode for most important commands.
Next, there is one octet that specifies the total length of the parameters, in octets (some parameters can be multiple octets).
Finally, the parameters themselves.
Notice that this means that when we receive a packet, we need to wait until we have three octets to know how long it will be.
**Important: everything is [little endian](https://en.wikipedia.org/wiki/Endianness), so if something is multiple bytes, send the least significant bytes first!!**

And this is the format for an event:

<p align="center">
<img src="./images/HCI event format.png" alt="Event format - HCI pdf page 776" width="800" />
</p>

Notice that it is quite similar, with the only difference that the event code (roughly equivalent to the OpCode) is one instead of two octets.

To prevent the controller from being overwhelmed, the spec provides for a command flow control system:

<p align="center">
<img src="./images/HCI command flow control.png" alt="Command flow control - HCI pdf page 79" width="600" />
</p>

We provide you the code that implements this, but you should know about it since it will come up later.
Also, although in theory commands can finish out of order, we assume for this lab that this won't happen (I have never observed it).

Complete the TODO's in `bt.c` for the following functions: `bt_init`, `_receive_event`, and `bt_send_command`.
You will notice that the function `_receive_packet` is called when the user of this module requests a packet synchronously (or asynchronously but there is pending data).
It determines the packet type based on the first byte and calls the appropriate function, which also blocks until it has received a complete packet.
You should be able to pass `2-hci-reset.c`, which executes an HCI_Reset command and waits for the response.

## Uploading the firmware patch

<p align="center">
<img src="./images/dory.jpg" alt="Pls don't hate me for this meme." width="514" />
</p>

At this point, we can communicate with the controller by sending it commands and receiving events.
However, the firmware contained in the CYW43438's ROM is buggy.
The solution is to send the device a Broadcom-provided patch every time the device powers up.
The process is as follows (as reverse-engineered from Linux):

1. Send an HCI_Reset command.
1. Send command with OGF 0x3f (denotes a vendor-specific command) and OCF 0x2e, to signal the start of the firmware patch. We provide this constant in `hci-consts.h` with the name `CMD_BCM_LOAD_FIRMWARE`.
1. Wait 50ms.
1. Send all the bytes in BCM43430A1.hcd (see below for how to to this while respecting flow control).
1. Wait 250ms.

There are many ways to get access to the data from the C code.
The "correct" thing to do would be to put it on the SD card or include it in the my-install program with an appropriate place in the link file.
My favorite way is to use the xxd tool to make a C code file with the data using the following command `xxd -i BCM43430A1.hcd > BCM43430A1.c` and uncomment the corresponding line in the Makefile.
The output should be something like this:

```c
unsigned char BCM43430A1_hcd[] = {
  0x4c, 0xfc, 0x46, 0x10, 0x18, 0x21, 0x00, 0x42, 0x52, 0x43, 0x4d, 0x63,
  0x66, 0x67, 0x53, 0x00, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x01,
  /* ~2500 more lines of data. */
  0x05, 0x39, 0x0a, 0x21, 0x00, 0x00, 0x4e, 0xfc, 0x04, 0xff, 0xff, 0xff,
  0xff
};
unsigned int BCM43430A1_hcd_len = 30049;
```

For step 4, we could theoretically just send all of the bytes to the device for step 4 with a simple for loop and calls to `pl011_put8()`.
However, if we look very carefully, we can realize that these bytes are just a sequence of commands sent one after the other, each containing a chunk of the patch.
Therefore, we really should be respecting the flow control we implemented in the previous part.
To do so, instead of just sending the bytes, we are going to interpret them and send them using the `bt_send_command()`, which we have already programmed to respect flow control.
There should be exactly the right amount of bytes such that there are none left over.

If you print the commands as you send them, you will notice that all but the last one have OGF 0x3f and OCF 0x4c, which presumably upload a chunk of data (most are 255 bytes but some are fewer). The last command is different, with OCF 0x4e, presumably telling the module to start execution in a new place in RAM.

Implement the `void bt_upload_firmware()` function. Running test `3-firmware-upload.c` should be useful as you work on this part. To verify everything is working, run `4-read-bdaddr.c` and write down your BD_ADDR (equivalent to a MAC address) for the next part.

## Hello friend

Now you will establish a connection with your friend.
Step 1, find someone you get along with (alternatively, make a new friend).
Step 2, decide who will actively establish the connection (whom I will call Alice) and who will accept it (naturally, Bob).

Although Bluetooth provides a mechanism to discover devices by "advertising," we are going to take the lazy route of having Alice directly connect to Bob using his BD_ADDR.
To do so, Bob has to listen for connections by enabling "page scanning."
Alice then issues the command HCI_Create_Connection with Bob's BD_ADDR.
Bob receives an event Connection Request containing Alice's BD_ADDR.
Bob accepts the connection and both receive a Connection Complete event.
This event also specifies the connection handle, which is a multiplexing key which both must use when sending messages back and forth.
The handle allows one device to keep multiple connections.

Today, we will be using ACL connections, which are equivalent to a TCP reliable bytestream (although they can be configured to be less reliable and faster).
In contrast, audio usually uses SCO connections, which are more like sending UDP packets not knowing whether they will get there.
Once a connection is established, the format for an ACL data packet is very simple:

<p align="center">
<img src="./images/HCI%20acl%20format.png" alt="Event format - HCI pdf page 776" width="800" />
</p>

The first octet and lower half of the second octet designate the connection handle to which this data packet should be sent.
The upper half of the second octet stores two flags, which we will set to zero today.
Since your handle variable will already have those upper bits set to zero, you can simply send the handle in little-endian format as if it took up the first two octets.
What follows is a massive 16-bit length field and the data.

Let's start by filling in the remaining TODO's in `bt.c` to be able to send and receive ACL packets.
Next, complete the TODO's in `5-connect.c` or `5-accept-connection.c` depending on whether you are Alice or Bob.
These files contain a few commands at the top to show a good template for sending commands (including asserting all the replied values), but they are not strictly necessary.
You will need to find the commands that you need to use in the [HCI section of the Bluetooth spec](./docs/BT%205.1%20HCI.pdf).
Once you establish a connection, you should be able to type lines into your terminal and they should come out of your partner's terminal (telnet-style).

Some important facts:

* So far, we have been expecting to receive a Command Complete for every command, since they have all been synchronous. However, many connection-related commands actually respond with a Command Status event since they are asynchronous.
* It is common for the HCI_Create_Connection event to fail sometimes, so you might want to try a few times or even build a loop that retries the connection until it works.

## Checkoff

Show us that you can connect to your friend's Pi and send an arbitrary message. Congrats, you are now a wireless master!

## Extensions

The possibilities really are endless here. Some ideas below.

Should be relatively simple:

* Enable encryption.
* Rewrite our bootloader to listen on Bluetooth instead of serial. Since ACL data can pretty much be used as a reliable bytestream, you can send an arbitrary program using Bluetooth (I did this on the Mango Pi).
* Advertise your device so you see it as connectable on your phone (actually connecting is not so easy).
* Listen for other devices advertising.
* Do something with Bluetooth Low Energy (the general scheme is similar, but LE has its own set of commands and events).
* Increase the baud rate of the UART connection with the BT device (to make firmware uploads faster).
* Store the firmware in the SD card to avoid sending it over the serial connection every time we run a new program with my-install.

Hard, probably worthy of a final project:

* Build a Bluetooth jammer (you probably need to make a custom firmware).
* The chip is a combined WiFi+Bluetooth. Connect to a WiFi network!
* Send/receive audio. You can make the Pi pretend to be a phone or a pair of headphones and connect to a real device.
* Pretend to be a mouse or keyboard. Connect to a computer or phone and control the mouse using the Pi. This is probably the most doable from this list.
