#ifndef WIIOS_APP_LOADER_H
#define WIIOS_APP_LOADER_H

#include "wiios_manifest.h"
#include "wiios_services.h"

typedef struct {
  WiiManifest manifest;
  WiiAppApi api;
  int loaded;
} WiiLoadedApp;

WiiResult app_loader_load_manifest(WiiServices *svc, const char *manifest_path, WiiLoadedApp *out_app);

#endif
