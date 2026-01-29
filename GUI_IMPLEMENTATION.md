# ParadoxOS GUI Implementation Guide

## Phase 4.1: Framebuffer Graphics (FIRST STEP)

### Goal
Replace VGA text mode (80x25 characters) with **graphical framebuffer** for drawing the "Anchor Universe" (desktop).

---

## Step 1: VESA/Multiboot Framebuffer

### Modify `boot.S` to request framebuffer mode

Add to Multiboot header:
```asm
.set FLAGS,    ALIGN | MEMINFO | VIDEO
.set MODE_TYPE, 0    /* 0 = linear framebuffer */
.set WIDTH,    1024
.set HEIGHT,   768
.set DEPTH,    32

/* After existing multiboot fields, add: */
.long MODE_TYPE
.long WIDTH
.long HEIGHT
.long DEPTH
```

### Receive framebuffer info from bootloader

In `kernel.c`, read Multiboot structure:
```c
typedef struct {
    uint32_t flags;
    // ... other fields ...
    uint32_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
} multiboot_info_t;

void kernel_main(multiboot_info_t* mbi) {
    if (mbi->flags & (1 << 12)) {
        // Framebuffer available!
        init_graphics(mbi->framebuffer_addr, 
                     mbi->framebuffer_width,
                     mbi->framebuffer_height);
    }
}
```

---

## Step 2: Graphics Driver (`src/kernel/graphics.c`)

### Basic Pixel Drawing
```c
uint32_t* framebuffer;
uint32_t fb_width;
uint32_t fb_height;

void init_graphics(uint32_t addr, uint32_t width, uint32_t height) {
    framebuffer = (uint32_t*)addr;
    fb_width = width;
    fb_height = height;
    
    // Clear to deep blue (ParadoxOS theme)
    for(uint32_t i = 0; i < width * height; i++) {
        framebuffer[i] = 0x001a1a3e; // RGB
    }
}

void draw_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x < fb_width && y < fb_height) {
        framebuffer[y * fb_width + x] = color;
    }
}

void draw_circle(uint32_t cx, uint32_t cy, uint32_t radius, uint32_t color) {
    // Midpoint circle algorithm
    int x = radius;
    int y = 0;
    int err = 0;
    
    while (x >= y) {
        draw_pixel(cx + x, cy + y, color);
        draw_pixel(cx + y, cy + x, color);
        draw_pixel(cx - y, cy + x, color);
        draw_pixel(cx - x, cy + y, color);
        draw_pixel(cx - x, cy - y, color);
        draw_pixel(cx - y, cy - x, color);
        draw_pixel(cx + y, cy - x, color);
        draw_pixel(cx + x, cy - y, color);
        
        if (err <= 0) {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}
```

---

## Step 3: First Visual - "Anchor Universe"

### Create pulsing central sphere
```c
typedef struct {
    float x, y;        // Position
    float radius;      // Current radius
    float energy;      // Pulsing energy
    uint32_t color;
} universe_t;

universe_t anchor;

void init_anchor_universe() {
    anchor.x = fb_width / 2;
    anchor.y = fb_height / 2;
    anchor.radius = 50.0f;
    anchor.energy = 1.0f;
    anchor.color = 0x4a9eff; // Bright blue
}

void render_anchor_universe() {
    // Pulse effect
    anchor.energy += 0.01f;
    float pulse = 1.0f + 0.1f * sinf(anchor.energy);
    float current_radius = anchor.radius * pulse;
    
    // Draw concentric circles (energy waves)
    for(int i = 0; i < 5; i++) {
        uint32_t alpha = 255 - (i * 50);
        uint32_t color = (alpha << 24) | (anchor.color & 0x00FFFFFF);
        draw_circle(anchor.x, anchor.y, current_radius + i*10, color);
    }
}
```

---

## Step 4: "Observer Birth" Screen (Welcome)

### First GUI Screen
```c
void render_welcome_screen() {
    // Dark background
    clear_screen(0x0a0a1e);
    
    // Central universe forming
    static float formation = 0.0f;
    formation += 0.02f;
    
    if (formation < 1.0f) {
        // Gradually expand from point
        draw_circle(fb_width/2, fb_height/2, 
                   formation * 100, 0x4a9eff);
    } else {
        render_anchor_universe();
        
        // Text overlay
        draw_text(fb_width/2 - 200, fb_height - 100,
                 "Press Any Key to Begin Observation",
                 0xffffff);
    }
}
```

---

## Step 5: Integration with Current System

### Modify `kernel_main()`
```c
void kernel_main(multiboot_info_t* mbi) {
    // Graphics mode
    if (mbi->flags & (1 << 12)) {
        init_graphics(mbi->framebuffer_addr, 
                     mbi->framebuffer_width,
                     mbi->framebuffer_height);
        
        init_anchor_universe();
        
        // Main loop
        gui_state = GUI_WELCOME;
        while(1) {
            switch(gui_state) {
                case GUI_WELCOME:
                    render_welcome_screen();
                    break;
                case GUI_REGISTRATION:
                    render_registration_screen();
                    break;
                case GUI_DESKTOP:
                    render_desktop();
                    break;
            }
            
            field_update_dynamics(); // Still running scheduler
            vsync_wait();
        }
    } else {
        // Fallback to text mode (current implementation)
        terminal_initialize();
        // ... existing code ...
    }
}
```

---

## Step 6: Building the GUI

### File Structure
```
src/
├── gui/
│   ├── graphics.c      # Framebuffer driver
│   ├── universe.c      # Universe rendering
│   ├── welcome.c       # Welcome screen
│   ├── registration.c  # Observer registration
│   └── desktop.c       # Anchor Universe desktop
└── include/
    └── gui.h           # GUI structures
```

### Build Command
Update `build.bat`:
```batch
gcc -m32 ... -c src/gui/graphics.c -o src/gui/graphics.o
gcc -m32 ... -c src/gui/universe.c -o src/gui/universe.o
...
ld ... src/gui/graphics.o src/gui/universe.o ...
```

---

## Step 7: Testing

### Expected Result
1. QEMU boots
2. Screen clears to deep blue
3. Central universe sphere appears and pulses
4. Text: "Press Any Key to Begin Observation"
5. Pressing a key transitions to next screen

---

## Next Steps (Phase 4.2)

Once basic graphics work:
- ✅ Mouse cursor (Observer pointer)
- ✅ Registration form fields
- ✅ Capability universes orbiting anchor
- ✅ Wave universe visualization (audio)
- ✅ Entanglement visualization (connections between universes)

---

## Key Design Principles

1. **No Windows**: Universes, not rectangles
2. **No Icons**: Energy fields, not static images
3. **No Menus**: Interaction rings, not dropdown lists
4. **Living Motion**: Everything pulses with energy

---

## Math Helpers

### Sine Approximation (for pulsing)
```c
float sinf(float x) {
    // Taylor series approximation
    float x2 = x * x;
    return x - (x * x2) / 6.0f + (x * x2 * x2) / 120.0f;
}
```

### Color Blending (for energy visualization)
```c
uint32_t blend_color(uint32_t c1, uint32_t c2, float t) {
    uint8_t r1 = (c1 >> 16) & 0xFF;
    uint8_t g1 = (c1 >> 8) & 0xFF;
    uint8_t b1 = c1 & 0xFF;
    
    uint8_t r2 = (c2 >> 16) & 0xFF;
    uint8_t g2 = (c2 >> 8) & 0xFF;
    uint8_t b2 = c2 & 0xFF;
    
    uint8_t r = r1 + (r2 - r1) * t;
    uint8_t g = g1 + (g2 - g1) * t;
    uint8_t b = b1 + (b2 - b1) * t;
    
    return (r << 16) | (g << 8) | b;
}
```

---

**Start here**: Implement Step 1-3, get a pulsing blue sphere on screen, then iterate!
