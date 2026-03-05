#ifndef WIIOS_SERVICE_MANAGER_H
#define WIIOS_SERVICE_MANAGER_H

#include "wiios_services.h"

WiiResult service_manager_init(void);
void service_manager_shutdown(void);
WiiServices *service_manager_services(void);

#endif
