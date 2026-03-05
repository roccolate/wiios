#include "wiios_services.h"
#include <stdio.h>
#include "../../../ui/widget.h"
#include "../../../gfx/font_bitmap.h"
#include "../../../core/boot_config.h"
#include "../../../core/path_resolver.h"

static WiiAppContext *g_ctx;
static int g_boot_to_launcher;
static WiiResult g_last_save_rc;
static const char *g_last_io_root;

static void load_boot_mode_from_config(void) {
  int mode = 1;
  const char *loaded_root = 0;
  g_boot_to_launcher = 1;
  if (!g_ctx || !g_ctx->svc) return;
  if (boot_config_load_mode(g_ctx->svc, &mode, &loaded_root) == WIIOS_OK) {
    g_boot_to_launcher = mode ? 1 : 0;
    if (loaded_root) g_last_io_root = loaded_root;
  }
}

static WiiResult save_boot_mode_to_config(void) {
  if (!g_ctx || !g_ctx->svc) return WIIOS_E_FAIL;
  return boot_config_save_mode(g_ctx->svc, g_boot_to_launcher ? 1 : 0, &g_last_io_root);
}

static WiiResult settings_init(WiiAppContext *ctx) {
  g_ctx = ctx;
  g_last_save_rc = WIIOS_OK;
  g_last_io_root = path_resolver_root();
  load_boot_mode_from_config();
  if (g_ctx && g_ctx->svc && g_ctx->svc->log_write) g_ctx->svc->log_write("settings init");
  return WIIOS_OK;
}

static WiiResult settings_handle_event(const WiiEvent *ev) {
  if (ev && ev->type == EV_ACTION && ev->data.act.action == ACT_OK) {
    char msg[128];
    g_boot_to_launcher = !g_boot_to_launcher;
    g_last_save_rc = save_boot_mode_to_config();
    if (g_ctx && g_ctx->svc && g_ctx->svc->log_write) {
      (void)snprintf(msg, sizeof(msg),
          "settings: %s rc=%d root=%s",
          g_last_save_rc == WIIOS_OK ? "config saved" : "config save failed",
          (int)g_last_save_rc,
          g_last_io_root ? g_last_io_root : path_resolver_root());
      g_ctx->svc->log_write(msg);
    }
  }
  return WIIOS_OK;
}

static WiiResult settings_draw(WiiSurface *surface) {
  char save_line[48];
  char root_line[96];
  wii_u32 accent = g_boot_to_launcher ? 0xFF4B9D4F : 0xFF9D4B4B;
  widget_draw_panel(surface, (WiiRect){120, 100, 400, 260}, 0xFF362218);
  widget_draw_panel(surface, (WiiRect){128, 108, 384, 40}, accent);
  widget_draw_panel(surface, (WiiRect){128, 156, 384, 196}, 0xFF1F140E);
  (void)snprintf(save_line, sizeof(save_line), "SAVE RC: %d", (int)g_last_save_rc);
  (void)snprintf(root_line, sizeof(root_line), "ROOT: %s", g_last_io_root ? g_last_io_root : path_resolver_root());
  font_draw_text(surface, 144, 122, "SETTINGS", 0xFFFFF3E8, 1);
  font_draw_text(surface, 144, 178, "BOOT TO LAUNCHER", 0xFFECD3BE, 1);
  font_draw_text(surface, 144, 198, g_boot_to_launcher ? "ENABLED" : "DISABLED", 0xFFFFE4CC, 2);
  font_draw_text(surface, 144, 238, "PRESS A TO TOGGLE", 0xFFD8B9A0, 1);
  font_draw_text(surface, 144, 258, save_line, 0xFFD8B9A0, 1);
  font_draw_text(surface, 144, 278, root_line, 0xFFD8B9A0, 1);
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
