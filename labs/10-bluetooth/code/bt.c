// Javier Garcia Nieto <jgnieto@cs.stanford.edu>. CS340LX Fall 2025.

#define CQE_T void*

#include "bt.h"
#include "hci-consts.h"
#include "rpi.h"
#include "pl011.h"
#include "circular.h"

#include <stdbool.h>

static struct {
    cq_t acl_rx_buffer;
    cq_t event_rx_buffer;
    bool initialized;

    // how many commands can we send without receiving events
    unsigned n_commands_can_send;
} module;

void bt_init(void) {
    assert(!module.initialized);

    module.initialized = true;
    module.n_commands_can_send = 1;

    todo("Call init on the pl011 module.");

    cq_init(&module.acl_rx_buffer, true);
    cq_init(&module.event_rx_buffer, true);
    kmalloc_init(100); // no free, so get a bunch of space

    todo("Set the Bluetooth enable pin to on and wait for 800ms. Remember to use gpio_ext.");
}

/*
 * HCI ACL Data:
 *   0                   1                   2                   3
 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |         Handle        | Flags |       Data Total Length       |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                                                               |
 *  |                              Data                             |
 *  |                               ...                             |
 *  |                                                               |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
static void _receive_acl_data(struct hci_acl_data_pkt *acl) {
    assert(module.initialized);

    // Warning: make sure that you only take exactly the number of bytes
    // necessary.
    todo("Process incoming ACL data packet in a blocking fashion");
}

/*
 * HCI Event:
 *   0                   1                   2                   3
 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |   Event Code  |   Params Len  |        Event Parameter 0      |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |        Event Parameter 1      | Evnt Param 2  | Evnt Param 3  |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                               ...                             |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |      Event Parameter N-1      |       Event Parameter N       |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
static void _receive_event(struct hci_event_pkt *evt) {
    assert(module.initialized);

    // Warning: make sure that you only take exactly the number of bytes
    // necessary.
    todo("Process incoming HCI event packet in a blocking fashion");

    // The EVENT_COMMAND_COMPLETE and EVENT_COMMAND_STATUS events include flow
    // control information that tells us how many commands we can send until
    // we get another one of these events giving us permission to send more.
    if (evt->event_code == EVENT_COMMAND_COMPLETE && evt->params_len >= 1) {
        u8 n_cmds = evt->params[0];
        module.n_commands_can_send = n_cmds;
    } else if (evt->event_code == EVENT_COMMAND_STATUS && evt->params_len >= 2) {
        u8 n_cmds = evt->params[1];
        module.n_commands_can_send = n_cmds;
    }
}

// Blockingly receive a single packet.
static void _receive_packet(void) {
    assert(module.initialized);
    u8 packet_type = pl011_get8();
    switch (packet_type) {
    case HCI_EVENT: {
        struct hci_event_pkt *evt = kmalloc_notzero(sizeof(*evt));
        _receive_event(evt);
        cq_push(&module.event_rx_buffer, evt);
        break;
    }
    case HCI_ACL_DATA: {
        struct hci_acl_data_pkt *acl = kmalloc_notzero(sizeof(*acl));
        _receive_acl_data(acl);
        cq_push(&module.acl_rx_buffer, acl);
        break;
    }
    default:
        panic("Unexpected BT packet type: %x\n", packet_type);
    }
}


/*
 * HCI Command:
 *   0                   1                   2                   3
 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |           OpCode              |  Params len   |  Parameter 0  |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                           Parameter 1                         |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                               ...                             |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |           Parameter N-1       |           Parameter N         |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
void bt_send_command(struct hci_command_pkt *cmd) {
    assert(module.initialized);

    // Wait until we are allowed to send a command. Ideally, here we would have
    // a timeout and reset the Bluetooth module if we wait too long.
    while (module.n_commands_can_send == 0) {
        _receive_packet();
    }
    module.n_commands_can_send--;

    todo("Send HCI command packet. Be careful to send in little endian!");
}

/*
 * HCI ACL Data:
 *   0                   1                   2                   3
 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |         Handle        | Flags |       Data Total Length       |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                                                               |
 *  |                              Data                             |
 *  |                               ...                             |
 *  |                                                               |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
void bt_send_acl_data(struct hci_acl_data_pkt *acl) {
    assert(module.initialized);
    todo("Send HCI ACL data packet. Set the flags to 0.");
}

extern unsigned char BCM43430A1_hcd[];
extern unsigned int BCM43430A1_hcd_len;

void bt_upload_firmware(void) {
    assert(module.initialized);

    // Send a reset and a load firmware command
    struct hci_command_pkt cmd = { 0 };
    cmd.opcode = CMD_RESET;
    bt_send_command(&cmd);
    cmd.opcode = CMD_BCM_LOAD_FIRMWARE;
    bt_send_command(&cmd);

    // Wait for command complete for the load firmware command
    while (1) {
        struct hci_event_pkt *event = bt_receive_event();
        if (event->event_code == EVENT_COMMAND_COMPLETE) {
            assert(event->params_len >= 3);
            u16 opcode = event->params[1] | (event->params[2] << 8);
            if (opcode == CMD_BCM_LOAD_FIRMWARE)
                break;
        }
    }

    printk("About to upload firmware...\n");

    delay_ms(50); // same time as linux

    int n_packets_sent = 0;
    todo("Iterate over the firmware and send it in chunks. Keep track of the number of packets in n_packets_sent.");

    assert(n_packets_sent == 121); // sanity check based on Javier's code

    // Purge event queue
    todo("Purge all the command complete events, (hint: you know that "
            "you sent packets_sent commands");

    printk("Waiting 250ms for patch to take effect...\n");
    delay_ms(250); // same time as linux
}

// Functions for pulling from the receive buffers

struct hci_event_pkt *bt_receive_event(void) {
    assert(module.initialized);
    while (cq_empty(&module.event_rx_buffer)) {
        _receive_packet();
    }
    return cq_pop(&module.event_rx_buffer);
}

struct hci_event_pkt *bt_receive_event_async(void) {
    assert(module.initialized);
    if (cq_empty(&module.event_rx_buffer)) {
        if (pl011_has_data()) {
            _receive_packet();
            if (cq_empty(&module.event_rx_buffer)) {
                return NULL;
            }
        } else {
            return NULL;
        }
    }
    return cq_pop(&module.event_rx_buffer);
}

struct hci_acl_data_pkt *bt_receive_acl(void) {
    assert(module.initialized);
    while (cq_empty(&module.acl_rx_buffer)) {
        _receive_packet();
    }
    return cq_pop(&module.acl_rx_buffer);
}

struct hci_acl_data_pkt *bt_receive_acl_async(void) {
    assert(module.initialized);
    if (cq_empty(&module.acl_rx_buffer)) {
        if (pl011_has_data()) {
            _receive_packet();
            if (cq_empty(&module.acl_rx_buffer)) {
                return NULL;
            }
        } else {
            return NULL;
        }
    }
    return cq_pop(&module.acl_rx_buffer);
}
