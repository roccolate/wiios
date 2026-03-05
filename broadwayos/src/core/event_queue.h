#ifndef WIIOS_EVENT_QUEUE_H
#define WIIOS_EVENT_QUEUE_H

#include "../../../shared/include/wiios_events.h"

void event_queue_init(void);
WiiResult event_queue_push(WiiEvent ev);
WiiResult event_queue_pop(WiiEvent *out);

#endif
