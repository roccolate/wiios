#ifndef WIIOS_EVENTS_H
#define WIIOS_EVENTS_H

#include "wiios_types.h"

typedef enum {
  EV_NONE = 0,
  EV_KEY_DOWN,
  EV_KEY_UP,
  EV_POINTER_MOVE,
  EV_POINTER_DOWN,
  EV_POINTER_UP,
  EV_ACTION
} WiiEventType;

typedef enum {
  ACT_OK = 1,
  ACT_BACK,
  ACT_MENU,
  ACT_HOME,
  ACT_UP,
  ACT_DOWN,
  ACT_LEFT,
  ACT_RIGHT,
  ACT_NEXT,
  ACT_PREV
} WiiAction;

typedef struct {
  WiiEventType type;
  wii_u32 timestamp_ms;
  union {
    struct { wii_u32 keycode; } key;
    struct { WiiVec2 pos; wii_u32 button; } pointer;
    struct { WiiAction action; } act;
  } data;
} WiiEvent;

#endif
