#ifndef WIIOS_BACKEND_IFACE_H
#define WIIOS_BACKEND_IFACE_H

#include "wiios_types.h"

typedef struct {
  const char *name;
  WiiResult (*init)(void);
  void (*shutdown)(void);
  WiiResult (*fs_list)(const char *path, char *out_buf, wii_u32 out_len);
  WiiResult (*fs_read_all)(const char *path, void **out_ptr, wii_u32 *out_len);
  WiiResult (*fs_write_all)(const char *path, const void *data, wii_u32 len);
  WiiResult (*fs_exists)(const char *path);
  WiiResult (*fs_mkdirs)(const char *path);
  WiiResult (*fs_rename)(const char *from, const char *to);
  WiiResult (*fs_remove)(const char *path);
  void (*fs_free)(void *ptr);
} WiiBackend;

const WiiBackend *backend_ios_get(void);
const WiiBackend *backend_null_get(void);

#endif
