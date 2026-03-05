#include "window_manager.h"
#include "../ui/widget.h"
#include "../gfx/font_bitmap.h"

static void wm_frame_window(const WiiWindow *w, WiiSurface *surface, int focused) {
  wii_u32 border = focused ? 0xFFE8B43C : 0xFF4C5D73;
  wii_u32 title = focused ? 0xFF2E3C4F : 0xFF243140;

  widget_draw_panel(surface, w->bounds, border);
  widget_draw_panel(surface, (WiiRect){w->bounds.x + 2, w->bounds.y + 2, w->bounds.w - 4, w->bounds.h - 4}, 0xFF111822);
  widget_draw_panel(surface, (WiiRect){w->bounds.x + 2, w->bounds.y + 2, w->bounds.w - 4, 18}, title);
  if (w->title) font_draw_text(surface, w->bounds.x + 8, w->bounds.y + 6, w->title, 0xFFEAF2FF, 1);
}

void wm_init(WiiWindowManager *wm) {
  wm->count = 0;
  wm->focused_idx = -1;
}

WiiResult wm_add_window(WiiWindowManager *wm, const WiiWindow *window) {
  if (!wm || !window) return WIIOS_E_INVAL;
  if (wm->count >= WIIOS_MAX_WINDOWS) return WIIOS_E_NOMEM;
  wm->windows[wm->count] = *window;
  if (wm->focused_idx < 0) wm->focused_idx = 0;
  wm->count++;
  return WIIOS_OK;
}

void wm_focus_next(WiiWindowManager *wm) {
  if (!wm || wm->count == 0) return;
  wm->focused_idx = (wm->focused_idx + 1) % (wii_i32)wm->count;
}

void wm_focus_prev(WiiWindowManager *wm) {
  if (!wm || wm->count == 0) return;
  wm->focused_idx = (wm->focused_idx - 1);
  if (wm->focused_idx < 0) wm->focused_idx = (wii_i32)wm->count - 1;
}

void wm_handle_event(WiiWindowManager *wm, const WiiEvent *ev) {
  if (!wm || !ev) return;
  if (ev->type == EV_ACTION) {
    if (ev->data.act.action == ACT_NEXT) wm_focus_next(wm);
    if (ev->data.act.action == ACT_PREV) wm_focus_prev(wm);
  }

  if (wm->focused_idx >= 0 && (wii_u32)wm->focused_idx < wm->count) {
    WiiWindow *focused = &wm->windows[wm->focused_idx];
    if (focused->on_event) focused->on_event(ev, focused->user);
  }
}

void wm_draw(const WiiWindowManager *wm, WiiSurface *surface) {
  if (!wm || !surface) return;
  for (wii_u32 i = 0; i < wm->count; ++i) {
    const WiiWindow *w = &wm->windows[i];
    if (!w->visible) continue;
    wm_frame_window(w, surface, (wii_i32)i == wm->focused_idx);
    if (w->on_draw) w->on_draw(surface, &w->bounds, (wii_i32)i == wm->focused_idx, w->user);
  }
}
