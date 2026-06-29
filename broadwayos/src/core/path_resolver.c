#include "path_resolver.h"

#include <stdio.h>
#include <string.h>

#include "os_paths.h"

#define ROOT_CANDIDATE_COUNT 9
#define APPDIR_CANDIDATE_COUNT 9

static char g_root[32];
static int g_config_missing;
static int g_using_usb;
static const WiiServices *g_svc;

static const char *k_root_candidates[ROOT_CANDIDATE_COUNT] = {
  "sd:/wiios",
  "sd0:/wiios",
  "sd1:/wiios",
  "usb:/wiios",
  "usb0:/wiios",
  "usb1:/wiios",
  "fat:/wiios",
  "fat0:/wiios",
  "fat1:/wiios",
};

static const char *k_appdir_candidates[APPDIR_CANDIDATE_COUNT] = {
  "sd:/apps/wiios",
  "sd0:/apps/wiios",
  "sd1:/apps/wiios",
  "usb:/apps/wiios",
  "usb0:/apps/wiios",
  "usb1:/apps/wiios",
  "fat:/apps/wiios",
  "fat0:/apps/wiios",
  "fat1:/apps/wiios",
};

static WiiResult read_exists(WiiServices *svc, const char *path) {
  if (svc->fs_exists) return svc->fs_exists(path);
  if (svc->fs_read_all && svc->fs_free) {
    void *buf;
    wii_u32 len;
    WiiResult rc = svc->fs_read_all(path, &buf, &len);
    if (rc == WIIOS_OK) {
      (void)len;
      svc->fs_free(buf);
    }
    return rc;
  }
  return WIIOS_E_INVAL;
}

static int starts_with(const char *s, const char *prefix) {
  size_t n;
  if (!s || !prefix) return 0;
  n = strlen(prefix);
  return strncmp(s, prefix, n) == 0;
}

static int root_is_usb_path(const char *root) {
  return starts_with(root, "usb");
}

static WiiResult device_root_from_root_path(const char *root, char *out, wii_u32 out_len) {
  const char *colon;
  int n;
  if (!root || !out || out_len == 0) return WIIOS_E_INVAL;
  colon = strchr(root, ':');
  if (!colon) return WIIOS_E_INVAL;
  n = snprintf(out, (size_t)out_len, "%.*s:/", (int)(colon - root), root);
  if (n < 0 || (wii_u32)n >= out_len) return WIIOS_E_NOMEM;
  return WIIOS_OK;
}

static int device_available(WiiServices *svc, const char *root) {
  char dev[16];
  if (!svc || !svc->fs_exists || !root) return 0;
  if (device_root_from_root_path(root, dev, sizeof(dev)) != WIIOS_OK) return 0;
  return svc->fs_exists(dev) == WIIOS_OK;
}

static int root_exists(WiiServices *svc, const char *root) {
  if (!svc || !svc->fs_exists || !root || !root[0]) return 0;
  return svc->fs_exists(root) == WIIOS_OK;
}

static int config_exists_in_root(WiiServices *svc, const char *root) {
  char cfg_path[128];
  WiiResult rc;
  if (!svc || !root) return 0;
  (void)snprintf(cfg_path, sizeof(cfg_path), "%s/%s", root, WIIOS_CONFIG_REL);
  rc = read_exists(svc, cfg_path);
  return rc == WIIOS_OK;
}

static void set_active_root(const char *root) {
  if (!root || !root[0]) root = WIIOS_APPDIR_SD;
  (void)strncpy(g_root, root, sizeof(g_root) - 1U);
  g_root[sizeof(g_root) - 1U] = '\0';
  g_using_usb = root_is_usb_path(g_root);
}

static void log_selected_root(WiiServices *svc, const char *tag, const char *root) {
  char msg[96];
  if (!svc || !svc->log_write || !tag || !root) return;
  (void)snprintf(msg, sizeof(msg), "storage root %s: %s", tag, root);
  svc->log_write(msg);
}

static wii_u32 append_unique(const char **out_roots, wii_u32 out_cap, wii_u32 used, const char *root,
                             int *out_truncated) {
  if (!out_roots || !root || !root[0]) return used;
  for (wii_u32 i = 0; i < used; ++i) {
    if (strcmp(out_roots[i], root) == 0) return used;
  }
  if (used >= out_cap) {
    if (out_truncated) *out_truncated = 1;
    return used;
  }
  out_roots[used++] = root;
  return used;
}

WiiResult path_resolver_init(WiiServices *svc) {
  wii_u32 i;

  if (!svc) return WIIOS_E_INVAL;

  g_svc = svc;
  g_config_missing = 0;
  g_using_usb = 0;

  for (i = 0; i < APPDIR_CANDIDATE_COUNT; ++i) {
    if (config_exists_in_root(svc, k_appdir_candidates[i])) {
      set_active_root(k_appdir_candidates[i]);
      log_selected_root(svc, "(appdir config)", g_root);
      return WIIOS_OK;
    }
  }
  for (i = 0; i < ROOT_CANDIDATE_COUNT; ++i) {
    if (config_exists_in_root(svc, k_root_candidates[i])) {
      set_active_root(k_root_candidates[i]);
      log_selected_root(svc, "(config)", g_root);
      return WIIOS_OK;
    }
  }

  for (i = 0; i < APPDIR_CANDIDATE_COUNT; ++i) {
    if (root_exists(svc, k_appdir_candidates[i])) {
      set_active_root(k_appdir_candidates[i]);
      g_config_missing = 1;
      log_selected_root(svc, "(appdir fallback)", g_root);
      return WIIOS_OK;
    }
  }
  for (i = 0; i < ROOT_CANDIDATE_COUNT; ++i) {
    if (root_exists(svc, k_root_candidates[i])) {
      set_active_root(k_root_candidates[i]);
      g_config_missing = 1;
      log_selected_root(svc, "(fallback)", g_root);
      return WIIOS_OK;
    }
  }

  for (i = 0; i < APPDIR_CANDIDATE_COUNT; ++i) {
    if (device_available(svc, k_appdir_candidates[i])) {
      set_active_root(k_appdir_candidates[i]);
      g_config_missing = 1;
      log_selected_root(svc, "(appdir fallback)", g_root);
      return WIIOS_OK;
    }
  }

  for (i = 0; i < ROOT_CANDIDATE_COUNT; ++i) {
    if (device_available(svc, k_root_candidates[i])) {
      set_active_root(k_root_candidates[i]);
      g_config_missing = 1;
      log_selected_root(svc, "(fallback)", g_root);
      return WIIOS_OK;
    }
  }

  set_active_root(WIIOS_APPDIR_SD);
  g_config_missing = 1;
  log_selected_root(svc, "(fallback)", g_root);
  return WIIOS_OK;
}

const char *path_resolver_root(void) {
  if (!g_root[0]) return WIIOS_APPDIR_SD;
  return g_root;
}

WiiResult path_resolver_join(const char *rel, char *out, wii_u32 out_len) {
  int n;

  if (!rel || !out || out_len == 0) return WIIOS_E_INVAL;
  if (!rel[0]) {
    n = snprintf(out, (size_t)out_len, "%s", path_resolver_root());
    if (n < 0 || (wii_u32)n >= out_len) return WIIOS_E_NOMEM;
    return WIIOS_OK;
  }
  n = snprintf(out, (size_t)out_len, "%s/%s", path_resolver_root(), rel);
  if (n < 0 || (wii_u32)n >= out_len) return WIIOS_E_NOMEM;
  return WIIOS_OK;
}

wii_u32 path_resolver_collect_roots(const char **out_roots, wii_u32 out_cap, int *out_truncated) {
  wii_u32 used = 0;
  if (!out_roots || out_cap == 0) return 0;
  if (out_truncated) *out_truncated = 0;

  used = append_unique(out_roots, out_cap, used, path_resolver_root(), out_truncated);
  for (wii_u32 i = 0; i < APPDIR_CANDIDATE_COUNT; ++i) {
    used = append_unique(out_roots, out_cap, used, k_appdir_candidates[i], out_truncated);
  }
  for (wii_u32 i = 0; i < ROOT_CANDIDATE_COUNT; ++i) {
    used = append_unique(out_roots, out_cap, used, k_root_candidates[i], out_truncated);
  }
  return used;
}

void path_resolver_set_root(const char *root) {
  if (!root || !root[0]) {
    if (g_svc && g_svc->log_write) {
      g_svc->log_write("path_resolver_set_root: ignored null/empty");
    }
    return;
  }
  set_active_root(root);
}

int path_resolver_config_missing(void) { return g_config_missing; }
int path_resolver_using_usb(void) { return g_using_usb; }
