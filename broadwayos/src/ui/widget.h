#ifndef WIIOS_WIDGET_H
#define WIIOS_WIDGET_H

#include "../gfx/surface.h"

typedef struct {
  WiiRect bounds;
} WiiWidget;

void widget_draw_panel(WiiSurface *s, WiiRect r, wii_u32 color);

#endif
