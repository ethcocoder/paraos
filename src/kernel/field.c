#include "../include/field.h"

#define MAX_FIELDS 32

static cognitive_field_t fields[MAX_FIELDS];
static uint32_t active_fields_count = 0;

/* External output function from kernel.c (temporary for debugging) */
extern void terminal_writestring(const char* data);

void init_cognitive_fields(void) 
{
    terminal_writestring("[ FIELD ] Quantizing Field Space... ");
    for(int i=0; i<MAX_FIELDS; i++) {
        fields[i].id = 0;
        fields[i].state = FIELD_STATE_DORMANT;
        fields[i].energy = 0;
    }
    active_fields_count = 0;
    terminal_writestring("DONE.\n");
}

void create_excitation(const char* name, void (*function)(void), uint32_t initial_energy)
{
    if (active_fields_count >= MAX_FIELDS) return;
    
    int index = active_fields_count++;
    fields[index].id = index + 1;
    fields[index].energy = initial_energy;
    fields[index].state = FIELD_STATE_SUPERPOSITION;
    fields[index].entry_point = function;
    
    /* Simple string copy */
    int i = 0;
    while(name[i] && i < 31) {
        fields[index].name[i] = name[i];
        i++;
    }
    fields[index].name[i] = 0;
    
    terminal_writestring("[ FIELD ] New Excitation Created: ");
    terminal_writestring(name);
    terminal_writestring("\n");
}

/* The "Quantum Scheduler" 
   Instead of round-robin, we pick the field with highest ENERGY.
   This simulates the collapse of the wavefunction to the most probable (energetic) state. */
void field_update_dynamics(void)
{
    /* In a real implementation, this would switch stacks. 
       For now, we just simulate the selection logic. */
       
    uint32_t max_energy = 0;
    int selected_index = -1;
    
    for(uint32_t i=0; i<active_fields_count; i++) {
        if (fields[i].state == FIELD_STATE_SUPERPOSITION || fields[i].state == FIELD_STATE_COLLAPSED) {
            if (fields[i].energy > max_energy) {
                max_energy = fields[i].energy;
                selected_index = i;
            }
        }
    }
    
    if (selected_index != -1) {
        /* Collapse/Run the selected field */
        fields[selected_index].state = FIELD_STATE_COLLAPSED;
        
        terminal_writestring("[ SCHEDULER ] Collapsing Field: ");
        terminal_writestring(fields[selected_index].name);
        terminal_writestring("\n");
        
        /* execute the cognitive function */
        if (fields[selected_index].entry_point) {
            fields[selected_index].entry_point();
        }
        
        /* Decay energy (Entropy increases, useful energy dissipates) */
        if (fields[selected_index].energy > 0)
            fields[selected_index].energy--;
            
        /* Here we would perform the context switch */
    }
}
