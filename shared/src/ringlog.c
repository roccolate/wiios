#include <stdio.h>
#include <string.h>

#define RINGLOG_CAP 4096

static char g_ring[RINGLOG_CAP];
static size_t g_len;

void ringlog_write(const char *msg) {
  size_t n = strlen(msg);
  if (n + 1 >= RINGLOG_CAP) {
    msg += (n - (RINGLOG_CAP - 2));
    n = RINGLOG_CAP - 2;
  }
  if (g_len + n + 1 >= RINGLOG_CAP) {
    size_t trim = (g_len + n + 1) - RINGLOG_CAP;
    memmove(g_ring, g_ring + trim, g_len - trim);
    g_len -= trim;
  }
  memcpy(g_ring + g_len, msg, n);
  g_len += n;
  g_ring[g_len++] = '\n';
}

const char *ringlog_data(void) { return g_ring; }
size_t ringlog_size(void) { return g_len; }

void ringlog_dump_stdout(void) {
  fwrite(g_ring, 1, g_len, stdout);
}
