#ifndef WIIOS_MANIFEST_H
#define WIIOS_MANIFEST_H

#include "wiios_types.h"

#define WIIOS_STR_SMALL 32
#define WIIOS_STR_MED 64
#define WIIOS_STR_PATH 128

typedef struct {
  char id[WIIOS_STR_SMALL];
  char title[WIIOS_STR_MED];
  char kind[WIIOS_STR_SMALL];
  char entry[WIIOS_STR_PATH];
  char icon[WIIOS_STR_PATH];
  char version[WIIOS_STR_SMALL];
  char requires[WIIOS_STR_MED];

  /* Legacy aliases kept while WiiOS migrates to retrocore-spec fields. */
  char name[WIIOS_STR_MED];
  char type[WIIOS_STR_SMALL];
} WiiManifest;

WiiResult manifest_parse_ini(const char *ini, WiiManifest *out_manifest);

#endif
