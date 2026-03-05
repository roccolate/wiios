#ifndef WIIOS_WINDOW_MANAGER_H
#define WIIOS_WINDOW_MANAGER_H

#include "window.h"

#define WIIOS_MAX_WINDOWS 8

typedef struct {
  WiiWindow windows[WIIOS_MAX_WINDOWS];
  wii_u32 count;
  wii_i32 focused_idx;
} WiiWindowManager;

void wm_init(WiiWindowManager *wm);
WiiResult wm_add_window(WiiWindowManager *wm, const WiiWindow *window);
void wm_draw(const WiiWindowManager *wm, WiiSurface *surface);
void wm_handle_event(WiiWindowManager *wm, const WiiEvent *ev);
void wm_focus_next(WiiWindowManager *wm);
void wm_focus_prev(WiiWindowManager *wm);

#endif
