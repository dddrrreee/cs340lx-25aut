// Javier Garcia Nieto <jgnieto@cs.stanford.edu>. CS340LX Fall 2025.

#include "bt.h"
#include "demand.h"
#include "hci-consts.h"
#include "rpi.h"
#include <string.h>

// Pick your own local name!
static const char LOCAL_NAME[] = "RPI connector";

// Peer address in big endian format (note that you have to send in little endian!)
static u8 peer_addr_be[] = { 
    0x1e, 0x70, 0x01, 0x14, 0x68, 0x05, // equivalent to 1e:70:01:14:68:05
};

void notmain(void) {
    bt_init();
    bt_upload_firmware();

    u8 peer_addr_le[6];
    for (int i = 0; i < 6; i++) {
        peer_addr_le[i] = peer_addr_be[5 - i];
    }

    struct hci_command_pkt cmd = {0};
    struct hci_event_pkt *evt;
    struct hci_acl_data_pkt *acl_recv;

    // Read BD_ADDR
    cmd.opcode = CMD_READ_BD_ADDR;
    bt_send_command(&cmd);

    evt = bt_receive_event();
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

    // Write local name (for fun!)
    cmd.opcode = CMD_WRITE_LOCAL_NAME;
    cmd.params_len = 248;
    memcpy(cmd.params, LOCAL_NAME, sizeof(LOCAL_NAME));
    bt_send_command(&cmd);

    evt = bt_receive_event();
    assert(evt->event_code == EVENT_COMMAND_COMPLETE);
    assert(evt->params_len == 4);
    assert(memcmp(&evt->params[1], &cmd.opcode, 2) == 0);
    assert(evt->params[3] == 0); // status success

    // Check that local name was written
    cmd.opcode = CMD_READ_LOCAL_NAME;
    cmd.params_len = 0;
    bt_send_command(&cmd);

    evt = bt_receive_event();
    assert(evt->event_code == EVENT_COMMAND_COMPLETE);
    assert(evt->params_len == 252);
    assert(memcmp(&evt->params[1], &cmd.opcode, 2) == 0);
    assert(evt->params[3] == 0); // status success
    printk("Local name: %s\n", &evt->params[4]);

    // Read buffer size
    cmd.opcode = CMD_READ_BUFFER_SIZE;
    cmd.params_len = 0;
    bt_send_command(&cmd);

    evt = bt_receive_event();
    assert(evt->event_code == EVENT_COMMAND_COMPLETE);
    assert(evt->params_len == 11);
    assert(memcmp(&evt->params[1], &cmd.opcode, 2) == 0);
    assert(evt->params[3] == 0); // status success

    printk("Max ACL data packet length: %d\n", 
        evt->params[4] | (evt->params[5] << 8));
    printk("Max number ACL data packets: %d\n", 
        evt->params[7] | (evt->params[8] << 8));

    todo("Use HCI_Create_Connection to connect to the peer device.");
    // Use 0xcc18 as the packet types (allow all).
    // No clock offset and no role switch.

    // Since this is non-blocking, we expect a command status event rather than
    // a command complete event
    evt = bt_receive_event();
    assert(evt->event_code == EVENT_COMMAND_STATUS);
    assert(evt->params_len == 4);
    assert(memcmp(&evt->params[2], &cmd.opcode, 2) == 0);
    assert(evt->params[0] == 0); // status successfully pending

    // Expect connection complete even if connection fails
    u8 handle[2];
    evt = bt_receive_event();
    assert(evt->event_code == EVENT_CONNECTION_COMPLETE);
    todo("Verify Connection Complete event parameters");
    todo("Copy connection handle from event parameters");

    printk("Connected! Handle: %02x%02x\n", handle[1], handle[0]);

    while (1) {
        evt = bt_receive_event_async();
        if (evt) {
            // very frequent event, so don't print always
            if (evt->event_code != EVENT_NUMBER_OF_COMPLETED_PACKETS) {
                printk("Received event code %02x\n", evt->event_code);
                if (evt->event_code == EVENT_DISCONNECTION_COMPLETE) {
                    printk("\nDisconnected\n");
                    break;
                }
            }
        }

        acl_recv = bt_receive_acl_async();
        if (acl_recv) {
            for (int i = 0; i < acl_recv->data_len; i++) {
                uart_put8(acl_recv->data[i]);
            }
        }
        
        if (uart_has_data()) {
            struct hci_acl_data_pkt acl_send = {0};
            memcpy(&acl_send.handle, handle, 2);
            for (int i = 0; uart_has_data() && i < 1021; i++) {
                acl_send.data[i] = uart_get8();
                acl_send.data_len++;
            }
            bt_send_acl_data(&acl_send);
        }
    }
}
