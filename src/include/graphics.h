#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

/* Graphics context */
typedef struct {
    uint32_t* framebuffer;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t bpp;
    uint8_t initialized;
} graphics_context_t;

/* Color type (ARGB) */
typedef uint32_t color_t;

/* Predefined colors (ParadoxOS theme) */
#define COLOR_SPACE_DEEP     0xFF0a0a1e
#define COLOR_SPACE_DARK     0xFF1a1a3e
#define COLOR_UNIVERSE_BLUE  0xFF4a9eff
#define COLOR_ENERGY_CYAN    0xFF00ffff
#define COLOR_FIELD_PURPLE   0xFF9d4edd
#define COLOR_TEXT_WHITE     0xFFffffff
#define COLOR_TEXT_GRAY      0xFFaaaaaa
#define COLOR_TRANSPARENT    0x00000000

/* Graphics initialization */
void graphics_init(uint32_t addr, uint32_t width, uint32_t height, uint32_t pitch, uint8_t bpp);
uint8_t graphics_is_available(void);

/* Drawing primitives */
void graphics_clear(color_t color);
void graphics_put_pixel(uint32_t x, uint32_t y, color_t color);
void graphics_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, color_t color);
void graphics_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, color_t color);
void graphics_draw_circle(uint32_t cx, uint32_t cy, uint32_t radius, color_t color);
void graphics_fill_circle(uint32_t cx, uint32_t cy, uint32_t radius, color_t color);
void graphics_draw_line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, color_t color);

/* Text rendering (simple bitmap font) */
void graphics_draw_char(uint32_t x, uint32_t y, char c, color_t color);
void graphics_draw_string(uint32_t x, uint32_t y, const char* str, color_t color);
void graphics_draw_string_centered(uint32_t y, const char* str, color_t color);

/* Utility */
color_t graphics_blend_color(color_t c1, color_t c2, float t);
uint32_t graphics_get_width(void);
uint32_t graphics_get_height(void);

#endif
