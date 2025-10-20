#include "app.h"
#include "em_chip.h"
#include "tetris.h"
#include "main_menu.h"

#include "game_state.h"

#include "sl_simple_button_instances.h"
#include "sl_board_control.h"
#include "sl_assert.h"
#include "dmd.h"
#include "sl_joystick.h"
#include "sl_sleeptimer.h"

// Debounce/rate limit for joystick to prevent overly fast inputs
static uint32_t last_joystick_poll_time = 0;
#define JOYSTICK_POLL_DELAY_MS 150

static sl_joystick_t sl_joystick_handle = JOYSTICK_HANDLE_DEFAULT;

void app_init(void)
{
  CHIP_Init();
  uint32_t status;

  // Enable the memory lcd
  status = sl_board_enable_display();
  EFM_ASSERT(status == SL_STATUS_OK);

  // Initialize the DMD support for memory lcd display
  status = DMD_init(0);
  EFM_ASSERT(status == DMD_OK);

  // Initialize the Joystick driver
  sl_joystick_init(&sl_joystick_handle);
  sl_joystick_start(&sl_joystick_handle);

  // Initialize game modules
  tetris_init();
  main_menu_init();
  slot_menu_init();
}

// This is called on every iteration of the main while loop
void app_process_action(void)
{
  tetris_process_action();
  game_state_t current_state = tetris_get_game_state();

  // --- Handle Input ---
  sl_joystick_position_t pos = JOYSTICK_NONE;
  if (sl_sleeptimer_get_tick_count() - last_joystick_poll_time > sl_sleeptimer_ms_to_tick(JOYSTICK_POLL_DELAY_MS)) {
    last_joystick_poll_time = sl_sleeptimer_get_tick_count();
    sl_joystick_get_position(&sl_joystick_handle, &pos);
  }

  if (current_state == GAME_STATE_MAIN_MENU) {
    main_menu_handle_input(pos, NULL);
  } else if (current_state == GAME_STATE_IN_GAME) {
    switch (pos) {
      case JOYSTICK_W: tetris_move_left(); break;
      case JOYSTICK_E: tetris_move_right(); break;
      case JOYSTICK_S: tetris_move_down(); break;
      case JOYSTICK_N: tetris_rotate(); break;
      default: break;
    }
  } else if (current_state == GAME_STATE_SLOT_SELECTION) {
    slot_menu_handle_input(pos, NULL);
  }

  // --- Handle Drawing ---
  // Note: Drawing is handled within the state update functions (e.g., tetris_update) for the game
  // and on demand for the menu.
  if (current_state == GAME_STATE_MAIN_MENU) {
    main_menu_draw();
  } else if (current_state == GAME_STATE_PAUSED) {
    tetris_draw_board();
  } else if (current_state == GAME_STATE_SLOT_SELECTION) {
    slot_menu_draw();
  }
  // For IN_GAME and GAME_OVER, drawing is handled by tetris_update and its call to tetris_draw_board
}

void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) != SL_SIMPLE_BUTTON_PRESSED) {
    return;
  }

  game_state_t current_state = tetris_get_game_state();

  if (current_state == GAME_STATE_MAIN_MENU) {
    // Pass button presses to menu handler
    main_menu_handle_input(JOYSTICK_NONE, handle);
  } else if (current_state == GAME_STATE_IN_GAME) {
      if (handle == &sl_button_btn1) {
        tetris_pause_game();
      }
      if (handle == &sl_button_btn0) { // BTN0 is for saving
        tetris_save_game();
      }
  } else if (current_state == GAME_STATE_PAUSED) {
      if (handle == &sl_button_btn1) {
        tetris_resume_game();
      }
  } else if (current_state == GAME_STATE_SLOT_SELECTION) {
      slot_menu_handle_input(JOYSTICK_NONE, handle);
  } else if (current_state == GAME_STATE_GAME_OVER) {
    if (handle == &sl_button_btn1) { // BTN1 is "Start"
      tetris_set_game_state(GAME_STATE_MAIN_MENU);
    }
  }
}
