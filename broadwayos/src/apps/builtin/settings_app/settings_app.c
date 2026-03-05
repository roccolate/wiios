#include "wiios_services.h"
#include "../../../ui/widget.h"
#include "../../../gfx/font_bitmap.h"
#include "../../../core/path_resolver.h"
#include "wiios_ini.h"
#include "os_paths.h"
#include <stdio.h>
#include <string.h>

static WiiAppContext *g_ctx;
static int g_boot_to_launcher;
static WiiResult g_last_save_rc;

static int parse_boot_value(const char *value) {
  if (!value || !value[0]) return 1;
  if (value[0] == 'd' || value[0] == 'D') return 0;
  return 1;
}

static void load_boot_mode_from_config(void) {
  char config_path[128];
  char boot_to[32];
  void *buf;
  wii_u32 len;
  WiiResult rc;

  g_boot_to_launcher = 1;
  if (!g_ctx || !g_ctx->svc || !g_ctx->svc->fs_read_all || !g_ctx->svc->fs_free) return;
  if (path_resolver_join(WIIOS_CONFIG_REL, config_path, sizeof(config_path)) != WIIOS_OK) return;

  rc = g_ctx->svc->fs_read_all(config_path, &buf, &len);
  if (rc != WIIOS_OK) {
    if (g_ctx->svc->log_write) g_ctx->svc->log_write("settings: config missing, default launcher");
    return;
  }
  (void)len;

  rc = ini_get_value((const char *)buf, "boot", "boot_to", boot_to, sizeof(boot_to));
  g_ctx->svc->fs_free(buf);
  if (rc != WIIOS_OK) {
    if (g_ctx->svc->log_write) g_ctx->svc->log_write("settings: boot_to invalid, default launcher");
    return;
  }

  if (!(boot_to[0] == 'l' || boot_to[0] == 'L' || boot_to[0] == 'd' || boot_to[0] == 'D')) {
    if (g_ctx->svc->log_write) g_ctx->svc->log_write("settings: unknown boot_to value, default launcher");
  }
  g_boot_to_launcher = parse_boot_value(boot_to);
}

static WiiResult save_boot_mode_to_config(void) {
  char config_path[128];
  char tmp_path[128];
  char root_path[64];
  char body[160];
  const char *boot_mode = g_boot_to_launcher ? "launcher" : "desktop";
  WiiResult rc;
  int n;

  if (!g_ctx || !g_ctx->svc) return WIIOS_E_FAIL;
  if (!g_ctx->svc->fs_write_all || !g_ctx->svc->fs_rename || !g_ctx->svc->fs_mkdirs) return WIIOS_E_FAIL;
  if (path_resolver_join(WIIOS_CONFIG_REL, config_path, sizeof(config_path)) != WIIOS_OK) return WIIOS_E_FAIL;
  if (path_resolver_join(WIIOS_CONFIG_TMP_REL, tmp_path, sizeof(tmp_path)) != WIIOS_OK) return WIIOS_E_FAIL;
  if (path_resolver_join("", root_path, sizeof(root_path)) != WIIOS_OK) return WIIOS_E_FAIL;

  rc = g_ctx->svc->fs_mkdirs(root_path);
  if (rc != WIIOS_OK) return rc;

  n = snprintf(body, sizeof(body),
      "[boot]\n"
      "boot_to=%s\n"
      "\n"
      "[system]\n"
      "language=es\n"
      "theme=default\n"
      "schema=1\n",
      boot_mode);
  if (n <= 0 || n >= (int)sizeof(body)) return WIIOS_E_NOMEM;

  rc = g_ctx->svc->fs_write_all(tmp_path, body, (wii_u32)n);
  if (rc != WIIOS_OK) return rc;
  return g_ctx->svc->fs_rename(tmp_path, config_path);
}

static WiiResult settings_init(WiiAppContext *ctx) {
  g_ctx = ctx;
  g_last_save_rc = WIIOS_OK;
  load_boot_mode_from_config();
  if (g_ctx && g_ctx->svc && g_ctx->svc->log_write) g_ctx->svc->log_write("settings init");
  return WIIOS_OK;
}

static WiiResult settings_handle_event(const WiiEvent *ev) {
  if (ev && ev->type == EV_ACTION && ev->data.act.action == ACT_OK) {
    g_boot_to_launcher = !g_boot_to_launcher;
    g_last_save_rc = save_boot_mode_to_config();
    if (g_ctx && g_ctx->svc && g_ctx->svc->log_write) {
      g_ctx->svc->log_write(g_last_save_rc == WIIOS_OK ? "settings: config saved" : "settings: config save failed");
    }
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
  font_draw_text(surface, 144, 258, g_last_save_rc == WIIOS_OK ? "SAVE: OK" : "SAVE: ERROR", 0xFFD8B9A0, 1);
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
