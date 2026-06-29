#include "backend_iface.h"

static WiiResult null_init(void) { return WIIOS_OK; }
static void null_shutdown(void) {}

static WiiResult null_fs_list(const char *path, char *out_buf, wii_u32 out_len) {
  (void)path;
  if (!out_buf || out_len < 1) return WIIOS_E_INVAL;
  out_buf[0] = '\0';
  return WIIOS_OK;
}

static WiiResult null_fs_read_all(const char *path, void **out_ptr, wii_u32 *out_len) {
  (void)path;
  (void)out_ptr;
  (void)out_len;
  return WIIOS_E_NOENT;
}

static WiiResult null_fs_write_all(const char *path, const void *data, wii_u32 len) {
  (void)path;
  (void)data;
  (void)len;
  return WIIOS_E_IO;
}

static WiiResult null_fs_exists(const char *path) {
  (void)path;
  return WIIOS_E_NOENT;
}

static WiiResult null_fs_mkdirs(const char *path) {
  (void)path;
  return WIIOS_E_IO;
}

static WiiResult null_fs_rename(const char *from, const char *to) {
  (void)from;
  (void)to;
  return WIIOS_E_IO;
}

static WiiResult null_fs_remove(const char *path) {
  (void)path;
  return WIIOS_E_NOENT;
}

static void null_fs_free(void *ptr) { (void)ptr; }

const WiiBackend *backend_null_get(void) {
  static const WiiBackend backend = {
    .name = "null",
    .init = null_init,
    .shutdown = null_shutdown,
    .fs_list = null_fs_list,
    .fs_read_all = null_fs_read_all,
    .fs_write_all = null_fs_write_all,
    .fs_exists = null_fs_exists,
    .fs_mkdirs = null_fs_mkdirs,
    .fs_rename = null_fs_rename,
    .fs_remove = null_fs_remove,
    .fs_free = null_fs_free,
  };
  return &backend;
}
