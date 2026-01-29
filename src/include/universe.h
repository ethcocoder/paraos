#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <stdint.h>
#include "graphics.h"

/* Universe state */
typedef enum {
    UNIVERSE_STATE_FORMING,
    UNIVERSE_STATE_STABLE,
    UNIVERSE_STATE_PULSING,
    UNIVERSE_STATE_COLLAPSING
} universe_state_t;

/* Universe structure (Anchor Universe = Desktop) */
typedef struct {
    float x, y;              // Center position
    float radius;            // Base radius
    float energy;            // Pulsing energy
    float formation;         // Formation progress (0.0 - 1.0)
    universe_state_t state;
    color_t primary_color;
    color_t energy_color;
} universe_t;

/*Simple sine approximation for pulsing */
float universe_sin(float x);
float universe_cos(float x);

/* Universe management */
void universe_init(universe_t* u, float x, float y, float radius, color_t color);
void universe_update(universe_t* u, float delta_time);
void universe_render(universe_t* u);

/* Anchor Universe (Desktop) */
void anchor_universe_init(void);
void anchor_universe_update(float delta_time);
void anchor_universe_render(void);
universe_t* anchor_universe_get(void);

#endif
