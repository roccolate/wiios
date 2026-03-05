#include "service_manager.h"
#include "backends/backend_iface.h"

extern void os_log_write(const char *msg);
extern wii_u32 os_time_ms(void);
extern wii_u32 input_get_buttons(void);
extern WiiVec2 input_get_pointer(void);

static const WiiBackend *g_backend;
static WiiServices g_services;

WiiResult service_manager_init(void) {
  g_backend = backend_ios_get();
  if (!g_backend) return WIIOS_E_FAIL;
  if (g_backend->init() != WIIOS_OK) return WIIOS_E_FAIL;

  g_services.log_write = os_log_write;
  g_services.time_ms = os_time_ms;
  g_services.input_get_buttons = input_get_buttons;
  g_services.input_get_pointer = input_get_pointer;
  g_services.fs_list = g_backend->fs_list;
  g_services.fs_read_all = g_backend->fs_read_all;
  g_services.fs_write_all = g_backend->fs_write_all;
  g_services.fs_exists = g_backend->fs_exists;
  g_services.fs_mkdirs = g_backend->fs_mkdirs;
  g_services.fs_rename = g_backend->fs_rename;
  g_services.fs_free = g_backend->fs_free;
  return WIIOS_OK;
}

void service_manager_shutdown(void) {
  if (g_backend) g_backend->shutdown();
  g_backend = 0;
}

WiiServices *service_manager_services(void) {
  return &g_services;
}
