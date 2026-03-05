#include "../../../../../shared/include/wiios_services.h"
#include "../../../ui/widget.h"

extern const char *ringlog_data(void);
extern size_t ringlog_size(void);

static WiiAppContext *g_ctx;

static WiiResult logviewer_init(WiiAppContext *ctx) {
  g_ctx = ctx;
  if (g_ctx && g_ctx->svc && g_ctx->svc->log_write) g_ctx->svc->log_write("logviewer init");
  return WIIOS_OK;
}

static WiiResult logviewer_handle_event(const WiiEvent *ev) {
  (void)ev;
  return WIIOS_OK;
}

static WiiResult logviewer_draw(WiiSurface *surface) {
  widget_draw_panel(surface, (WiiRect){20, 20, 600, 440}, 0xFF1D2430);
  size_t n = ringlog_size();
  if (g_ctx && g_ctx->svc && g_ctx->svc->log_write) {
    (void)n;
  }
  (void)ringlog_data;
  return WIIOS_OK;
}

static void logviewer_shutdown(void) {
  if (g_ctx && g_ctx->svc && g_ctx->svc->log_write) g_ctx->svc->log_write("logviewer shutdown");
}

WiiAppApi logviewer_app_api(void) {
  WiiAppApi api = {
    .init = logviewer_init,
    .handle_event = logviewer_handle_event,
    .draw = logviewer_draw,
    .shutdown = logviewer_shutdown,
  };
  return api;
}
