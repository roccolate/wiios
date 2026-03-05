#include "widget.h"

void widget_draw_panel(WiiSurface *s, WiiRect r, wii_u32 color) {
  surface_fill_rect(s, r, color);
}
