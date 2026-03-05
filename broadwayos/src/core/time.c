#include <time.h>
#include "../../../shared/include/wiios_types.h"

wii_u32 os_time_ms(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (wii_u32)((ts.tv_sec * 1000ULL) + (ts.tv_nsec / 1000000ULL));
}
