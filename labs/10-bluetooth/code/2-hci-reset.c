// Javier Garcia Nieto <jgnieto@cs.stanford.edu>. CS340LX Fall 2025.

#include "rpi.h"
#include "bt.h"
#include "hci-consts.h"

void notmain(void) {
    bt_init();

    // Send HCI_Reset command
    struct hci_command_pkt cmd = {0};
    cmd.opcode = CMD_RESET;
    bt_send_command(&cmd);

    // Check that result matches expected
    struct hci_event_pkt *evt = bt_receive_event();
    assert(evt->event_code == EVENT_COMMAND_COMPLETE);
    assert(evt->params_len == 4); // opcode (2) + num commands (1) + status (1)
    assert(evt->params[1] == (CMD_RESET & 0xff)); // opcode LSB
    assert(evt->params[2] == (CMD_RESET >> 8));   // opcode
    assert(evt->params[3] == 0); // status == 0 (success)

    printk("Success!\n");
}

