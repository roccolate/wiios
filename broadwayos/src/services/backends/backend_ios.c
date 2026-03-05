#include "backend_iface.h"

#include <fat.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/iosupport.h>
#include <sys/stat.h>

#define IOS_PATH_CAP 256

static int ios_path_has_device(const char *path) {
  return path && GetDeviceOpTab(path) != 0;
}

static WiiResult ios_init(void) {
  return fatInitDefault() ? WIIOS_OK : WIIOS_E_IO;
}

static void ios_shutdown(void) {}

static WiiResult ios_fs_list(const char *path, char *out_buf, wii_u32 out_len) {
  DIR *d;
  struct dirent *ent;
  wii_u32 used = 0;

  if (!path || !out_buf || out_len < 2) return WIIOS_E_INVAL;
  if (!ios_path_has_device(path)) return WIIOS_E_NOENT;
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
  if (!ios_path_has_device(path)) return WIIOS_E_NOENT;

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

static WiiResult ios_fs_write_all(const char *path, const void *data, wii_u32 len) {
  FILE *f;

  if (!path || (!data && len > 0)) return WIIOS_E_INVAL;
  if (!ios_path_has_device(path)) return WIIOS_E_NOENT;
  f = fopen(path, "wb");
  if (!f) return WIIOS_E_IO;
  if (len > 0 && fwrite(data, 1, len, f) != len) {
    fclose(f);
    return WIIOS_E_IO;
  }
  fclose(f);
  return WIIOS_OK;
}

static WiiResult ios_fs_exists(const char *path) {
  struct stat st;
  if (!path) return WIIOS_E_INVAL;
  if (!ios_path_has_device(path)) return WIIOS_E_NOENT;
  return stat(path, &st) == 0 ? WIIOS_OK : WIIOS_E_NOENT;
}

static WiiResult ios_mkdir_one(const char *path) {
  struct stat st;
  if (stat(path, &st) == 0) {
    return S_ISDIR(st.st_mode) ? WIIOS_OK : WIIOS_E_IO;
  }
  if (mkdir(path, 0777) == 0 || errno == EEXIST) return WIIOS_OK;
  return WIIOS_E_IO;
}

static WiiResult ios_fs_mkdirs(const char *path) {
  char tmp[IOS_PATH_CAP];
  char *scan_start;
  char *p;
  size_t len;

  if (!path) return WIIOS_E_INVAL;
  if (!ios_path_has_device(path)) return WIIOS_E_NOENT;
  len = strlen(path);
  if (len == 0 || len >= sizeof(tmp)) return WIIOS_E_INVAL;
  memcpy(tmp, path, len + 1U);
  if (tmp[len - 1] == '/') tmp[len - 1] = '\0';

  scan_start = tmp + 1;
  p = strchr(tmp, ':');
  if (p && p[1] == '/') scan_start = p + 2;

  for (p = scan_start; *p; ++p) {
    if (*p == '/') {
      *p = '\0';
      if (ios_mkdir_one(tmp) != WIIOS_OK) return WIIOS_E_IO;
      *p = '/';
    }
  }
  return ios_mkdir_one(tmp);
}

static WiiResult ios_fs_rename(const char *from, const char *to) {
  if (!from || !to) return WIIOS_E_INVAL;
  if (!ios_path_has_device(from) || !ios_path_has_device(to)) return WIIOS_E_NOENT;
  return rename(from, to) == 0 ? WIIOS_OK : WIIOS_E_IO;
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
    .fs_write_all = ios_fs_write_all,
    .fs_exists = ios_fs_exists,
    .fs_mkdirs = ios_fs_mkdirs,
    .fs_rename = ios_fs_rename,
    .fs_free = ios_fs_free,
  };
  return &backend;
}
