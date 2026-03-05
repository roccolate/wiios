#include <stdio.h>
#include "../../shared/include/wiios_services.h"
#include "../../shared/include/wiios_ini.h"
#include "gfx/surface.h"
#include "core/event_queue.h"
#include "core/input.h"
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

static int boot_to_launcher(WiiServices *svc) {
  void *buf;
  wii_u32 len;
  char boot_to[32];
  WiiResult rc;

  if (!svc || !svc->fs_read_all || !svc->fs_free) return 0;
  rc = svc->fs_read_all(WIIOS_CONFIG_PATH, &buf, &len);
  if (rc != WIIOS_OK) return 0;
  (void)len;

  rc = ini_get_value((const char *)buf, "boot", "boot_to", boot_to, sizeof(boot_to));
  svc->fs_free(buf);
  if (rc != WIIOS_OK) return 0;
  return boot_to[0] == 'l' || boot_to[0] == 'L';
}

int main(void) {
  wii_u32 last_ms;
  wii_u32 now_ms;
  wii_u32 dt_ms;
  WiiEvent ev;
  WiiServices *svc;
  WiiAppContext ctx;
  WiiSurface surface = vi_init_surface();
  const WiiShell *shell;
  WiiLoadedApp ext_app;
  int ext_active = 0;
  int launcher_mode;
  WiiAppApi launcher = {0};
  int running = 1;

  if (service_manager_init() != WIIOS_OK) {
    os_log_write("service manager init failed");
    vi_shutdown();
    return 1;
  }
  svc = service_manager_services();
  ctx = (WiiAppContext){ .abi_version = 1, .svc = svc, .os_reserved = 0 };

  svc->log_write("WiiOS MS3 boot");
  event_queue_init();

  if (app_loader_load_manifest(svc, WIIOS_HELLO_MANIFEST, &ext_app) == WIIOS_OK) {
    ext_active = 1;
    svc->log_write("external app loaded: hello");
  } else {
    svc->log_write("external app load skipped");
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
    vi_present(&surface);
  }

  if (launcher_mode) {
    launcher.shutdown();
  } else {
    if (ext_active) ext_app.api.shutdown();
    shell->shutdown();
  }
  svc->log_write("WiiOS MS4 shutdown");
  input_shutdown();
  service_manager_shutdown();

  ringlog_dump_stdout();
  vi_shutdown();
  puts("main loop complete");
  return 0;
}
