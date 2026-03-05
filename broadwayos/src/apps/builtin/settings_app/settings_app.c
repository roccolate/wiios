#include "wiios_services.h"
#include "../../../ui/widget.h"
#include "../../../gfx/font_bitmap.h"

static WiiAppContext *g_ctx;
static int g_boot_to_launcher;

static WiiResult settings_init(WiiAppContext *ctx) {
  g_ctx = ctx;
  g_boot_to_launcher = 1;
  if (g_ctx && g_ctx->svc && g_ctx->svc->log_write) g_ctx->svc->log_write("settings init");
  return WIIOS_OK;
}

static WiiResult settings_handle_event(const WiiEvent *ev) {
  if (ev && ev->type == EV_ACTION && ev->data.act.action == ACT_OK) {
    g_boot_to_launcher = !g_boot_to_launcher;
  }
  return WIIOS_OK;
}

static WiiResult settings_draw(WiiSurface *surface) {
  wii_u32 accent = g_boot_to_launcher ? 0xFF4B9D4F : 0xFF9D4B4B;
  widget_draw_panel(surface, (WiiRect){120, 100, 400, 260}, 0xFF362218);
  widget_draw_panel(surface, (WiiRect){128, 108, 384, 40}, accent);
  widget_draw_panel(surface, (WiiRect){128, 156, 384, 196}, 0xFF1F140E);
  font_draw_text(surface, 144, 122, "SETTINGS", 0xFFFFF3E8, 1);
  font_draw_text(surface, 144, 178, "BOOT TO LAUNCHER", 0xFFECD3BE, 1);
  font_draw_text(surface, 144, 198, g_boot_to_launcher ? "ENABLED" : "DISABLED", 0xFFFFE4CC, 2);
  font_draw_text(surface, 144, 238, "PRESS A TO TOGGLE", 0xFFD8B9A0, 1);
  return WIIOS_OK;
}

static void settings_shutdown(void) {
  if (g_ctx && g_ctx->svc && g_ctx->svc->log_write) g_ctx->svc->log_write("settings shutdown");
}

WiiAppApi settings_app_api(void) {
  WiiAppApi api = {
    .init = settings_init,
    .handle_event = settings_handle_event,
    .draw = settings_draw,
    .shutdown = settings_shutdown,
  };
  return api;
}
