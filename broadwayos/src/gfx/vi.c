#include <gccore.h>
#include <ogc/system.h>
#include "surface.h"

static GXRModeObj *g_rmode;
static void *g_xfb[2];
static int g_fb;
static WiiSurface g_surface;

WiiSurface vi_init_surface(void) {
  VIDEO_Init();

  g_rmode = VIDEO_GetPreferredMode(0);
  g_xfb[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(g_rmode));
  g_xfb[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(g_rmode));
  g_fb = 0;

  VIDEO_Configure(g_rmode);
  VIDEO_SetNextFramebuffer(g_xfb[g_fb]);
  VIDEO_SetBlack(FALSE);
  VIDEO_Flush();
  VIDEO_WaitVSync();
  if (g_rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

  g_surface.pixels = g_xfb[g_fb];
  g_surface.width = (wii_u32)g_rmode->fbWidth;
  g_surface.height = (wii_u32)g_rmode->xfbHeight;
  g_surface.stride_bytes = (wii_u32)g_rmode->fbWidth * 2U;
  g_surface.pixel_format = WIIOS_PIXEL_YUYV422;
  return g_surface;
}

void vi_present(WiiSurface *surface) {
  VIDEO_SetNextFramebuffer(g_xfb[g_fb]);
  VIDEO_Flush();
  VIDEO_WaitVSync();

  g_fb ^= 1;
  g_surface.pixels = g_xfb[g_fb];
  if (surface) surface->pixels = g_surface.pixels;
}

void vi_shutdown(void) {
  VIDEO_SetBlack(TRUE);
  VIDEO_Flush();
}
