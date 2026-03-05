#ifndef WIIOS_INPUT_H
#define WIIOS_INPUT_H

#include "../../../shared/include/wiios_events.h"

void input_init(void);
void input_shutdown(void);
void input_poll_actions(void);

wii_u32 input_get_buttons(void);
WiiVec2 input_get_pointer(void);

#endif
