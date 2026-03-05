#ifndef WIIOS_SHELL_IFACE_H
#define WIIOS_SHELL_IFACE_H

#include "wiios_events.h"
#include "../gfx/surface.h"

typedef struct WiiShell {
  void (*init)(void);
  void (*handle_event)(const WiiEvent *ev);
  void (*update)(wii_u32 dt_ms);
  void (*draw)(WiiSurface *s);
  void (*shutdown)(void);
} WiiShell;

#endif
