#include "boot_config.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "wiios_ini.h"
#include "path_resolver.h"
#include "os_paths.h"

#define BOOT_CONFIG_ROOT_CAP 32
#define BOOT_CONFIG_BODY_CAP 1024

static int mode_from_text(const char *s) {
  if (!s || !s[0]) return 1;
  if (s[0] == 'd' || s[0] == 'D') return 0;
  return 1;
}

static int is_valid_mode_text(const char *s) {
  return s && (s[0] == 'l' || s[0] == 'L' || s[0] == 'd' || s[0] == 'D');
}

static WiiResult build_path(const char *root, const char *rel, char *out, wii_u32 out_len) {
  int n;
  if (!root || !rel || !out || out_len == 0) return WIIOS_E_INVAL;
  n = snprintf(out, (size_t)out_len, "%s/%s", root, rel);
  if (n < 0 || (wii_u32)n >= out_len) return WIIOS_E_NOMEM;
  return WIIOS_OK;
}

static void trim_span(const char **start, const char **end) {
  while (*start < *end && isspace((unsigned char)**start)) (*start)++;
  while (*end > *start && isspace((unsigned char)*((*end) - 1))) (*end)--;
}

static int span_equals(const char *start, const char *end, const char *lit) {
  size_t n = (size_t)(end - start);
  return strlen(lit) == n && strncmp(start, lit, n) == 0;
}

static WiiResult append_span(char *out, wii_u32 out_len, wii_u32 *used, const char *data, wii_u32 len) {
  if (!out || !used || (len > 0 && !data)) return WIIOS_E_INVAL;
  if (*used + len + 1U > out_len) return WIIOS_E_NOMEM;
  if (len > 0) memcpy(out + *used, data, len);
  *used += len;
  out[*used] = '\0';
  return WIIOS_OK;
}

static WiiResult append_cstr(char *out, wii_u32 out_len, wii_u32 *used, const char *text) {
  if (!text) return WIIOS_E_INVAL;
  return append_span(out, out_len, used, text, (wii_u32)strlen(text));
}

static int parse_section_name(const char *start, const char *end, const char **name_start, const char **name_end) {
  const char *s;
  const char *close;

  if (!start || !end || start >= end || *start != '[') return 0;
  s = start + 1;
  close = s;
  while (close < end && *close != ']') ++close;
  if (close >= end || *close != ']') return 0;

  *name_start = s;
  *name_end = close;
  trim_span(name_start, name_end);
  return 1;
}

static int line_is_key(const char *start, const char *end, const char *key) {
  const char *eq;
  const char *k0;
  const char *k1;

  if (!start || !end || !key || start >= end) return 0;
  eq = start;
  while (eq < end && *eq != '=') ++eq;
  if (eq >= end || *eq != '=') return 0;

  k0 = start;
  k1 = eq;
  trim_span(&k0, &k1);
  return span_equals(k0, k1, key);
}

static WiiResult append_boot_to_line(char *out, wii_u32 out_len, wii_u32 *used, const char *mode) {
  char line[48];
  int n;
  if (!mode) return WIIOS_E_INVAL;
  n = snprintf(line, sizeof(line), "boot_to=%s\n", mode);
  if (n <= 0 || n >= (int)sizeof(line)) return WIIOS_E_NOMEM;
  return append_span(out, out_len, used, line, (wii_u32)n);
}

static WiiResult build_merged_config_body(const char *existing, int launcher_mode, char *out, wii_u32 out_len) {
  const char *mode = launcher_mode ? "launcher" : "desktop";
  wii_u32 used = 0;
  int in_boot = 0;
  int saw_boot_section = 0;
  int saw_boot_key = 0;
  int saw_system_section = 0;
  const char *p = existing;
  WiiResult rc;

  if (!out || out_len == 0) return WIIOS_E_INVAL;
  out[0] = '\0';

  if (!existing || !existing[0]) {
    rc = append_cstr(out, out_len, &used, "[boot]\n");
    if (rc != WIIOS_OK) return rc;
    rc = append_boot_to_line(out, out_len, &used, mode);
    if (rc != WIIOS_OK) return rc;
    rc = append_cstr(out, out_len, &used, "\n[system]\nlanguage=es\ntheme=default\nschema=1\n");
    if (rc != WIIOS_OK) return rc;
    return WIIOS_OK;
  }

  while (*p) {
    const char *line = p;
    const char *end = p;
    const char *trim0;
    const char *trim1;

    while (*end && *end != '\n' && *end != '\r') ++end;
    p = end;
    while (*p == '\n' || *p == '\r') ++p;

    trim0 = line;
    trim1 = end;
    trim_span(&trim0, &trim1);

    if (trim0 < trim1 && *trim0 == '[') {
      const char *section0 = 0;
      const char *section1 = 0;
      if (in_boot && !saw_boot_key) {
        rc = append_boot_to_line(out, out_len, &used, mode);
        if (rc != WIIOS_OK) return rc;
        saw_boot_key = 1;
      }
      if (parse_section_name(trim0, trim1, &section0, &section1)) {
        in_boot = span_equals(section0, section1, "boot");
        if (in_boot) saw_boot_section = 1;
        if (span_equals(section0, section1, "system")) saw_system_section = 1;
      } else {
        in_boot = 0;
      }
      rc = append_span(out, out_len, &used, line, (wii_u32)(end - line));
      if (rc != WIIOS_OK) return rc;
      rc = append_cstr(out, out_len, &used, "\n");
      if (rc != WIIOS_OK) return rc;
      continue;
    }

    if (in_boot && line_is_key(trim0, trim1, "boot_to")) {
      if (!saw_boot_key) {
        rc = append_boot_to_line(out, out_len, &used, mode);
        if (rc != WIIOS_OK) return rc;
        saw_boot_key = 1;
      }
      continue;
    }

    rc = append_span(out, out_len, &used, line, (wii_u32)(end - line));
    if (rc != WIIOS_OK) return rc;
    rc = append_cstr(out, out_len, &used, "\n");
    if (rc != WIIOS_OK) return rc;
  }

  if (in_boot && !saw_boot_key) {
    rc = append_boot_to_line(out, out_len, &used, mode);
    if (rc != WIIOS_OK) return rc;
    saw_boot_key = 1;
  }

  if (!saw_boot_section) {
    if (used > 0 && out[used - 1U] != '\n') {
      rc = append_cstr(out, out_len, &used, "\n");
      if (rc != WIIOS_OK) return rc;
    }
    rc = append_cstr(out, out_len, &used, "[boot]\n");
    if (rc != WIIOS_OK) return rc;
    rc = append_boot_to_line(out, out_len, &used, mode);
    if (rc != WIIOS_OK) return rc;
  }

  if (!saw_system_section) {
    if (used > 0 && out[used - 1U] != '\n') {
      rc = append_cstr(out, out_len, &used, "\n");
      if (rc != WIIOS_OK) return rc;
    }
    rc = append_cstr(out, out_len, &used, "[system]\nlanguage=es\ntheme=default\nschema=1\n");
    if (rc != WIIOS_OK) return rc;
  }

  return WIIOS_OK;
}

static void log_root_overflow(WiiServices *svc, const char *op) {
  char msg[72];
  if (!svc || !svc->log_write || !op) return;
  (void)snprintf(msg, sizeof(msg), "boot config %s roots truncated", op);
  svc->log_write(msg);
}

static WiiResult load_from_root(WiiServices *svc, const char *root, int *out_launcher_mode) {
  char cfg[160];
  char mode[32];
  void *buf;
  wii_u32 len;
  WiiResult rc;

  rc = build_path(root, WIIOS_CONFIG_REL, cfg, sizeof(cfg));
  if (rc != WIIOS_OK) return rc;

  rc = svc->fs_read_all(cfg, &buf, &len);
  if (rc != WIIOS_OK) return rc;
  (void)len;

  rc = ini_get_value((const char *)buf, "boot", "boot_to", mode, sizeof(mode));
  svc->fs_free(buf);
  if (rc != WIIOS_OK) return WIIOS_E_INVAL;
  if (!is_valid_mode_text(mode)) return WIIOS_E_INVAL;

  *out_launcher_mode = mode_from_text(mode);
  return WIIOS_OK;
}

static WiiResult save_to_root(WiiServices *svc, const char *root, int launcher_mode) {
  char cfg[160];
  char tmp[160];
  char body[BOOT_CONFIG_BODY_CAP];
  void *existing_buf = 0;
  const char *existing = 0;
  wii_u32 existing_len = 0;
  WiiResult rc;

  rc = svc->fs_exists ? svc->fs_exists(root) : WIIOS_E_NOENT;
  if (rc == WIIOS_E_NOENT) {
    if (!svc->fs_mkdirs) return WIIOS_E_FAIL;
    rc = svc->fs_mkdirs(root);
    if (rc != WIIOS_OK) return rc;
  } else if (rc != WIIOS_OK) {
    return rc;
  }

  rc = build_path(root, WIIOS_CONFIG_REL, cfg, sizeof(cfg));
  if (rc != WIIOS_OK) return rc;
  rc = build_path(root, WIIOS_CONFIG_TMP_REL, tmp, sizeof(tmp));
  if (rc != WIIOS_OK) return rc;

  if (svc->fs_read_all && svc->fs_free) {
    rc = svc->fs_read_all(cfg, &existing_buf, &existing_len);
    if (rc == WIIOS_OK) {
      (void)existing_len;
      existing = (const char *)existing_buf;
    } else if (rc == WIIOS_E_NOENT) {
      existing = 0;
    } else {
      existing = 0;
    }
  }

  rc = build_merged_config_body(existing, launcher_mode, body, sizeof(body));
  if (existing_buf && svc->fs_free) svc->fs_free(existing_buf);
  if (rc != WIIOS_OK) return rc;

  rc = svc->fs_write_all(tmp, body, (wii_u32)strlen(body));
  if (rc != WIIOS_OK) return rc;
  rc = svc->fs_rename(tmp, cfg);
  if (rc != WIIOS_OK && svc->fs_remove) {
    /* Best-effort cleanup of the orphan tmp file. The tmp file's existence
     * would shadow the next save attempt and confuse the user on power loss. */
    WiiResult cleanup_rc = svc->fs_remove(tmp);
    if (cleanup_rc != WIIOS_OK && svc->log_write) {
      char msg[80];
      (void)snprintf(msg, sizeof(msg), "boot config tmp cleanup failed rc=%d", (int)cleanup_rc);
      svc->log_write(msg);
    }
  }
  return rc;
}

WiiResult boot_config_load_mode(WiiServices *svc, int *out_launcher_mode, const char **out_root_used) {
  const char *roots[BOOT_CONFIG_ROOT_CAP];
  wii_u32 count;
  WiiResult first_invalid = WIIOS_E_NOENT;
  int truncated = 0;

  if (!svc || !out_launcher_mode || !svc->fs_read_all || !svc->fs_free) return WIIOS_E_INVAL;
  if (out_root_used) *out_root_used = 0;

  count = path_resolver_collect_roots(roots, BOOT_CONFIG_ROOT_CAP, &truncated);
  if (truncated) log_root_overflow(svc, "load");

  for (wii_u32 i = 0; i < count; ++i) {
    WiiResult rc = load_from_root(svc, roots[i], out_launcher_mode);
    if (rc == WIIOS_OK) {
      path_resolver_set_root(roots[i]);
      if (out_root_used) *out_root_used = roots[i];
      return WIIOS_OK;
    }
    if (rc == WIIOS_E_INVAL && first_invalid == WIIOS_E_NOENT) first_invalid = WIIOS_E_INVAL;
  }

  return first_invalid;
}

WiiResult boot_config_save_mode(WiiServices *svc, int launcher_mode, const char **out_root_used) {
  const char *roots[BOOT_CONFIG_ROOT_CAP];
  wii_u32 count;
  WiiResult last = WIIOS_E_FAIL;
  int truncated = 0;

  if (!svc || !svc->fs_mkdirs || !svc->fs_write_all || !svc->fs_rename || !svc->fs_remove) return WIIOS_E_INVAL;
  if (out_root_used) *out_root_used = 0;

  count = path_resolver_collect_roots(roots, BOOT_CONFIG_ROOT_CAP, &truncated);
  if (truncated) log_root_overflow(svc, "save");

  for (wii_u32 i = 0; i < count; ++i) {
    WiiResult rc = save_to_root(svc, roots[i], launcher_mode);
    if (rc == WIIOS_OK) {
      path_resolver_set_root(roots[i]);
      if (out_root_used) *out_root_used = roots[i];
      return WIIOS_OK;
    }
    last = rc;
  }
  return last;
}
