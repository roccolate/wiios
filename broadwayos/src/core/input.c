#include <gccore.h>
#include <wiiuse/wpad.h>
#include "../../../shared/include/wiios_types.h"
#include "event_queue.h"

static wii_u32 g_buttons;

static void push_action(WiiAction action) {
  (void)event_queue_push((WiiEvent){
    .type = EV_ACTION,
    .timestamp_ms = 0,
    .data.act = { .action = action },
  });
}

void input_init(void) {
  PAD_Init();
  WPAD_Init();
  WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
  g_buttons = 0;
}

void input_shutdown(void) {}

void input_poll_actions(void) {
  u32 gc_down;
  u32 wpad_down;

  PAD_ScanPads();
  WPAD_ScanPads();

  gc_down = PAD_ButtonsDown(0);
  wpad_down = WPAD_ButtonsDown(0);
  g_buttons = PAD_ButtonsHeld(0) | WPAD_ButtonsHeld(0);

  if ((gc_down & PAD_BUTTON_A) || (wpad_down & (WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A))) push_action(ACT_OK);
  if ((gc_down & PAD_BUTTON_B) || (wpad_down & (WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B))) push_action(ACT_BACK);
  if ((gc_down & PAD_BUTTON_RIGHT) || (wpad_down & (WPAD_BUTTON_RIGHT | WPAD_CLASSIC_BUTTON_RIGHT))) push_action(ACT_NEXT);
  if ((gc_down & PAD_BUTTON_LEFT) || (wpad_down & (WPAD_BUTTON_LEFT | WPAD_CLASSIC_BUTTON_LEFT))) push_action(ACT_PREV);
  if ((gc_down & PAD_BUTTON_START) || (wpad_down & WPAD_BUTTON_HOME)) push_action(ACT_HOME);
}

wii_u32 input_get_buttons(void) { return g_buttons; }

WiiVec2 input_get_pointer(void) {
  ir_t ir;
  WPAD_IR(0, &ir);
  if (ir.valid) {
    return (WiiVec2){(wii_i32)ir.x, (wii_i32)ir.y};
  }
  return (WiiVec2){0, 0};
}
