#include "wiios_services.h"
#include "../../../ui/widget.h"
#include "../../../gfx/font_bitmap.h"
#include "os_paths.h"

static WiiAppContext *g_ctx;
static char g_list_buf[256];

static WiiResult filemgr_init(WiiAppContext *ctx) {
  g_ctx = ctx;
  g_list_buf[0] = '\0';
  if (g_ctx && g_ctx->svc) {
    (void)g_ctx->svc->fs_list(WIIOS_APPS_PATH, g_list_buf, sizeof(g_list_buf));
    if (g_ctx->svc->log_write) g_ctx->svc->log_write("filemgr init");
  }
  return WIIOS_OK;
}

static WiiResult filemgr_handle_event(const WiiEvent *ev) {
  (void)ev;
  return WIIOS_OK;
}

static WiiResult filemgr_draw(WiiSurface *surface) {
  widget_draw_panel(surface, (WiiRect){90, 80, 460, 300}, 0xFF12344A);
  widget_draw_panel(surface, (WiiRect){98, 88, 444, 32}, 0xFF1B4D6D);
  widget_draw_panel(surface, (WiiRect){98, 126, 444, 246}, 0xFF0F2636);
  font_draw_text(surface, 112, 100, "FILE MANAGER", 0xFFEAF6FF, 1);
  font_draw_text(surface, 112, 140, "PATH SD:/WIIOS/APPS", 0xFFB7D4EA, 1);
  if (g_list_buf[0]) {
    font_draw_text(surface, 112, 160, g_list_buf, 0xFF8FC2E4, 1);
  } else {
    font_draw_text(surface, 112, 160, "NO ENTRIES YET", 0xFF8FC2E4, 1);
  }
  (void)g_list_buf;
  return WIIOS_OK;
}

static void filemgr_shutdown(void) {
  if (g_ctx && g_ctx->svc && g_ctx->svc->log_write) g_ctx->svc->log_write("filemgr shutdown");
}

WiiAppApi filemgr_app_api(void) {
  WiiAppApi api = {
    .init = filemgr_init,
    .handle_event = filemgr_handle_event,
    .draw = filemgr_draw,
    .shutdown = filemgr_shutdown,
  };
  return api;
}
