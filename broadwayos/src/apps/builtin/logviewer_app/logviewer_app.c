#include "wiios_services.h"
#include "../../../ui/widget.h"
#include "../../../gfx/font_bitmap.h"
#include "wiios_ringlog.h"

#include <string.h>

#define LOGVIEWER_LINE_BUF 101
#define LOGVIEWER_LINES_CAP 128
#define LOGVIEWER_VISIBLE 50

static WiiAppContext *g_ctx;
static wii_u32 g_top_line;
static const char *g_line_ptrs[LOGVIEWER_LINES_CAP];
static wii_u32 g_line_lens[LOGVIEWER_LINES_CAP];
static wii_u32 g_line_count;

static void rebuild_lines(void) {
  const char *data = ringlog_data();
  size_t total = ringlog_size();
  const char *end = data + total;

  g_line_count = 0;
  g_line_ptrs[0] = data;
  for (const char *p = data; p < end && g_line_count < LOGVIEWER_LINES_CAP - 1; ++p) {
    if (*p == '\n') {
      g_line_lens[g_line_count] = (wii_u32)(p - g_line_ptrs[g_line_count]);
      g_line_count++;
      g_line_ptrs[g_line_count] = p + 1;
    }
  }
  if (g_line_count < LOGVIEWER_LINES_CAP && g_line_ptrs[g_line_count] < end) {
    g_line_lens[g_line_count] = (wii_u32)(end - g_line_ptrs[g_line_count]);
    g_line_count++;
  }

  if (g_top_line > g_line_count) g_top_line = (g_line_count > 0) ? g_line_count - 1 : 0;
}

static WiiResult logviewer_init(WiiAppContext *ctx) {
  g_ctx = ctx;
  g_top_line = 0;
  g_line_count = 0;
  if (g_ctx && g_ctx->svc && g_ctx->svc->log_write) g_ctx->svc->log_write("logviewer init");
  return WIIOS_OK;
}

static WiiResult logviewer_handle_event(const WiiEvent *ev) {
  if (!ev || ev->type != EV_ACTION) return WIIOS_OK;
  if (ev->data.act.action == ACT_PREV) {
    if (g_top_line > 0) g_top_line--;
  } else if (ev->data.act.action == ACT_NEXT) {
    if (g_line_count > LOGVIEWER_VISIBLE &&
        g_top_line + LOGVIEWER_VISIBLE < g_line_count) {
      g_top_line++;
    }
  }
  return WIIOS_OK;
}

static WiiResult logviewer_draw(WiiSurface *surface) {
  char line_buf[LOGVIEWER_LINE_BUF];
  wii_i32 y = 56;
  wii_i32 max_y = 20 + 440 - 16;
  wii_u32 drawn = 0;

  widget_draw_panel(surface, (WiiRect){20, 20, 600, 440}, 0xFF1D2430);
  widget_draw_panel(surface, (WiiRect){20, 20, 600, 24}, 0xFF2C3B52);
  font_draw_text(surface, 32, 28, "LOG VIEWER", 0xFFEAF2FF, 1);
  font_draw_text(surface, 480, 28, "UP DN SCROLL  B BACK", 0xFFB5C9DE, 1);

  rebuild_lines();

  if (g_line_count == 0) {
    font_draw_text(surface, 32, 56, "NO LOG ENTRIES", 0xFF8FC2E4, 1);
    return WIIOS_OK;
  }

  for (wii_u32 i = g_top_line; i < g_line_count && drawn < LOGVIEWER_VISIBLE; ++i) {
    wii_u32 n = g_line_lens[i];
    if (n > LOGVIEWER_LINE_BUF - 1) n = LOGVIEWER_LINE_BUF - 1;
    if (n > 0) memcpy(line_buf, g_line_ptrs[i], n);
    line_buf[n] = '\0';
    if (y > max_y - 8) break;
    font_draw_text(surface, 32, y, line_buf, 0xFFB7D4EA, 1);
    y += 8;
    drawn++;
  }
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
