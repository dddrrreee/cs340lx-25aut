// Javier Garcia Nieto <jgnieto@cs.stanford.edu>. CS340LX Fall 2025.

#ifndef BT_H
#define BT_H

#include <stdint.h>

#define BT_EN   45

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define OPCODE(ogf, ocf) (((ocf) & 0x03FF) | (((ogf) & 0x3F) << 10))
#define OGF(opcode) ((opcode) >> 10)
#define OCF(opcode) ((opcode) & 0x03FF)

enum bt_packet_type {
    HCI_CMD =       0x01,
    HCI_ACL_DATA =  0x02,
    HCI_SCO_DATA =  0x03,
    HCI_EVENT =     0x04,
};

struct hci_command_pkt {
    u16 opcode;
    u8 params_len;
    u8 params[255];
};

struct hci_event_pkt {
    u8 event_code;
    u8 params_len;
    u8 params[255];
};

struct hci_acl_data_pkt {
    u16 handle;
    u16 data_len;
    u8 data[1021]; // BT max is 2^16 but our BT chip has a max of 1021
};

void bt_init(void);
void bt_upload_firmware(void);
void bt_send_command(struct hci_command_pkt *cmd);
void bt_send_acl_data(struct hci_acl_data_pkt *acl);

struct hci_event_pkt *bt_receive_event(void);
struct hci_event_pkt *bt_receive_event_async(void);

struct hci_acl_data_pkt *bt_receive_acl(void);
struct hci_acl_data_pkt *bt_receive_acl_async(void);


#endif // BT_H
