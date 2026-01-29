#include "../include/io.h"

extern void terminal_writestring(const char*);
extern void terminal_putchar(char);

extern void create_excitation(const char* name, void (*function)(void), uint32_t initial_energy);

void task_observer_interaction(void) {
    terminal_writestring(" [ OBSERVER ] Interaction Detected: Collapsing Possibilities...\n");
}

void keyboard_handler() {
    uint8_t scancode = inb(0x60);
    
    if (!(scancode & 0x80)) {
        // High energy excitation from user input
        create_excitation("User Observation", task_observer_interaction, 20);
    }
    
    // Send EOI
    outb(0x20, 0x20);
}
