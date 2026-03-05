#include <stdio.h>
#include "../../shared/include/wiios_services.h"
#include "../../shared/include/wiios_ini.h"
#include "gfx/surface.h"
#include "gfx/font_bitmap.h"
#include "core/event_queue.h"
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

static int boot_to_launcher(WiiServices *svc) {
  void *buf;
  wii_u32 len;
  char boot_to[32];
  char config_path[128];
  WiiResult rc;

  if (!svc || !svc->fs_read_all || !svc->fs_free) return 0;
  if (path_resolver_join(WIIOS_CONFIG_REL, config_path, sizeof(config_path)) != WIIOS_OK) return 0;
  rc = svc->fs_read_all(config_path, &buf, &len);
  if (rc != WIIOS_OK) {
    if (svc->log_write) svc->log_write("boot: config missing, default launcher");
    return 1;
  }
  (void)len;

  rc = ini_get_value((const char *)buf, "boot", "boot_to", boot_to, sizeof(boot_to));
  svc->fs_free(buf);
  if (rc != WIIOS_OK) {
    if (svc->log_write) svc->log_write("boot: invalid boot_to, default launcher");
    return 1;
  }
  if (boot_to[0] == 'd' || boot_to[0] == 'D') return 0;
  if (boot_to[0] == 'l' || boot_to[0] == 'L') return 1;
  if (svc->log_write) svc->log_write("boot: unknown boot_to value, default launcher");
  return 1;
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
  const WiiShell *shell;
  WiiLoadedApp ext_app;
  WiiResult load_rc;
  char hello_manifest[160];
  int ext_active = 0;
  int launcher_mode;
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
  if (path_resolver_config_missing()) startup_status = "CONFIG MISSING - USING DEFAULTS";

  svc->log_write("WiiOS v0.1.0-dev boot");
  event_queue_init();

  load_rc = WIIOS_E_FAIL;
  if (path_resolver_join(WIIOS_HELLO_MANIFEST_REL, hello_manifest, sizeof(hello_manifest)) == WIIOS_OK) {
    load_rc = app_loader_load_manifest(svc, hello_manifest, &ext_app);
  }
  if (load_rc == WIIOS_OK) {
    ext_active = 1;
    svc->log_write("external app loaded: hello");
  } else {
    svc->log_write("external app load skipped");
    startup_status = (load_rc == WIIOS_E_NOENT) ? "APP MANIFEST MISSING" : "APP MANIFEST INVALID";
  }

  launcher_mode = boot_to_launcher(svc);
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
        shell->handle_event(&ev);
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
      shell->update(dt_ms);
      shell->draw(&surface);
      if (ext_active) ext_app.api.draw(&surface);
    }
    draw_status_overlay(&surface, startup_status);
    vi_present(&surface);
  }

  if (launcher_mode) {
    launcher.shutdown();
  } else {
    if (ext_active) ext_app.api.shutdown();
    shell->shutdown();
  }
  svc->log_write("WiiOS v0.1.0-dev shutdown");
  input_shutdown();
  service_manager_shutdown();

  ringlog_dump_stdout();
  vi_shutdown();
  puts("main loop complete");
  return 0;
}
