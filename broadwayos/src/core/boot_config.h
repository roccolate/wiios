#ifndef WIIOS_BOOT_CONFIG_H
#define WIIOS_BOOT_CONFIG_H

#include "wiios_services.h"

WiiResult boot_config_load_mode(WiiServices *svc, int *out_launcher_mode, const char **out_root_used);
WiiResult boot_config_save_mode(WiiServices *svc, int launcher_mode, const char **out_root_used);

#endif
