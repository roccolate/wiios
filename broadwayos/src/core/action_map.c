#include "../../../shared/include/wiios_events.h"

WiiAction action_map_from_key(wii_u32 keycode) {
  switch (keycode) {
    case 13: return ACT_OK;
    case 27: return ACT_BACK;
    default: return ACT_MENU;
  }
}
