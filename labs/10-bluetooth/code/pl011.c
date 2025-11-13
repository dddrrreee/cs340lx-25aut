// Javier Garcia Nieto <jgnieto@cs.stanford.edu>. CS340LX Fall 2025.

#define CQ_N 131072

#include "gpio.h"
#include "rpi.h"
#include "pl011.h"
#include "circular.h"
#include "rpi-interrupts.h"

#include <stdbool.h>

#define GPIO_BT_CTS 30
#define GPIO_BT_RTS 31
#define GPIO_BT_TX 32
#define GPIO_BT_RX 33

#define ADDR(x) (0x20201000 + (x))

// Assuming 48 MHz UART clock (can be checked with mailbox), use 115200 baud
// 48,000,000 / (16 * (26 + 3/64)) = 115177, close enough to 115200
#define BAUD_INT_VAL 26
#define BAUD_FRAC_VAL 3 // setting to 0 would be closer to 43438 baud?

#define UART0_DR        ADDR(0x000)
#define UART0_RSRECR    ADDR(0x004)
#define UART0_FR        ADDR(0x018)
#define UART0_ILPR      ADDR(0x020)
#define UART0_IBRD      ADDR(0x024)
#define UART0_FBRD      ADDR(0x028)
#define UART0_LCRH      ADDR(0x02C)
#define UART0_CR        ADDR(0x030)
#define UART0_IFLS      ADDR(0x034)
#define UART0_IMSC      ADDR(0x038)
#define UART0_RIS       ADDR(0x03c)
#define UART0_MIS       ADDR(0x040)
#define UART0_ICR       ADDR(0x044)
#define UART0_DMACR     ADDR(0x048)
#define UART0_ITCR      ADDR(0x080)
#define UART0_ITIP      ADDR(0x084)
#define UART0_ITOP      ADDR(0x088)
#define UART0_TDR       ADDR(0x08c)

static struct {
    cq_t rx_buffer;
    bool initialized;
} module;


// Only called within interrupt so no need for device barriers
static uint8_t _get8(void) {
    todo("Read a character from the data register. Make sure to mask out the error bits");
    return 0;
}

// Only called within interrupt so no need for device barriers
static int _has_data(void) {
    todo("Use the FR register to check if data is available to read");
    return 0;
}

void interrupt_vector(void) {
    dev_barrier();
    uint32_t mis = GET32(UART0_MIS);

    bool rx_interrupt = (mis & (1 << 4)) != 0;
    bool rx_timeout_interrupt = (mis & (1 << 6)) != 0;

    if (rx_interrupt || rx_timeout_interrupt) { // RX interrupt
        while (_has_data()) {
            cq_push(&module.rx_buffer, _get8());
        }
        if (rx_interrupt) {
            todo("Clear RX interrupt in ICR register");
        }
        if (rx_timeout_interrupt) {
            todo("Clear RX timeout interrupt in ICR register");
        }
    } else {
        panic("Unexpected UART interrupt: MIS=0x%08x\n", mis);
    }
    dev_barrier();
}

void pl011_init(void) {
    assert(!module.initialized);
    module.initialized = true;

    cq_init(&module.rx_buffer, 1);

    while (pl011_has_data())
        pl011_get8(); // flush any spurious data

    pl011_flush_tx();

    // disable UART
    PUT32(UART0_CR, 0);

    dev_barrier();

    todo("Set up GPIO pins for UART0 functionality and disable pull-ups/downs."
            "We need all 4 pins. Remember to use gpio_hi.");

    dev_barrier();

    interrupt_init();

    // Set up UART
    todo("Clear the interrupts using the ICR register");
    todo("Set the integer & fractional baud rate registers (IBRD, FBRD)");
    todo("Set the interrupt mask register (IMSC) to enable RX and RX timeout interrupts");
    todo("Set the interrupt FIFO level select register (IFLS) to generate interrupts as soonq as data is available");
    todo("Set the line control register (LCRH) to 8n1 and enable FIFOs");

    // Enable UART
    PUT32(UART0_CR, 0xb01);  // enable RTS, TX, RX, UART

    dev_barrier();

    todo("Write to the IRQ_Enable_2 register to enable UART interrupts");

    enable_interrupts();

    dev_barrier();
}

int pl011_can_put8(void) {
    assert(module.initialized);
    dev_barrier();
    int ret;
    todo("Check that device can accept a character for transmission");
    dev_barrier();
    return ret;
}

void pl011_put8(uint8_t c) {
    while (!pl011_can_put8())
        rpi_wait();
    todo("Write the character to the data register");
    dev_barrier();
}


int pl011_tx_is_empty(void) {
    assert(module.initialized);
    dev_barrier();
    int ret;
    todo("Check that device is not busy and empty");
    dev_barrier();
    return ret;
}

// Below are functions we provide
int pl011_has_data(void) {
    assert(module.initialized);
    return !cq_empty(&module.rx_buffer);
}

uint8_t pl011_get8(void) {
    assert(module.initialized);
    return cq_pop(&module.rx_buffer);
}


void pl011_flush_tx(void) {
    assert(module.initialized);
    while (!pl011_tx_is_empty())
        rpi_wait();
}

int pl011_get8_async(void) { 
    assert(module.initialized);
    if (!pl011_has_data())
        return -1;
    return pl011_get8();
}

