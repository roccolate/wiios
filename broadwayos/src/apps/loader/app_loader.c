#include "app_loader.h"

#include <string.h>

WiiAppApi hello_app_api(void);

/*
 * Loads a manifest from disk and resolves it to a built-in app api.
 *
 * NOTE: this loader is intentionally minimal. The "hello" app is hardcoded;
 * the manifest's entry and icon fields are validated for shape but not
 * used to drive any dynamic loading. A future iteration should scan
 * <root>/apps/<app-id>/manifest.ini, validate that entry exists on disk
 * and map ids to dynamically-loaded apis.
 */
WiiResult app_loader_load_manifest(WiiServices *svc, const char *manifest_path, WiiLoadedApp *out_app) {
  void *buf;
  wii_u32 len;
  WiiResult rc;

  if (!svc || !manifest_path || !out_app) return WIIOS_E_INVAL;
  memset(out_app, 0, sizeof(*out_app));

  rc = svc->fs_read_all(manifest_path, &buf, &len);
  if (rc != WIIOS_OK) return rc;

  (void)len;
  rc = manifest_parse_ini((const char *)buf, &out_app->manifest);
  svc->fs_free(buf);
  if (rc != WIIOS_OK) return WIIOS_E_INVAL;
  if (!out_app->manifest.id[0]) return WIIOS_E_INVAL;

  if (strcmp(out_app->manifest.id, "hello") == 0) {
    if (!out_app->manifest.entry[0] || !out_app->manifest.type[0]) return WIIOS_E_INVAL;
    out_app->api = hello_app_api();
    out_app->loaded = 1;
    return WIIOS_OK;
  }

  /* Unknown app id: not yet supported. */
  return WIIOS_E_NOENT;
}
