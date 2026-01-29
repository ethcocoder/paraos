#ifndef COMMON_IO_H
#define COMMON_IO_H

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

static inline void sti() {
    asm volatile ("sti");
}

static inline void cli() {
    asm volatile ("cli");
}

#endif
