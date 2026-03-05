#ifndef WIIOS_SURFACE_H
#define WIIOS_SURFACE_H

#include <stddef.h>
#include "../../../shared/include/wiios_types.h"

typedef struct WiiSurface {
  void *pixels;
  wii_u32 width;
  wii_u32 height;
  wii_u32 stride_bytes;
  wii_u32 pixel_format;
} WiiSurface;

#define WIIOS_PIXEL_ARGB8888 0
#define WIIOS_PIXEL_YUYV422  1

void surface_clear(WiiSurface *s, wii_u32 color);
void surface_fill_rect(WiiSurface *s, WiiRect r, wii_u32 color);

#endif
