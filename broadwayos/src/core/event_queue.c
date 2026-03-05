#include "event_queue.h"

#define QCAP 64
static WiiEvent q[QCAP];
static wii_u32 head;
static wii_u32 tail;
static wii_u32 size;

void event_queue_init(void) { head = tail = size = 0; }

WiiResult event_queue_push(WiiEvent ev) {
  if (size == QCAP) return WIIOS_E_NOMEM;
  q[tail] = ev;
  tail = (tail + 1U) % QCAP;
  ++size;
  return WIIOS_OK;
}

WiiResult event_queue_pop(WiiEvent *out) {
  if (size == 0) return WIIOS_E_NOENT;
  *out = q[head];
  head = (head + 1U) % QCAP;
  --size;
  return WIIOS_OK;
}
