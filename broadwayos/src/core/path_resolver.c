#include "path_resolver.h"

#include <stdio.h>
#include <string.h>

#include "os_paths.h"

static char g_root[32];
static int g_config_missing;
static int g_using_usb;

static WiiResult read_exists(WiiServices *svc, const char *path) {
  void *buf;
  wii_u32 len;
  WiiResult rc;

  rc = svc->fs_read_all(path, &buf, &len);
  if (rc == WIIOS_OK) {
    (void)len;
    svc->fs_free(buf);
  }
  return rc;
}

WiiResult path_resolver_init(WiiServices *svc) {
  char cfg_path[128];
  WiiResult rc;

  if (!svc || !svc->fs_read_all || !svc->fs_free) return WIIOS_E_INVAL;

  g_config_missing = 0;
  g_using_usb = 0;

  (void)snprintf(cfg_path, sizeof(cfg_path), "%s/%s", WIIOS_ROOT_SD, WIIOS_CONFIG_REL);
  rc = read_exists(svc, cfg_path);
  if (rc == WIIOS_OK) {
    (void)strncpy(g_root, WIIOS_ROOT_SD, sizeof(g_root) - 1U);
    g_root[sizeof(g_root) - 1U] = '\0';
    return WIIOS_OK;
  }

  (void)snprintf(cfg_path, sizeof(cfg_path), "%s/%s", WIIOS_ROOT_USB, WIIOS_CONFIG_REL);
  rc = read_exists(svc, cfg_path);
  if (rc == WIIOS_OK) {
    (void)strncpy(g_root, WIIOS_ROOT_USB, sizeof(g_root) - 1U);
    g_root[sizeof(g_root) - 1U] = '\0';
    g_using_usb = 1;
    return WIIOS_OK;
  }

  (void)strncpy(g_root, WIIOS_ROOT_SD, sizeof(g_root) - 1U);
  g_root[sizeof(g_root) - 1U] = '\0';
  g_config_missing = 1;
  return WIIOS_OK;
}

const char *path_resolver_root(void) {
  if (!g_root[0]) return WIIOS_ROOT_SD;
  return g_root;
}

WiiResult path_resolver_join(const char *rel, char *out, wii_u32 out_len) {
  int n;

  if (!rel || !out || out_len == 0) return WIIOS_E_INVAL;
  n = snprintf(out, (size_t)out_len, "%s/%s", path_resolver_root(), rel);
  if (n < 0 || (wii_u32)n >= out_len) return WIIOS_E_NOMEM;
  return WIIOS_OK;
}

int path_resolver_config_missing(void) { return g_config_missing; }
int path_resolver_using_usb(void) { return g_using_usb; }
