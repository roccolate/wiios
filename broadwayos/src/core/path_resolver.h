#ifndef WIIOS_PATH_RESOLVER_H
#define WIIOS_PATH_RESOLVER_H

#include "wiios_services.h"

WiiResult path_resolver_init(WiiServices *svc);
const char *path_resolver_root(void);
WiiResult path_resolver_join(const char *rel, char *out, wii_u32 out_len);
wii_u32 path_resolver_collect_roots(const char **out_roots, wii_u32 out_cap, int *out_truncated);
void path_resolver_set_root(const char *root);
int path_resolver_config_missing(void);
int path_resolver_using_usb(void);

#endif
