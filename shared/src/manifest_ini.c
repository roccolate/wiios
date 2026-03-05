#include "wiios_manifest.h"
#include "wiios_ini.h"

#include <string.h>

WiiResult manifest_parse_ini(const char *ini, WiiManifest *out_manifest) {
  WiiResult rc;

  if (!ini || !out_manifest) return WIIOS_E_INVAL;
  memset(out_manifest, 0, sizeof(*out_manifest));

  rc = ini_get_value(ini, "app", "id", out_manifest->id, sizeof(out_manifest->id));
  if (rc != WIIOS_OK) return rc;

  (void)ini_get_value(ini, "app", "name", out_manifest->name, sizeof(out_manifest->name));
  (void)ini_get_value(ini, "app", "version", out_manifest->version, sizeof(out_manifest->version));
  (void)ini_get_value(ini, "app", "entry", out_manifest->entry, sizeof(out_manifest->entry));
  (void)ini_get_value(ini, "app", "icon", out_manifest->icon, sizeof(out_manifest->icon));
  (void)ini_get_value(ini, "app", "type", out_manifest->type, sizeof(out_manifest->type));

  return WIIOS_OK;
}
