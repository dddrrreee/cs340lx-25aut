// Javier Garcia Nieto <jgnieto@cs.stanford.edu>. CS340LX Fall 2025.

#include "rpi.h"
#include "pl011.h"
#include "bt.h"
#include "gpio-high.h"

void notmain(void) {
    pl011_init();

    gpio_hi_set_output(BT_EN); // BT_EN
    gpio_hi_set_on(BT_EN);
    delay_ms(800);

    u8 hci_reset[] = {
        0x01,        // HCI Command packet type
        0x03, 0x0C,  // OpCode: 0x0C03 (little endian)
        0x00         // Parameter length: 0
    };

    for (int i = 0; i < sizeof hci_reset; i++) {
        pl011_put8(hci_reset[i]);
    }

    u8 hci_expect[] = {
        0x04,        // Event packet type
        0x0E,        // Command Complete event
        0x04,        // Parameter length = 4 bytes
        0x01,        // Num HCI Command Packets (could be anything)
        0x03,        // Opcode low byte (echo)
        0x0C,        // Opcode high byte (echo)
        0x00,        // Status: SUCCESS
    };

    u8 hci_reply[sizeof hci_expect];

    for(int i = 0; i < sizeof hci_expect; i++) {
        hci_reply[i] = pl011_get8();
    }

    for(int i = 0; i < sizeof hci_expect; i++) {
        u8 got = hci_reply[i];
        u8 exp = hci_expect[i];
        if (i == 3) {
            output("SUCCESS: byte[%d]=%x (can vary)\n", i, got);
        } else if (got != exp) {
            panic("ERROR: expected byte[%d]=%x, have %x\n",
                i, exp, got);
        } else {
            output("SUCCESS: byte[%d]=%x (matched)\n", i, got);
        }
    }
}

