#ifndef WIIOS_SERVICES_H
#define WIIOS_SERVICES_H

#include "wiios_events.h"

typedef struct WiiSurface WiiSurface;
typedef struct WiiServices WiiServices;

typedef struct {
  wii_u32 abi_version;
  WiiServices *svc;
  void *os_reserved;
} WiiAppContext;

struct WiiServices {
  void (*log_write)(const char *msg);
  wii_u32 (*time_ms)(void);
  wii_u32 (*input_get_buttons)(void);
  WiiVec2 (*input_get_pointer)(void);
  WiiResult (*fs_list)(const char *path, char *out_buf, wii_u32 out_len);
  WiiResult (*fs_read_all)(const char *path, void **out_ptr, wii_u32 *out_len);
  void (*fs_free)(void *ptr);
};

typedef struct {
  WiiResult (*init)(WiiAppContext *ctx);
  WiiResult (*handle_event)(const WiiEvent *ev);
  WiiResult (*draw)(WiiSurface *surface);
  void (*shutdown)(void);
} WiiAppApi;

#endif
