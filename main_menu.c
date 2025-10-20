#include "main_menu.h"
#include "tetris.h"
#include "glib.h"
#include "sl_simple_button_instances.h"
#include <stdio.h>
#include <string.h>

// --- Module state ---
static int selected_option = 0;
static int start_level = 1;

static const char* menu_options_no_load[] = {"Start Game", "Adjust Level", "Scoreboard"};
static const char* menu_options_with_load[] = {"Start Game", "Adjust Level", "Load Game", "Scoreboard"};
static const char** menu_options;
static int num_menu_options;


// --- Local Functions ---
static void draw_title(GLIB_Context_t *pGlib);
static void draw_background_blocks(GLIB_Context_t *pGlib);

// --- Public Functions ---

void main_menu_init(void)
{
  selected_option = 0;
  start_level = 1;
  if (tetris_has_saved_game()) {
    menu_options = menu_options_with_load;
    num_menu_options = 4;
  } else {
    menu_options = menu_options_no_load;
    num_menu_options = 3;
  }
}

int main_menu_get_start_level(void)
{
  return start_level;
}

void main_menu_draw(void)
{
  GLIB_Context_t *pGlib = tetris_get_glib_context();
  GLIB_clear(pGlib);

  // 1. Draw the decorative background
  // draw_background_blocks(pGlib);
  draw_title(pGlib);

  // 2. Draw a semi-transparent overlay for the menu options
  GLIB_Rectangle_t rect = { .xMin = 10, .yMin = 55, .xMax = 118, .yMax = 125 };
  pGlib->foregroundColor = White;
  GLIB_drawRectFilled(pGlib, &rect);
  pGlib->foregroundColor = Black;
  pGlib->backgroundColor = White;

  // 3. Draw Menu Options
  for (int i = 0; i < num_menu_options; i++) {
    int option_y = 60 + (i * 15);
    const char* option_text = menu_options[i];
    int text_x = (pGlib->pDisplayGeometry->xSize - (strlen(option_text) * 6)) / 2;

    if (i == selected_option) {
      GLIB_drawString(pGlib, ">", 1, text_x - 10, option_y, 0);
    }
    GLIB_drawString(pGlib, option_text, strlen(option_text), text_x, option_y, 0);

    if (i == 1) { // If it's the "Adjust Level" option
      char level_buffer[8];
      snprintf(level_buffer, sizeof(level_buffer), "<%d>", start_level);
      GLIB_drawString(pGlib, level_buffer, strlen(level_buffer), text_x + (strlen(option_text) * 6) + 6, option_y, 0);
    }
  }

  DMD_updateDisplay();
  pGlib->backgroundColor = White; // Reset for other parts of the app
}

void main_menu_handle_input(sl_joystick_position_t joystick_pos, const sl_button_t *button_handle)
{
  if (joystick_pos == JOYSTICK_S) { // Down
    selected_option = (selected_option + 1) % num_menu_options;
  } else if (joystick_pos == JOYSTICK_N) { // Up
    selected_option = (selected_option - 1 + num_menu_options) % num_menu_options;
  }

  if (selected_option == 1) { // Adjust Level
    if (joystick_pos == JOYSTICK_E) { // Right
      start_level++;
      if (start_level > 10) {
        start_level = 1;
      }
    } else if (joystick_pos == JOYSTICK_W) { // Left
      start_level--;
      if (start_level < 1) {
        start_level = 10;
      }
    }
  }

  if (joystick_pos == JOYSTICK_C) { // Center click
    if (selected_option == 0) { // Start Game
      tetris_start_new_game(start_level);
    } else if (strcmp(menu_options[selected_option], "Load Game") == 0) {
      tetris_set_game_state(GAME_STATE_SLOT_SELECTION);
    } else if (strcmp(menu_options[selected_option], "Scoreboard") == 0) {
        tetris_set_game_state(GAME_STATE_SCOREBOARD);
    }
  }

  (void)button_handle; // Suppress unused parameter warning
}

// --- Pixel Art Title (Bitmap-based) ---
#define FONT_BLOCK_SIZE 4
#define FONT_LETTER_HEIGHT 5

static const char FONT_MAP_T[] = {1,1,1, 0,1,0, 0,1,0, 0,1,0, 0,1,0};
static const char FONT_MAP_E[] = {1,1,1, 1,0,0, 1,1,0, 1,0,0, 1,1,1};
static const char FONT_MAP_R[] = {1,1,0, 1,0,1, 1,1,0, 1,0,1, 1,0,1};
static const char FONT_MAP_I[] = {1, 1, 1, 1, 1};
static const char FONT_MAP_S[] = {1,1,1, 1,0,0, 1,1,1, 0,0,1, 1,1,1};

static void draw_letter_from_map(GLIB_Context_t *pGlib, int x_start, int y_start, const char *map, int width, int height)
{
  GLIB_Rectangle_t rect;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if (map[y * width + x]) {
        rect.xMin = x_start + x * FONT_BLOCK_SIZE;
        rect.yMin = y_start + y * FONT_BLOCK_SIZE;
        rect.xMax = rect.xMin + FONT_BLOCK_SIZE - 1;
        rect.yMax = rect.yMin + FONT_BLOCK_SIZE - 1;
        GLIB_drawRectFilled(pGlib, &rect);
      }
    }
  }
}

static void draw_title(GLIB_Context_t *pGlib)
{
    int top = 12;
    int left = (128 - (3*FONT_BLOCK_SIZE + 2 + 3*FONT_BLOCK_SIZE + 2 + 3*FONT_BLOCK_SIZE + 2 + 3*FONT_BLOCK_SIZE + 2 + 1*FONT_BLOCK_SIZE + 2 + 3*FONT_BLOCK_SIZE)) / 2;

    draw_letter_from_map(pGlib, left, top, FONT_MAP_T, 3, FONT_LETTER_HEIGHT); left += 3*FONT_BLOCK_SIZE + 2;
    draw_letter_from_map(pGlib, left, top, FONT_MAP_E, 3, FONT_LETTER_HEIGHT); left += 3*FONT_BLOCK_SIZE + 2;
    draw_letter_from_map(pGlib, left, top, FONT_MAP_T, 3, FONT_LETTER_HEIGHT); left += 3*FONT_BLOCK_SIZE + 2;
    draw_letter_from_map(pGlib, left, top, FONT_MAP_R, 3, FONT_LETTER_HEIGHT); left += 3*FONT_BLOCK_SIZE + 2;
    draw_letter_from_map(pGlib, left, top, FONT_MAP_I, 1, FONT_LETTER_HEIGHT); left += 1*FONT_BLOCK_SIZE + 2;
    draw_letter_from_map(pGlib, left, top, FONT_MAP_S, 3, FONT_LETTER_HEIGHT);
}

static void draw_background_blocks(GLIB_Context_t *pGlib)
{
    GLIB_Rectangle_t rect;
    int bs = BLOCK_SIZE;
    rect = (GLIB_Rectangle_t){0, 0, BOARD_WIDTH*bs+1, BOARD_HEIGHT*bs+1}; 
    GLIB_drawRect(pGlib, &rect);

    // Draw some fake blocks
    const Point fake_blocks[] = {
        {1,20},{2,20},{3,20},
        {7,20},{8,20},
        {1,19},{2,19},
        {5,19},{6,19},{7,19},
        {3,18},{4,18},{5,18},
        {8,18},{8,17}
    };
    for (unsigned int i = 0; i < sizeof(fake_blocks)/sizeof(Point); i++) {
        rect = (GLIB_Rectangle_t){
            fake_blocks[i].x * bs + 1,
            fake_blocks[i].y * bs + 1,
            fake_blocks[i].x * bs + 1 + bs - 1,
            fake_blocks[i].y * bs + 1 + bs - 1
        };
        GLIB_drawRectFilled(pGlib, &rect);
    }
}


// --- Slot Menu ---

static int selected_slot = 0;

void slot_menu_init(void)
{
  selected_slot = 0;
}

void slot_menu_draw(void)
{
  GLIB_Context_t *pGlib = tetris_get_glib_context();
  GLIB_clear(pGlib);

  // Title
  char* title_text = "Load Game";
  int text_x = (pGlib->pDisplayGeometry->xSize - (strlen(title_text) * 6)) / 2;
  GLIB_drawString(pGlib, title_text, strlen(title_text), text_x, 10, 0);

  // Draw slots
  for (int i = 0; i < 5; i++) {
    int option_y = 30 + (i * 15);
    char slot_name[32];
    tetris_get_slot_name(i, slot_name, sizeof(slot_name));

    if (i == selected_slot) {
      GLIB_drawString(pGlib, ">", 1, 10, option_y, 0);
    }
    GLIB_drawString(pGlib, slot_name, strlen(slot_name), 20, option_y, 0);
  }

  // Button hints
  char* hint_text = "BTN1:BACK BTN0:DEL";
  text_x = (pGlib->pDisplayGeometry->xSize - (strlen(hint_text) * 6)) / 2;
  GLIB_drawString(pGlib, hint_text, strlen(hint_text), text_x, 120, 0);

  DMD_updateDisplay();
}

void slot_menu_handle_input(sl_joystick_position_t joystick_pos, const sl_button_t *button_handle)
{
  if (joystick_pos == JOYSTICK_S) { // Down
    selected_slot = (selected_slot + 1) % 5;
  } else if (joystick_pos == JOYSTICK_N) { // Up
    selected_slot = (selected_slot - 1 + 5) % 5;
  }

  if (joystick_pos == JOYSTICK_C) { // Center click to load
    tetris_load_from_slot(selected_slot);
  }

  if (button_handle == &sl_button_btn1) { // Back to main menu
    tetris_set_game_state(GAME_STATE_MAIN_MENU);
  }

  if (button_handle == &sl_button_btn0) { // Delete slot
    tetris_delete_slot(selected_slot);
  }
}

// --- Scoreboard ---

void scoreboard_draw(void)
{
    GLIB_Context_t *pGlib = tetris_get_glib_context();
    GLIB_clear(pGlib);

    // Title
    char* title_text = "Scoreboard";
    int text_x = (pGlib->pDisplayGeometry->xSize - (strlen(title_text) * 6)) / 2;
    GLIB_drawString(pGlib, title_text, strlen(title_text), text_x, 10, 0);

    // Draw scores
    uint32_t high_scores[5];
    tetris_get_high_scores(high_scores);
    for (int i = 0; i < 5; i++) {
        int option_y = 30 + (i * 15);
        char score_buffer[16];
        snprintf(score_buffer, sizeof(score_buffer), "%d. %lu", i + 1, high_scores[i]);
        GLIB_drawString(pGlib, score_buffer, strlen(score_buffer), 20, option_y, 0);
    }

    // Button hints
    char* hint_text = "BTN1: BACK";
    text_x = (pGlib->pDisplayGeometry->xSize - (strlen(hint_text) * 6)) / 2;
    GLIB_drawString(pGlib, hint_text, strlen(hint_text), text_x, 110, 0);

    DMD_updateDisplay();
}

void scoreboard_handle_input(sl_joystick_position_t joystick_pos, const sl_button_t *button_handle)
{
    (void)joystick_pos;
    if (button_handle == &sl_button_btn1) { // Back to main menu
        tetris_set_game_state(GAME_STATE_MAIN_MENU);
    }
}