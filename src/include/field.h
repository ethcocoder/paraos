#ifndef FIELD_H
#define FIELD_H

#include <stdint.h>

/* Cognitive Field States */
typedef enum {
    FIELD_STATE_DORMANT,
    FIELD_STATE_SUPERPOSITION, /* Ready/Waiting */
    FIELD_STATE_COLLAPSED,     /* Running */
    FIELD_STATE_ENTANGLED      /* Blocked/Waiting on other field */
} field_state_t;

/* Cognitive Field Structure 
   Represents a unit of computation as an energy field. */
typedef struct {
    uint32_t id;
    uint32_t energy;       /* Priority/Resource coupling */
    uint32_t entropy;      /* Curiosity/Uncertainty metric */
    field_state_t state;
    char name[32];
    
    /* Context/Stack Pointers would go here */
    void (*entry_point)(void);
} cognitive_field_t;

/* Global System Dynamics */
void init_cognitive_fields(void);
void field_update_dynamics(void); /* The "Scheduler" */
void create_excitation(const char* name, void (*function)(void), uint32_t initial_energy);

#endif
