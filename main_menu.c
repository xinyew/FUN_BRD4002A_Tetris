#include "main_menu.h"
#include "tetris.h"
#include "glib.h"
#include <stdio.h>
#include <string.h>

// --- Module state ---
static int selected_option = 0;
static int start_level = 1;

static const char* menu_options_no_load[] = {"Start Game", "Adjust Level"};
static const char* menu_options_with_load[] = {"Start Game", "Adjust Level", "Load Game"};
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
    num_menu_options = 3;
  } else {
    menu_options = menu_options_no_load;
    num_menu_options = 2;
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
  draw_background_blocks(pGlib);
  draw_title(pGlib);

  // 2. Draw a semi-transparent overlay for the menu options
  GLIB_Rectangle_t rect = { .xMin = 10, .yMin = 55, .xMax = 118, .yMax = 110 };
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
    } else if (selected_option == 2) { // Load Game
      tetris_load_game();
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
