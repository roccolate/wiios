#include "backend_iface.h"

#include <fat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static WiiResult ios_init(void) {
  return fatInitDefault() ? WIIOS_OK : WIIOS_E_IO;
}

static void ios_shutdown(void) {}

static WiiResult ios_fs_list(const char *path, char *out_buf, wii_u32 out_len) {
  DIR *d;
  struct dirent *ent;
  wii_u32 used = 0;

  if (!path || !out_buf || out_len < 2) return WIIOS_E_INVAL;
  out_buf[0] = '\0';

  d = opendir(path);
  if (!d) return WIIOS_E_NOENT;

  while ((ent = readdir(d)) != 0) {
    size_t n;
    if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) continue;
    n = strlen(ent->d_name);
    if (used + (wii_u32)n + 2U >= out_len) break;
    memcpy(out_buf + used, ent->d_name, n);
    used += (wii_u32)n;
    out_buf[used++] = '\n';
    out_buf[used] = '\0';
  }
  closedir(d);
  return WIIOS_OK;
}

static WiiResult ios_fs_read_all(const char *path, void **out_ptr, wii_u32 *out_len) {
  FILE *f;
  long n;
  void *buf;

  if (!path || !out_ptr || !out_len) return WIIOS_E_INVAL;

  f = fopen(path, "rb");
  if (!f) return WIIOS_E_NOENT;

  if (fseek(f, 0, SEEK_END) != 0) {
    fclose(f);
    return WIIOS_E_IO;
  }

  n = ftell(f);
  if (n < 0) {
    fclose(f);
    return WIIOS_E_IO;
  }

  if (fseek(f, 0, SEEK_SET) != 0) {
    fclose(f);
    return WIIOS_E_IO;
  }

  buf = malloc((size_t)n + 1U);
  if (!buf) {
    fclose(f);
    return WIIOS_E_NOMEM;
  }

  if (fread(buf, 1, (size_t)n, f) != (size_t)n) {
    free(buf);
    fclose(f);
    return WIIOS_E_IO;
  }
  ((char *)buf)[n] = '\0';

  fclose(f);
  *out_ptr = buf;
  *out_len = (wii_u32)n;
  return WIIOS_OK;
}

static void ios_fs_free(void *ptr) {
  free(ptr);
}

const WiiBackend *backend_ios_get(void) {
  static const WiiBackend backend = {
    .name = "ios",
    .init = ios_init,
    .shutdown = ios_shutdown,
    .fs_list = ios_fs_list,
    .fs_read_all = ios_fs_read_all,
    .fs_free = ios_fs_free,
  };
  return &backend;
}
