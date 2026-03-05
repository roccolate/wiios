#include "wiios_services.h"
#include "../../../ui/widget.h"
#include "../../../gfx/font_bitmap.h"

static WiiAppContext *g_ctx;

static WiiResult hello_init(WiiAppContext *ctx) {
  g_ctx = ctx;
  if (g_ctx && g_ctx->svc && g_ctx->svc->log_write) g_ctx->svc->log_write("hello app init");
  return WIIOS_OK;
}

static WiiResult hello_handle_event(const WiiEvent *ev) {
  (void)ev;
  return WIIOS_OK;
}

static WiiResult hello_draw(WiiSurface *surface) {
  widget_draw_panel(surface, (WiiRect){460, 24, 172, 64}, 0xFF2D6E45);
  widget_draw_panel(surface, (WiiRect){466, 30, 160, 52}, 0xFF3C8E59);
  font_draw_text(surface, 486, 50, "HELLO APP", 0xFFE8FFF0, 1);
  return WIIOS_OK;
}

static void hello_shutdown(void) {
  if (g_ctx && g_ctx->svc && g_ctx->svc->log_write) g_ctx->svc->log_write("hello app shutdown");
}

WiiAppApi hello_app_api(void) {
  WiiAppApi api = {
    .init = hello_init,
    .handle_event = hello_handle_event,
    .draw = hello_draw,
    .shutdown = hello_shutdown,
  };
  return api;
}
