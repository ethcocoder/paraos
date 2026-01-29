#include <stdint.h>
#include <stddef.h>
#include "../include/field.h"
#include "../include/gdt.h"
#include "../include/idt.h"
#include "../include/io.h"
#include "../include/multiboot.h"
#include "../include/graphics.h"
#include "../include/gui.h"

/* Hardware text mode color constants (for text mode fallback) */
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_WHITE = 15,
};

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

typedef struct registers {
    uint32_t ds;                  
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; 
    uint32_t int_no, err_code;    
    uint32_t eip, cs, eflags, useresp, ss; 
} registers_t;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    terminal_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
    } else {
        const size_t index = terminal_row * VGA_WIDTH + terminal_column;
        terminal_buffer[index] = vga_entry(c, terminal_color);
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT)
                terminal_row = 0;
        }
    }
}

void terminal_writestring(const char* data) {
    for (size_t i = 0; data[i] != 0; i++)
        terminal_putchar(data[i]);
}

extern void isr0();
extern void irq1();

void isr_handler(registers_t regs) {
    terminal_writestring("[ OBSERVER ] Exception!\n");
}

extern void gui_handle_key(uint8_t scancode);

void irq_handler(registers_t regs) {
    if (regs.int_no == 33) {
        // Read scancode
        uint8_t scancode = inb(0x60);
        
        // Pass to GUI if graphics mode is active
        if (graphics_is_available()) {
            gui_handle_key(scancode);
        }
        
        // Send EOI
        outb(0x20, 0x20);
    }
}

extern void pic_remap(int offset1, int offset2);

/* Dummy "Tasks" (Field Excitations) - for compatibility */
void task_kernel_monitor(void) {
    // Silent in graphics mode
}

void task_memory_dream(void) {
    // Silent in graphics mode
}

/* Main Entry Point */
void kernel_main(uint32_t magic, multiboot_info_t* mbi) {
    /* Graphics Mode Detection */
    if (magic == 0x2BADB002 && (mbi->flags & (1 << 12))) {
        // High-Resolution Graphics (Multiboot)
         graphics_init((uint32_t)mbi->framebuffer_addr,
                     mbi->framebuffer_width,
                     mbi->framebuffer_height,
                     mbi->framebuffer_pitch,
                     mbi->framebuffer_bpp);
    } else {
        // Fallback to Text Mode (Safest)
        // We tried forcing graphics but got stride mismatches (green stripes).
        // Let's stabilize the timeline with text mode first.
        graphics_init(0, 0, 0, 0, 0); // This triggers text mode in graphics.c
    }

extern void pic_remap(int, int);

    if (graphics_is_available()) {
        /* GRAPHICS MODE */
        init_gdt();
        init_idt();
        pic_remap(0x20, 0x28);
        gui_init();
        
        // Enable interrupts
        asm volatile("sti");
        
        /* Enter GUI Loop */
        while(1) {
            gui_update(0.016f); // 60 FPS delta
            gui_render();
        }
    } else {
        /* TEXT MODE FALLBACK */
        // Explicitly use VGA Text Mode memory
        uint16_t* terminal_buffer = (uint16_t*)0xB8000;
        
        // Clear screen (Blue background)
        for(int y=0; y<25; y++) {
            for(int x=0; x<80; x++) {
                const size_t index = y * 80 + x;
                terminal_buffer[index] = (uint16_t)' ' | (uint16_t)((0x1F) << 8); // White on Blue
            }
        }
        
        // Print Welcome Message
        const char* msg = "ParadoxOS Kernel v0.3.0 - Cognitive Field Initialized.";
        int msg_len = 0; while(msg[msg_len]) msg_len++;
        
        for(int i=0; i<msg_len; i++) {
            terminal_buffer[i] = (uint16_t)msg[i] | (uint16_t)((0x1F) << 8);
        }
        
        // Secondary Message
        const char* msg2 = "System Stable. Ready for Observer.";
        for(int i=0; i<100; i++) { // Simple delay
             asm volatile("nop");
        }
        for(int i=0; i<34; i++) {
            terminal_buffer[80 + i] = (uint16_t)msg2[i] | (uint16_t)((0x1E) << 8); // Yellow on Blue
        }
        
        init_gdt();
        init_idt();
        pic_remap(0x20, 0x28);
        asm volatile("sti");
        
        while(1) {
            asm volatile("hlt");
        }
    }
}

