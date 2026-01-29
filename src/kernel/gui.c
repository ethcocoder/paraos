#include "../include/gui.h"
#include "../include/graphics.h"
#include "../include/universe.h"

static gui_state_t current_state = GUI_STATE_WELCOME;
static float time_elapsed = 0.0f;

void gui_init(void) {
    current_state = GUI_STATE_WELCOME;
    time_elapsed = 0.0f;
    anchor_universe_init();
}

void gui_update(float delta_time) {
    time_elapsed += delta_time;
    
    switch (current_state) {
        case GUI_STATE_WELCOME:
        case GUI_STATE_DESKTOP:
            anchor_universe_update(delta_time);
            break;
default:
            break;
    }
}

void gui_render(void) {
    switch (current_state) {
        case GUI_STATE_WELCOME:
            gui_welcome_render();
            break;
        case GUI_STATE_REGISTRATION:
            gui_registration_render();
            break;
        case GUI_STATE_DESKTOP:
            gui_desktop_render();
            break;
        default:
            break;
    }
}

void gui_handle_key(uint8_t scancode) {
    // Only handle key presses (not releases)
    if (scancode & 0x80) return;
    
    switch (current_state) {
        case GUI_STATE_WELCOME:
            // Any key advances to registration
            current_state = GUI_STATE_REGISTRATION;
            break;
            
        case GUI_STATE_REGISTRATION:
            // Space/Enter advances to desktop
            if (scancode == 0x1C || scancode == 0x39) { // Enter or Space
                current_state = GUI_STATE_DESKTOP;
            }
            break;
            
        case GUI_STATE_DESKTOP:
            // ESC returns to welcome
            if (scancode == 0x01) {
                current_state = GUI_STATE_WELCOME;
                time_elapsed = 0.0f;
                anchor_universe_init();
            }
            break;
            
        default:
            break;
    }
}

gui_state_t gui_get_state(void) {
    return current_state;
}

void gui_set_state(gui_state_t state) {
    current_state = state;
}

/* Welcome Screen (Observer Birth) */
void gui_welcome_render(void) {
    // Dark space background
    graphics_clear(COLOR_SPACE_DEEP);
    
    universe_t* anchor = anchor_universe_get();
    
    // Title appears after universe forms
    if (anchor->formation > 0.5f) {
        uint32_t y = graphics_get_height() / 4;
        
        graphics_draw_string_centered(y, "=== PARADOX OS ===", COLOR_TEXT_WHITE);
        graphics_draw_string_centered(y + 30, "A Quantum-Inspired Universal Operating System", COLOR_TEXT_GRAY);
    }
    
    // Render forming universe
    anchor_universe_render();
    
    // Prompt appears when fully formed
    if (anchor->formation >= 1.0f) {
        uint32_t y = graphics_get_height() - 100;
        
        // Pulsing text effect
        float pulse = (universe_sin(time_elapsed * 2.0f) + 1.0f) * 0.5f;
        color_t prompt_color = graphics_blend_color(COLOR_TEXT_GRAY, COLOR_TEXT_WHITE, pulse);
        
        graphics_draw_string_centered(y, "You are about to enter a living system.", prompt_color);
        graphics_draw_string_centered(y + 20, "Press any key to begin observation", prompt_color);
    }
}

/* Registration Screen (Observer Identity) */
void gui_registration_render(void) {
    graphics_clear(COLOR_SPACE_DARK);
    
    uint32_t cx = graphics_get_width() / 2;
    uint32_t y = 100;
    
    // Title
    graphics_draw_string_centered(y, "=== OBSERVER REGISTRATION ===", COLOR_TEXT_WHITE);
    y += 40;
    
    // Form (simplified for now)
    graphics_draw_string_centered(y, "Observer Name: [ANONYMOUS]", COLOR_TEXT_GRAY);
    y += 30;
    graphics_draw_string_centered(y, "Knowledge Level: [EXPLORER]", COLOR_TEXT_GRAY);
    y += 30;
    graphics_draw_string_centered(y, "Purpose: [OBSERVATION]", COLOR_TEXT_GRAY);
    y += 80;
    
    // Info text
    graphics_draw_string_centered(y, "This creates your Observer Universe.", COLOR_UNIVERSE_BLUE);
    y += 20;
    graphics_draw_string_centered(y, "Your interactions will shape system state.", COLOR_UNIVERSE_BLUE);
    y += 60;
    
    // Prompt
    float pulse = (universe_sin(time_elapsed * 2.0f) + 1.0f) * 0.5f;
    color_t prompt_color = graphics_blend_color(COLOR_TEXT_GRAY, COLOR_TEXT_WHITE, pulse);
    graphics_draw_string_centered(y, "Press SPACE to continue", prompt_color);
    
    // Small decorative universe
    universe_t deco;
    universe_init(&deco, cx, graphics_get_height() - 150, 40.0f, COLOR_FIELD_PURPLE);
    deco.formation = 1.0f;
    deco.state = UNIVERSE_STATE_PULSING;
    deco.energy = time_elapsed;
    universe_render(&deco);
}

/* Desktop Screen (Anchor Universe) */
void gui_desktop_render(void) {
    graphics_clear(COLOR_SPACE_DEEP);
    
    // Render main anchor universe (desktop)
    anchor_universe_render();
    
    // Status bar at bottom
    uint32_t y = graphics_get_height() - 30;
    graphics_fill_rect(0, y, graphics_get_width(), 30, 0xFF1a1a2e);
    
    graphics_draw_string(10, y + 10, "[ ANCHOR UNIVERSE ]", COLOR_ENERGY_CYAN);
    graphics_draw_string(200, y + 10, "Cognitive Fields: ACTIVE", COLOR_TEXT_GRAY);
    graphics_draw_string(450, y + 10, "Observer: CONNECTED", COLOR_TEXT_GRAY);
    
    // ESC hint
    graphics_draw_string(graphics_get_width() - 200, y + 10, "ESC = Return", COLOR_TEXT_GRAY);
    
    // Top info
    graphics_draw_string(10, 10, "ParadoxOS v0.3.0 - Observer Desktop", COLOR_TEXT_WHITE);
    graphics_draw_string(10, 25, "Press ESC to return to Welcome", COLOR_TEXT_GRAY);
}
