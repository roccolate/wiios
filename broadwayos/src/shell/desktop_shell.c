#include "desktop_shell.h"
#include "../wm/window_manager.h"
#include "../ui/widget.h"
#include "../gfx/font_bitmap.h"

static WiiAppContext *g_ctx;
static WiiWindowManager g_wm;

typedef struct {
  wii_u32 pulses;
} DemoState;

static DemoState g_log_state;
static DemoState g_sys_state;

static void draw_log_window(WiiSurface *s, const WiiRect *b, int focused, void *user) {
  (void)user;
  widget_draw_panel(s, (WiiRect){b->x + 8, b->y + 28, b->w - 16, b->h - 36}, 0xFF1B2735);
  widget_draw_panel(s, (WiiRect){b->x + 12, b->y + 32, b->w - 24, 20}, focused ? 0xFF305277 : 0xFF243B53);
  widget_draw_panel(s, (WiiRect){b->x + 12, b->y + 58, b->w - 24, 12}, 0xFF27405A);
  widget_draw_panel(s, (WiiRect){b->x + 12, b->y + 74, b->w - 40, 12}, 0xFF27405A);
}

static void draw_system_window(WiiSurface *s, const WiiRect *b, int focused, void *user) {
  DemoState *state = (DemoState *)user;
  wii_i32 w = (wii_i32)((state->pulses % (wii_u32)(b->w - 24)) + 12);
  widget_draw_panel(s, (WiiRect){b->x + 8, b->y + 28, b->w - 16, b->h - 36}, 0xFF2A1E18);
  widget_draw_panel(s, (WiiRect){b->x + 12, b->y + 34, b->w - 24, 14}, focused ? 0xFF8E5A2E : 0xFF6C4323);
  widget_draw_panel(s, (WiiRect){b->x + 12, b->y + 54, w, 18}, 0xFFB2733B);
}

static void handle_system_event(const WiiEvent *ev, void *user) {
  DemoState *state = (DemoState *)user;
  if (ev->type == EV_ACTION && ev->data.act.action == ACT_OK) state->pulses += 24;
}

static void shell_init(void) {
  wm_init(&g_wm);
  g_log_state.pulses = 0;
  g_sys_state.pulses = 48;

  wm_add_window(&g_wm, &(WiiWindow){
    .id = 1,
    .title = "Log Viewer",
    .bounds = {28, 32, 380, 230},
    .visible = 1,
    .on_draw = draw_log_window,
    .on_event = 0,
    .user = &g_log_state,
  });

  wm_add_window(&g_wm, &(WiiWindow){
    .id = 2,
    .title = "System Status",
    .bounds = {230, 160, 360, 260},
    .visible = 1,
    .on_draw = draw_system_window,
    .on_event = handle_system_event,
    .user = &g_sys_state,
  });

  if (g_ctx && g_ctx->svc && g_ctx->svc->log_write) g_ctx->svc->log_write("desktop_shell init (2 windows)");
}

static void shell_handle_event(const WiiEvent *ev) {
  wm_handle_event(&g_wm, ev);
}

static void shell_update(wii_u32 dt_ms) {
  (void)dt_ms;
  g_sys_state.pulses += 2;
}

static void shell_draw(WiiSurface *s) {
  surface_clear(s, 0xFF0B1118);
  widget_draw_panel(s, (WiiRect){0, 0, (wii_i32)s->width, 24}, 0xFF141E2A);
  font_draw_text(s, 8, 8, "WIIOS DESKTOP", 0xFFDCE8F6, 1);
  font_draw_text(s, 355, 8, "LEFT RIGHT FOCUS  A ACTION  B->LAUNCHER", 0xFFB5C9DE, 1);
  wm_draw(&g_wm, s);
}

static void shell_shutdown(void) {
  if (g_ctx && g_ctx->svc && g_ctx->svc->log_write) g_ctx->svc->log_write("desktop_shell shutdown");
}

void desktop_shell_set_context(WiiAppContext *ctx) {
  g_ctx = ctx;
}

const WiiShell *desktop_shell_get(void) {
  static WiiShell shell = {
    .init = shell_init,
    .handle_event = shell_handle_event,
    .update = shell_update,
    .draw = shell_draw,
    .shutdown = shell_shutdown,
  };
  return &shell;
}
