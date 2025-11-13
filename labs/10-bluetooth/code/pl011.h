#ifndef PL011_H
#define PL011_H

#include <stdint.h>

void pl011_init(void);

// get one byte from the pl011
uint8_t pl011_get8(void);
// put one byte on the pl011:
void pl011_put8(uint8_t c);

// returns -1 if no byte, the value otherwise.
int pl011_get8_async(void);

// 0 = no data, 1 = at least one byte
int pl011_has_data(void);

// 0 = no space, 1 = space for at least 1 byte
int pl011_can_put8(void);

// flush out the tx fifo
void pl011_flush_tx(void);

#endif // PL011_H
