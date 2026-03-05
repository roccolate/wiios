#ifndef WIIOS_INI_H
#define WIIOS_INI_H

#include "wiios_types.h"

WiiResult ini_get_value(const char *text, const char *section, const char *key, char *out, wii_u32 out_len);

#endif
