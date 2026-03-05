#include <stdio.h>
#include "../../shared/include/wiios_services.h"
#include "../../shared/include/wiios_ini.h"
#include "gfx/surface.h"
#include "gfx/font_bitmap.h"
#include "core/event_queue.h"
#include "core/boot_config.h"
#include "core/input.h"
#include "core/path_resolver.h"
#include "ui/widget.h"
#include "shell/desktop_shell.h"
#include "services/service_manager.h"
#include "apps/loader/app_loader.h"
#include "os_paths.h"

WiiSurface vi_init_surface(void);
void vi_present(WiiSurface *surface);
void vi_shutdown(void);
void os_log_write(const char *msg);
void ringlog_dump_stdout(void);
WiiAppApi launcher_app_api(void);
void launcher_app_set_external(const WiiAppApi *api, int available);

static WiiResult build_root_path(const char *root, const char *rel, char *out, wii_u32 out_len) {
  int n;
  if (!root || !rel || !out || out_len == 0) return WIIOS_E_INVAL;
  n = snprintf(out, (size_t)out_len, "%s/%s", root, rel);
  if (n < 0 || (wii_u32)n >= out_len) return WIIOS_E_NOMEM;
  return WIIOS_OK;
}

static WiiResult load_external_manifest_with_fallbacks(WiiServices *svc, WiiLoadedApp *out_app) {
  const char *roots[32];
  wii_u32 count;
  WiiResult first_invalid = WIIOS_E_NOENT;
  char manifest_path[160];
  int truncated = 0;

  if (!svc || !out_app) return WIIOS_E_INVAL;

  count = path_resolver_collect_roots(roots, 32, &truncated);
  if (truncated && svc->log_write) svc->log_write("manifest roots truncated");
  for (wii_u32 i = 0; i < count; ++i) {
    WiiResult rc = build_root_path(roots[i], WIIOS_HELLO_MANIFEST_REL, manifest_path, sizeof(manifest_path));
    if (rc != WIIOS_OK) continue;

    rc = app_loader_load_manifest(svc, manifest_path, out_app);
    if (rc == WIIOS_OK) {
      if (svc->log_write) {
        char msg[200];
        (void)snprintf(msg, sizeof(msg), "manifest loaded: %s", manifest_path);
        svc->log_write(msg);
      }
      return WIIOS_OK;
    }
    if (rc == WIIOS_E_INVAL && first_invalid == WIIOS_E_NOENT) first_invalid = WIIOS_E_INVAL;
  }

  return first_invalid;
}

static void draw_status_overlay(WiiSurface *surface, const char *msg) {
  if (!surface || !msg || !msg[0]) return;
  widget_draw_panel(surface, (WiiRect){0, (wii_i32)surface->height - 26, (wii_i32)surface->width, 26}, 0xFF3A2E1F);
  font_draw_text(surface, 10, (wii_i32)surface->height - 18, msg, 0xFFFFE8C9, 1);
}

static void run_storage_error_screen(WiiSurface *surface) {
  WiiEvent ev;
  int running = 1;

  event_queue_init();
  input_init();
  while (running) {
    input_poll_actions();
    while (event_queue_pop(&ev) == WIIOS_OK) {
      if (ev.type == EV_ACTION && ev.data.act.action == ACT_HOME) running = 0;
    }
    surface_clear(surface, 0xFF1B0F0F);
    widget_draw_panel(surface, (WiiRect){40, 90, 560, 160}, 0xFF482020);
    font_draw_text(surface, 66, 120, "STORAGE ERROR", 0xFFFFD0D0, 2);
    font_draw_text(surface, 66, 160, "CHECK SD OR USB DEVICE", 0xFFFFE4CE, 1);
    font_draw_text(surface, 66, 178, "PRESS HOME TO EXIT", 0xFFFFE4CE, 1);
    vi_present(surface);
  }
  input_shutdown();
}

int main(void) {
  wii_u32 last_ms;
  wii_u32 now_ms;
  wii_u32 dt_ms;
  WiiEvent ev;
  WiiServices *svc;
  WiiAppContext ctx;
  WiiSurface surface = vi_init_surface();
  const char *startup_status = 0;
  const WiiShell *shell = 0;
  WiiLoadedApp ext_app;
  WiiResult load_rc;
  char startup_status_buf[128];
  int ext_active = 0;
  int launcher_mode = 1;
  WiiResult boot_rc = WIIOS_E_NOENT;
  const char *boot_root = 0;
  WiiAppApi launcher = {0};
  int running = 1;

  if (service_manager_init() != WIIOS_OK) {
    os_log_write("service manager init failed");
    run_storage_error_screen(&surface);
    vi_shutdown();
    return 1;
  }
  svc = service_manager_services();
  ctx = (WiiAppContext){ .abi_version = 1, .svc = svc, .os_reserved = 0 };
  (void)path_resolver_init(svc);
  startup_status_buf[0] = '\0';
  if (path_resolver_config_missing()) startup_status = "CONFIG MISSING - USING DEFAULTS";

  svc->log_write("WiiOS v0.1.0-dev boot");
  event_queue_init();

  load_rc = load_external_manifest_with_fallbacks(svc, &ext_app);
  if (load_rc == WIIOS_OK) {
    ext_active = 1;
    svc->log_write("external app loaded: hello");
  } else {
    svc->log_write("external app load skipped");
    startup_status = (load_rc == WIIOS_E_NOENT) ? "APP MANIFEST MISSING" : "APP MANIFEST INVALID";
  }

  boot_rc = boot_config_load_mode(svc, &launcher_mode, &boot_root);
  if (boot_rc == WIIOS_E_NOENT) startup_status = "CONFIG MISSING - USING DEFAULTS";
  if (boot_rc == WIIOS_E_INVAL) startup_status = "CONFIG INVALID - USING DEFAULTS";
  if (boot_rc != WIIOS_OK && boot_rc != WIIOS_E_NOENT && boot_rc != WIIOS_E_INVAL) startup_status = "STORAGE ERROR";
  if (boot_rc == WIIOS_OK && boot_root && svc->log_write) {
    char msg[96];
    (void)snprintf(msg, sizeof(msg), "boot config root: %s", boot_root);
    svc->log_write(msg);
  }

  if (launcher_mode) {
    launcher = launcher_app_api();
    launcher_app_set_external(ext_active ? &ext_app.api : 0, ext_active);
    launcher.init(&ctx);
    svc->log_write("boot mode: launcher");
  } else {
    desktop_shell_set_context(&ctx);
    shell = desktop_shell_get();
    shell->init();
    if (ext_active) ext_app.api.init(&ctx);
    svc->log_write("boot mode: desktop");
  }

  input_init();

  last_ms = svc->time_ms();
  while (running) {
    input_poll_actions();
    while (event_queue_pop(&ev) == WIIOS_OK) {
      if (ev.type == EV_ACTION && ev.data.act.action == ACT_HOME) {
        running = 0;
        break;
      }
      if (launcher_mode) {
        launcher.handle_event(&ev);
      } else {
        if (ev.type == EV_ACTION && ev.data.act.action == ACT_BACK) {
          char logmsg[128];
          const char *save_root = 0;
          WiiResult rc = boot_config_save_mode(svc, 1, &save_root);
          const char *shown_root = save_root ? save_root : path_resolver_root();
          (void)snprintf(startup_status_buf, sizeof(startup_status_buf),
              rc == WIIOS_OK ? "BOOT->LAUNCHER SAVED rc=%d %s" : "SAVE ERROR rc=%d %s",
              (int)rc, shown_root);
          startup_status = startup_status_buf;
          if (svc->log_write) {
            (void)snprintf(logmsg, sizeof(logmsg),
                "desktop save %s rc=%d root=%s",
                rc == WIIOS_OK ? "ok" : "fail",
                (int)rc,
                shown_root);
            svc->log_write(logmsg);
          }
          continue;
        }
        if (shell) shell->handle_event(&ev);
        if (ext_active) ext_app.api.handle_event(&ev);
      }
    }
    now_ms = svc->time_ms();
    dt_ms = now_ms - last_ms;
    last_ms = now_ms;
    if (launcher_mode) {
      (void)dt_ms;
      launcher.draw(&surface);
    } else {
      if (shell) {
        shell->update(dt_ms);
        shell->draw(&surface);
      }
      if (ext_active) ext_app.api.draw(&surface);
    }
    draw_status_overlay(&surface, startup_status);
    vi_present(&surface);
  }

  if (launcher_mode) {
    launcher.shutdown();
  } else {
    if (ext_active) ext_app.api.shutdown();
    if (shell) shell->shutdown();
  }
  svc->log_write("WiiOS v0.1.0-dev shutdown");
  input_shutdown();
  service_manager_shutdown();

  ringlog_dump_stdout();
  vi_shutdown();
  puts("main loop complete");
  return 0;
}
