// engler: simple program to echo a tty device (eg
// the lidar).  is a stripped down version of the 
// <my-install.c> driver.
//
// usage: 
//      lidar-cat <device>
//
// why: 
//  - we want to see what the lidar is spitting out.
//  - most people use "screend" to interaact with ttyUSB devices, 
//    but (1) it's really a pain and (2) can't pull in the 
//    results into computation (eg decode the packets)
//  - you can use variants of this code to program a bunch of 
//    different tty-usb devices.
//
// result:
//  - can write the decoding logic on unix, which is a bit
//    more forgiving.
//  - can adapt this code and use the lidar in a unix program
//    (e.g., to do screen animations on your laptop etc).
//
// limitations:
//  - only does echo.
//  - baud rate is hard coded.
//  - does not use your "find_usb" code from 140e.
//
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>

#include "libunix.h"

int main(int argc, char *argv[]) { 
	const char *dev_name = 0;

    if(argc == 2)
        dev_name = argv[1];
    else
        panic("need one argument: device to echo\n");
    
    // on linux, baud rates are defined in:
    //  /usr/include/asm-generic/termbits.h
    int baud = B230400;  // from datasheet.

	int fd = open_tty(dev_name);
    fd = set_tty_to_8n1(fd, baud, 1);
    int ret;

    // note: C will concat string constants
    output("about to echo the raw lidar output!\n"
           "    this shows if device up\n"
           "what to do: turn this into lidar a <tcpdump>\n"
           "and print the actual data\n");

    // just echos the raw bytes: will be a bunch of garbage.
    // use the data sheet to decode and print.
    // (maybe worth having a "raw" and "decode" option, idk)
    unsigned char c;
    while((ret = read(fd, &c, 1)) == 1)
        output("%c", c);

    output("read failed (ret=%d): done!\n", ret);
	return 0;
}
