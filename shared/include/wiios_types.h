#ifndef WIIOS_TYPES_H
#define WIIOS_TYPES_H

#include <stdint.h>

typedef int32_t wii_i32;
typedef uint32_t wii_u32;
typedef uint8_t wii_u8;

typedef struct { wii_i32 x, y; } WiiVec2;
typedef struct { wii_i32 x, y, w, h; } WiiRect;

typedef enum {
  WIIOS_OK = 0,
  WIIOS_E_FAIL = -1,
  WIIOS_E_NOENT = -2,
  WIIOS_E_PERM = -3,
  WIIOS_E_INVAL = -4,
  WIIOS_E_NOMEM = -5,
  WIIOS_E_IO = -6,
  WIIOS_E_TIMEOUT = -7
} WiiResult;

#endif
