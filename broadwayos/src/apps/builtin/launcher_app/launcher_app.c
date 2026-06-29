#include "wiios_services.h"
#include "../../../ui/widget.h"
#include "../../../gfx/font_bitmap.h"

WiiAppApi filemgr_app_api(void);
WiiAppApi settings_app_api(void);

typedef struct {
  WiiAppContext *ctx;
  wii_u32 selected;
  int launched;
  WiiAppApi filemgr;
  WiiAppApi settings;
  WiiAppApi ext_hello;
  int has_ext_hello;
} LauncherState;

static LauncherState g;

void launcher_app_set_external(const WiiAppApi *api, int available) {
  g.has_ext_hello = available;
  if (available && api) g.ext_hello = *api;
}

static WiiResult launcher_init(WiiAppContext *ctx) {
  g.ctx = ctx;
  g.selected = 0;
  g.launched = -1;
  g.filemgr = filemgr_app_api();
  g.settings = settings_app_api();

  g.filemgr.init(ctx);
  g.settings.init(ctx);

  if (g.has_ext_hello && g.ext_hello.init) g.ext_hello.init(ctx);
  if (ctx && ctx->svc && ctx->svc->log_write) ctx->svc->log_write("launcher init");
  return WIIOS_OK;
}

static WiiResult launcher_handle_event(const WiiEvent *ev) {
  if (!ev) return WIIOS_E_INVAL;

  if (ev->type == EV_ACTION) {
    if (g.launched == -1) {
      /* Tile navigation only meaningful at the launcher level. */
      if (ev->data.act.action == ACT_NEXT) g.selected = (g.selected + 1U) % 3U;
      if (ev->data.act.action == ACT_PREV) g.selected = (g.selected + 2U) % 3U;
      if (ev->data.act.action == ACT_OK) g.launched = (int)g.selected;
    } else {
      /* Inside a sub-app: BACK returns to the launcher; navigation events
       * (NEXT/PREV) are forwarded to the sub-app, not consumed by the launcher. */
      if (ev->data.act.action == ACT_BACK) g.launched = -1;
    }
  }

  if (g.launched == 1) g.filemgr.handle_event(ev);
  if (g.launched == 2) g.settings.handle_event(ev);
  if (g.launched == 0 && g.has_ext_hello && g.ext_hello.handle_event) g.ext_hello.handle_event(ev);
  return WIIOS_OK;
}

static void draw_tiles(WiiSurface *surface) {
  static const char *names[3] = {"HELLO", "FILES", "SETTINGS"};
  for (int i = 0; i < 3; ++i) {
    wii_i32 x = 56 + i * 190;
    wii_u32 border = ((wii_u32)i == g.selected) ? 0xFFE7B550 : 0xFF526C84;
    wii_u32 fill = (i == 0) ? 0xFF2D6E45 : (i == 1) ? 0xFF2A5A7A : 0xFF744A2A;
    widget_draw_panel(surface, (WiiRect){x, 70, 170, 130}, border);
    widget_draw_panel(surface, (WiiRect){x + 4, 74, 162, 122}, fill);
    font_draw_text(surface, x + 24, 130, names[i], 0xFFF4F8FF, 2);
  }
}

static WiiResult launcher_draw(WiiSurface *surface) {
  surface_clear(surface, 0xFF101722);
  widget_draw_panel(surface, (WiiRect){0, 0, (wii_i32)surface->width, 36}, 0xFF1C2C40);
  font_draw_text(surface, 14, 12, "WIIOS LAUNCHER", 0xFFE8F2FF, 2);
  font_draw_text(surface, 410, 12, "A OPEN  B BACK", 0xFFD4E4F7, 1);
  draw_tiles(surface);

  if (g.launched == 1) g.filemgr.draw(surface);
  if (g.launched == 2) g.settings.draw(surface);
  if (g.launched == 0 && g.has_ext_hello && g.ext_hello.draw) g.ext_hello.draw(surface);

  return WIIOS_OK;
}

static void launcher_shutdown(void) {
  g.filemgr.shutdown();
  g.settings.shutdown();
  if (g.has_ext_hello && g.ext_hello.shutdown) g.ext_hello.shutdown();
  if (g.ctx && g.ctx->svc && g.ctx->svc->log_write) g.ctx->svc->log_write("launcher shutdown");
}

WiiAppApi launcher_app_api(void) {
  WiiAppApi api = {
    .init = launcher_init,
    .handle_event = launcher_handle_event,
    .draw = launcher_draw,
    .shutdown = launcher_shutdown,
  };
  return api;
}
