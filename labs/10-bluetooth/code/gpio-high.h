#ifndef GPIO_HIGH_H
#define GPIO_HIGH_H

#include "rpi.h"

#define REG_SIZE            4
#define FSEL_PINS_PER_REG   10
#define SET_PINS_PER_REG    32
#define PUDCLK_PINS_PER_REG 32

enum {
    GPIO_BASE       = 0x20200000,
    gpio_set0       = (GPIO_BASE + 0x1C),
    gpio_clr0       = (GPIO_BASE + 0x28),
    gpio_lev0       = (GPIO_BASE + 0x34),
    gpio_fsel0      = (GPIO_BASE + 0x00),
    gpio_pud        = (GPIO_BASE + 0x94),
    gpio_pudclk0    = (GPIO_BASE + 0x98)
};

static inline void gpio_hi_set_function(unsigned pin, gpio_func_t function) {
    assert(function >= 0 && function <= 0b111);
    uint32_t fsel_addr = gpio_fsel0 + (pin / FSEL_PINS_PER_REG) * REG_SIZE;
    uint32_t reg_content = GET32(fsel_addr);
    uint32_t shift = (pin % FSEL_PINS_PER_REG) * 3;
    uint32_t mask = ~(0b111 << shift);
    reg_content &= mask;
    reg_content |= function << shift;
    PUT32(fsel_addr, reg_content);
}

static inline void gpio_hi_set_output(unsigned pin) {
    gpio_set_function(pin, GPIO_FUNC_OUTPUT);
}

static inline void gpio_hi_set_on(unsigned pin) {
    uint32_t set_addr = gpio_set0 + (pin / SET_PINS_PER_REG) * REG_SIZE;
    uint32_t new_content = 0b1 << (pin % SET_PINS_PER_REG);
    PUT32(set_addr, new_content);
}

static inline void gpio_hi_pud_off(unsigned int pin) {
    PUT32(gpio_pud, 0);
    delay_cycles(150);
    int clockn = pin / PUDCLK_PINS_PER_REG;
    uint32_t pudclk_addr = gpio_pudclk0 + clockn * REG_SIZE;
    PUT32(pudclk_addr, 0b1 << (pin % PUDCLK_PINS_PER_REG));
    delay_cycles(150);
    PUT32(gpio_pud, 0);
    PUT32(pudclk_addr, 0);
}

#endif // GPIO_HIGH_H
