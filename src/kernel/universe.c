#include "../include/universe.h"
#include "../include/graphics.h"

static universe_t anchor;

/* Fast sine approximation using Taylor series */
float universe_sin(float x) {
    // Normalize to [-PI, PI]
    const float PI = 3.14159265f;
    while (x > PI) x -= 2 * PI;
    while (x < -PI) x += 2 * PI;
    
    // Taylor series: sin(x) ≈ x - x³/6 + x⁵/120
    float x2 = x * x;
    float x3 = x2 * x;
    float x5 = x3 * x2;
    return x - (x3 / 6.0f) + (x5 / 120.0f);
}

float universe_cos(float x) {
    const float PI = 3.14159265f;
    return universe_sin(x + PI / 2.0f);
}

void universe_init(universe_t* u, float x, float y, float radius, color_t color) {
    u->x = x;
    u->y = y;
    u->radius = radius;
    u->energy = 0.0f;
    u->formation = 0.0f;
    u->state = UNIVERSE_STATE_FORMING;
    u->primary_color = color;
    u->energy_color = COLOR_ENERGY_CYAN;
}

void universe_update(universe_t* u, float delta_time) {
    switch (u->state) {
        case UNIVERSE_STATE_FORMING:
            u->formation += delta_time * 0.5f;
            if (u->formation >= 1.0f) {
                u->formation = 1.0f;
                u->state = UNIVERSE_STATE_STABLE;
            }
            break;
            
        case UNIVERSE_STATE_STABLE:
            // Transition to pulsing after stabilization
            u->state = UNIVERSE_STATE_PULSING;
            break;
            
        case UNIVERSE_STATE_PULSING:
            u->energy += delta_time;
            break;
            
        case UNIVERSE_STATE_COLLAPSING:
            u->formation -= delta_time * 2.0f;
            if (u->formation <= 0.0f) {
                u->formation = 0.0f;
            }
            break;
    }
}

void universe_render(universe_t* u) {
    if (u->formation <= 0.0f) return;
    
    // Calculate current radius with pulsing effect
    float pulse = 1.0f;
    if (u->state == UNIVERSE_STATE_PULSING) {
        pulse = 1.0f + 0.1f * universe_sin(u->energy);
    }
    
    float current_radius = u->radius * u->formation * pulse;
    
    // Draw concentric energy rings
    const int num_rings = 5;
    for (int i = 0; i < num_rings; i++) {
        float ring_radius = current_radius + (i * 15.0f);
        float alpha = 1.0f - ((float)i / num_rings);
        
        // Blend between primary and energy colors based on pulse
        float energy_intensity = (universe_sin(u->energy + i * 0.5f) + 1.0f) * 0.5f;
        color_t ring_color = graphics_blend_color(u->primary_color, u->energy_color, energy_intensity * 0.3f);
        
        // Adjust alpha
        uint8_t a = (uint8_t)(alpha * 255.0f);
        ring_color = (a << 24) | (ring_color & 0x00FFFFFF);
        
        graphics_draw_circle((uint32_t)u->x, (uint32_t)u->y, (uint32_t)ring_radius, ring_color);
    }
    
    // Draw central core (solid)
    graphics_fill_circle((uint32_t)u->x, (uint32_t)u->y, (uint32_t)(current_radius * 0.6f), u->primary_color);
    
    // Draw bright center point (Observer focus)
    graphics_fill_circle((uint32_t)u->x, (uint32_t)u->y, (uint32_t)(current_radius * 0.2f), COLOR_TEXT_WHITE);
}

/* Anchor Universe (Desktop) Implementation */
void anchor_universe_init(void) {
    uint32_t cx = graphics_get_width() / 2;
    uint32_t cy = graphics_get_height() / 2;
    
    universe_init(&anchor, (float)cx, (float)cy, 60.0f, COLOR_UNIVERSE_BLUE);
}

void anchor_universe_update(float delta_time) {
    universe_update(&anchor, delta_time);
}

void anchor_universe_render(void) {
    universe_render(&anchor);
}

universe_t* anchor_universe_get(void) {
    return &anchor;
}
