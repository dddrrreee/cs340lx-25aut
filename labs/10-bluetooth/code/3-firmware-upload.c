// Javier Garcia Nieto <jgnieto@cs.stanford.edu>. CS340LX Fall 2025.

#include "bt.h"
#include "rpi.h"

void notmain(void) {
    bt_init();
    bt_upload_firmware();

    printk("Success!\n");
}
