#include "surface.h"
#include <stdint.h>

static void rgb_to_ycbcr(wii_u32 color, uint8_t *y, uint8_t *cb, uint8_t *cr) {
  int r = (int)((color >> 16) & 0xFF);
  int g = (int)((color >> 8) & 0xFF);
  int b = (int)(color & 0xFF);
  int yi = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
  int cbi = ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
  int cri = ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;
  if (yi < 0) yi = 0;
  if (yi > 255) yi = 255;
  if (cbi < 0) cbi = 0;
  if (cbi > 255) cbi = 255;
  if (cri < 0) cri = 0;
  if (cri > 255) cri = 255;
  *y = (uint8_t)yi;
  *cb = (uint8_t)cbi;
  *cr = (uint8_t)cri;
}

void surface_clear(WiiSurface *s, wii_u32 color) {
  surface_fill_rect(s, (WiiRect){0, 0, (wii_i32)s->width, (wii_i32)s->height}, color);
}

void surface_fill_rect(WiiSurface *s, WiiRect r, wii_u32 color) {
  if (!s || !s->pixels || r.w <= 0 || r.h <= 0) return;
  if (r.x < 0) { r.w += r.x; r.x = 0; }
  if (r.y < 0) { r.h += r.y; r.y = 0; }
  if (r.x + r.w > (wii_i32)s->width) r.w = (wii_i32)s->width - r.x;
  if (r.y + r.h > (wii_i32)s->height) r.h = (wii_i32)s->height - r.y;
  if (r.w <= 0 || r.h <= 0) return;

  if (s->pixel_format == WIIOS_PIXEL_ARGB8888) {
    for (wii_i32 y = 0; y < r.h; ++y) {
      uint32_t *row = (uint32_t *)((uint8_t *)s->pixels + (r.y + y) * s->stride_bytes);
      for (wii_i32 x = 0; x < r.w; ++x) row[r.x + x] = color;
    }
    return;
  }

  if (s->pixel_format == WIIOS_PIXEL_YUYV422) {
    uint8_t yv;
    uint8_t cb;
    uint8_t cr;
    wii_i32 x0 = r.x & ~1;
    wii_i32 x1 = (r.x + r.w + 1) & ~1;
    if (x1 > (wii_i32)s->width) x1 = (wii_i32)s->width & ~1;

    rgb_to_ycbcr(color, &yv, &cb, &cr);
    for (wii_i32 y = 0; y < r.h; ++y) {
      uint32_t *row = (uint32_t *)((uint8_t *)s->pixels + (r.y + y) * s->stride_bytes);
      for (wii_i32 x = x0; x < x1; x += 2) {
        row[x >> 1] = ((uint32_t)yv << 24) | ((uint32_t)cb << 16) | ((uint32_t)yv << 8) | cr;
      }
    }
  }
}
