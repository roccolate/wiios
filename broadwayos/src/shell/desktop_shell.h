#ifndef WIIOS_DESKTOP_SHELL_H
#define WIIOS_DESKTOP_SHELL_H

#include "shell_iface.h"
#include "wiios_services.h"

void desktop_shell_set_context(WiiAppContext *ctx);
const WiiShell *desktop_shell_get(void);

#endif
