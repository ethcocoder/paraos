#ifndef GUI_H
#define GUI_H

#include <stdint.h>

/* GUI States (Observer Journey) */
typedef enum {
    GUI_STATE_WELCOME,        // Observer Birth
    GUI_STATE_REGISTRATION,   // Observer Identity
    GUI_STATE_SECURITY,       // Trust Initialization
    GUI_STATE_DESKTOP,        // Anchor Universe (normal operation)
    GUI_STATE_CAPABILITY      // Capability activation
} gui_state_t;

/* GUI Module */
void gui_init(void);
void gui_update(float delta_time);
void gui_render(void);
void gui_handle_key(uint8_t scancode);
gui_state_t gui_get_state(void);
void gui_set_state(gui_state_t state);

/* Individual screens */
void gui_welcome_render(void);
void gui_registration_render(void);
void gui_desktop_render(void);

#endif
