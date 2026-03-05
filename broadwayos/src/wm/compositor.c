#include "window_manager.h"

void compositor_draw(const WiiWindowManager *wm, WiiSurface *surface) {
  wm_draw(wm, surface);
}
