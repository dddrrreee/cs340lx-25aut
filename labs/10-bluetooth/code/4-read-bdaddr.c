// Javier Garcia Nieto <jgnieto@cs.stanford.edu>. CS340LX Fall 2025.

#include "demand.h"
#include "rpi.h"
#include "bt.h"
#include "hci-consts.h"

void notmain(void) {
    bt_init();
    bt_upload_firmware();

    struct hci_command_pkt cmd = {0};
    cmd.opcode = CMD_READ_BD_ADDR;
    bt_send_command(&cmd);

    struct hci_event_pkt *evt = bt_receive_event();
    assert(evt->event_code == EVENT_COMMAND_COMPLETE);
    assert(memcmp(&evt->params[1], &cmd.opcode, 2) == 0);
    assert(evt->params_len == 10); // opcode (2) + num commands (1) + status (1) + bdaddr (6)
    assert(evt->params[3] == 0); // status == 0 (success)

    printk("BD_ADDR: ");
    for (int i = 5; i >= 0; i--) {
        printk("%02x", evt->params[4 + i]);
        if (i != 0) {
            printk(":");
        }
    }
    printk("\n");
}
