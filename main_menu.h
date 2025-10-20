#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "sl_joystick.h"
#include "sl_simple_button.h"

void main_menu_init(void);
void main_menu_draw(void);
void main_menu_handle_input(sl_joystick_position_t joystick_pos, const sl_button_t *button_handle);
int main_menu_get_start_level(void);

void slot_menu_init(void);
void slot_menu_draw(void);
void slot_menu_handle_input(sl_joystick_position_t joystick_pos, const sl_button_t *button_handle);

void scoreboard_draw(void);
void scoreboard_handle_input(sl_joystick_position_t joystick_pos, const sl_button_t *button_handle);

#endif // MAIN_MENU_H
