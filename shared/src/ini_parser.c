#include "wiios_ini.h"

#include <ctype.h>
#include <string.h>

static void trim_range(const char **start, const char **end) {
  while (*start < *end && isspace((unsigned char)**start)) (*start)++;
  while (*end > *start && isspace((unsigned char)*((*end) - 1))) (*end)--;
}

static int equals_span(const char *s, const char *e, const char *lit) {
  size_t n = (size_t)(e - s);
  return strlen(lit) == n && strncmp(s, lit, n) == 0;
}

WiiResult ini_get_value(const char *text, const char *section, const char *key, char *out, wii_u32 out_len) {
  const char *p;
  int in_section;

  if (!text || !section || !key || !out || out_len == 0) return WIIOS_E_INVAL;
  p = text;
  in_section = 0;

  while (*p) {
    const char *line = p;
    const char *end = p;
    const char *k;
    const char *v;
    const char *eq;

    while (*end && *end != '\n' && *end != '\r') ++end;
    p = end;
    while (*p == '\n' || *p == '\r') ++p;

    trim_range(&line, &end);
    if (line == end) continue;
    if (*line == ';' || *line == '#') continue;

    if (*line == '[') {
      const char *close = line + 1;
      while (close < end && *close != ']') ++close;
      if (close < end && *close == ']') {
        const char *s0 = line + 1;
        const char *s1 = close;
        trim_range(&s0, &s1);
        in_section = equals_span(s0, s1, section);
      } else {
        in_section = 0;
      }
      continue;
    }

    if (!in_section) continue;

    eq = line;
    while (eq < end && *eq != '=') ++eq;
    if (eq >= end || *eq != '=') continue;

    k = line;
    v = eq + 1;
    trim_range(&k, &eq);
    trim_range(&v, &end);

    if (!equals_span(k, eq, key)) continue;

    {
      wii_u32 n = (wii_u32)(end - v);
      if (n >= out_len) n = out_len - 1;
      memcpy(out, v, n);
      out[n] = '\0';
      return WIIOS_OK;
    }
  }

  return WIIOS_E_NOENT;
}
