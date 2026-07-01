#include "wiios_manifest.h"
#include "wiios_ini.h"

#include <string.h>

static void manifest_copy_alias(char *dst, wii_u32 dst_len, const char *src) {
  size_t n;

  if (!dst || dst_len == 0 || !src) return;
  n = strlen(src);
  if (n >= dst_len) n = dst_len - 1;
  memcpy(dst, src, n);
  dst[n] = '\0';
}

WiiResult manifest_parse_ini(const char *ini, WiiManifest *out_manifest) {
  WiiResult rc;

  if (!ini || !out_manifest) return WIIOS_E_INVAL;
  memset(out_manifest, 0, sizeof(*out_manifest));

  rc = ini_get_value(ini, "app", "id", out_manifest->id, sizeof(out_manifest->id));
  if (rc != WIIOS_OK) return rc;

  (void)ini_get_value(ini, "app", "title", out_manifest->title, sizeof(out_manifest->title));
  (void)ini_get_value(ini, "app", "kind", out_manifest->kind, sizeof(out_manifest->kind));
  (void)ini_get_value(ini, "app", "entry", out_manifest->entry, sizeof(out_manifest->entry));
  (void)ini_get_value(ini, "app", "icon", out_manifest->icon, sizeof(out_manifest->icon));
  (void)ini_get_value(ini, "app", "version", out_manifest->version, sizeof(out_manifest->version));
  (void)ini_get_value(ini, "app", "requires", out_manifest->requires, sizeof(out_manifest->requires));

  /* Backward compatibility with the v0.1 WiiOS manifest shape. */
  (void)ini_get_value(ini, "app", "name", out_manifest->name, sizeof(out_manifest->name));
  (void)ini_get_value(ini, "app", "type", out_manifest->type, sizeof(out_manifest->type));
  if (!out_manifest->title[0] && out_manifest->name[0]) {
    manifest_copy_alias(out_manifest->title, sizeof(out_manifest->title), out_manifest->name);
  }
  if (!out_manifest->kind[0] && out_manifest->type[0]) {
    manifest_copy_alias(out_manifest->kind, sizeof(out_manifest->kind), out_manifest->type);
  }
  if (!out_manifest->name[0] && out_manifest->title[0]) {
    manifest_copy_alias(out_manifest->name, sizeof(out_manifest->name), out_manifest->title);
  }
  if (!out_manifest->type[0] && out_manifest->kind[0]) {
    manifest_copy_alias(out_manifest->type, sizeof(out_manifest->type), out_manifest->kind);
  }

  return WIIOS_OK;
}
