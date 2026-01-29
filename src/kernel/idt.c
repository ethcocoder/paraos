#include "../include/idt.h"
#include <string.h>

extern void idt_flush(uint32_t);

idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    idt_entries[num].flags   = flags /* | 0x60 */;
}

void init_idt() {
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    // Zero out the IDT
    for(int i = 0; i < 256; i++) {
        idt_entries[i].base_lo = 0;
        idt_entries[i].base_hi = 0;
        idt_entries[i].sel = 0;
        idt_entries[i].always0 = 0;
        idt_entries[i].flags = 0;
    }

    // Add ISR calls here (we will define them in assembly)
    // For now, we'll just initialize the pointer
    
    idt_flush((uint32_t)&idt_ptr);
}
