/***************************************************************************//**
 * @file app.c
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include "app.h"
#include "tetris.h"
#include "sl_simple_button_instances.h"
#include "sl_board_control.h"
#include "sl_assert.h"
#include "dmd.h"
#include "sl_joystick.h"
#include "sl_sleeptimer.h"

#ifndef BUTTON_INSTANCE_0
#define BUTTON_INSTANCE_0   sl_button_btn0
#endif

#ifndef BUTTON_INSTANCE_1
#define BUTTON_INSTANCE_1   sl_button_btn1
#endif

static sl_joystick_t sl_joystick_handle = JOYSTICK_HANDLE_DEFAULT;
static uint32_t last_joystick_poll_time = 0;
#define JOYSTICK_POLL_DELAY_MS 150

void app_init(void)
{
  uint32_t status;

  /* Enable the memory lcd */
  status = sl_board_enable_display();
  EFM_ASSERT(status == SL_STATUS_OK);

  /* Initialize the DMD support for memory lcd display */
  status = DMD_init(0);
  EFM_ASSERT(status == DMD_OK);

  /* Initialize the Joystick driver */
  sl_joystick_init(&sl_joystick_handle);
  sl_joystick_start(&sl_joystick_handle);

  tetris_init();
}

void app_poll_joystick(void)
{
  // Simple debounce/rate limit for joystick
  if (sl_sleeptimer_get_tick_count() - last_joystick_poll_time < sl_sleeptimer_ms_to_tick(JOYSTICK_POLL_DELAY_MS)) {
      return;
  }
  last_joystick_poll_time = sl_sleeptimer_get_tick_count();

  sl_joystick_position_t pos;
  sl_joystick_get_position(&sl_joystick_handle, &pos);

  switch (pos) {
    case JOYSTICK_W:
      tetris_move_left();
      break;
    case JOYSTICK_E:
      tetris_move_right();
      break;
    case JOYSTICK_S:
      tetris_move_down();
      break;
    case JOYSTICK_N:
      tetris_rotate();
      break;
    default:
      break;
  }
}

void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (&BUTTON_INSTANCE_0 == handle) {
      tetris_move_left();
    } else if (&BUTTON_INSTANCE_1 == handle) {
      tetris_move_right();
    }
  }
}