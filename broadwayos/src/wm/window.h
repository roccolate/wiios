#ifndef WIIOS_WINDOW_H
#define WIIOS_WINDOW_H

#include "wiios_events.h"
#include "../gfx/surface.h"

typedef void (*WiiWindowDrawFn)(WiiSurface *surface, const WiiRect *bounds, int focused, void *user);
typedef void (*WiiWindowEventFn)(const WiiEvent *ev, void *user);

typedef struct {
  wii_u32 id;
  const char *title;
  WiiRect bounds;
  int visible;
  WiiWindowDrawFn on_draw;
  WiiWindowEventFn on_event;
  void *user;
} WiiWindow;

#endif
