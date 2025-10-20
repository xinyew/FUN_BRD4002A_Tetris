#include "tetris.h"
#include "glib.h"
#include "sl_sleeptimer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Game State
static game_state_t current_game_state;

// Graphics
static GLIB_Context_t glibContext;

// Game variables
static int board[BOARD_WIDTH][BOARD_HEIGHT];
static Tetromino current_tetromino;
static Tetromino next_tetromino;
static Point current_position;
static int lines_cleared;
static int level;
static int score;

// Timer
static sl_sleeptimer_timer_handle_t tetris_timer;

// --- Local function prototypes ---
static void tetris_timer_callback(sl_sleeptimer_timer_handle_t *handle, void *data);
static Tetromino get_random_tetromino(void);
static void spawn_new_tetromino(void);
static bool check_collision(Point pos, Tetromino tet);
static void merge_tetromino(void);
static void clear_lines(void);
static void tetris_set_game_speed(void);

static const Tetromino tetrominoes[] = {
    // I
    { { {-1, 0}, {0, 0}, {1, 0}, {2, 0} }, 1 },
    // O
    { { {0, 0}, {1, 0}, {0, 1}, {1, 1} }, 2 },
    // T
    { { {-1, 0}, {0, 0}, {1, 0}, {0, 1} }, 3 },
    // L
    { { {1, -1}, {-1, 0}, {0, 0}, {1, 0} }, 4 },
    // J
    { { {-1, -1}, {-1, 0}, {0, 0}, {1, 0} }, 5 },
    // S
    { { {0, 0}, {1, 0}, {-1, 1}, {0, 1} }, 6 },
    // Z
    { { {-1, 0}, {0, 0}, {0, 1}, {1, 1} }, 7 }
};

// --- Public functions ---

void tetris_init(void)
{
  GLIB_contextInit(&glibContext);
  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;
    GLIB_setFont(&glibContext, (GLIB_Font_t *) &GLIB_FontNarrow6x8);
    current_game_state = GAME_STATE_MAIN_MENU;
  }
  
  void tetris_start_new_game(int starting_level)
  {
    memset(board, 0, sizeof(board));
    lines_cleared = 0;
    level = starting_level;
    score = 0;
  
    next_tetromino = get_random_tetromino();
    spawn_new_tetromino();
  
    tetris_set_game_speed(); // Set initial speed based on level
  
    current_game_state = GAME_STATE_IN_GAME;
  }
  
  void tetris_update(void)
  {
      if (current_game_state != GAME_STATE_IN_GAME) {
          return;
      }
  
      Point next_pos = current_position;
      next_pos.y++;
  
      if (check_collision(next_pos, current_tetromino)) {
          merge_tetromino();
          clear_lines();
          spawn_new_tetromino();
          if (check_collision(current_position, current_tetromino)) {
              sl_sleeptimer_stop_timer(&tetris_timer);
              current_game_state = GAME_STATE_GAME_OVER;
          }
      } else {
          current_position = next_pos;
      }
      tetris_draw_board();
  }
  
  void tetris_draw_board(void)
  {
      GLIB_clear(&glibContext);
  
      if (current_game_state == GAME_STATE_GAME_OVER) {
          char* game_over_text = "GAME OVER";
          int text_x = (glibContext.pDisplayGeometry->xSize - (strlen(game_over_text) * 6)) / 2;
          GLIB_drawString(&glibContext, game_over_text, strlen(game_over_text), text_x, 40, 0);
  
          char score_buffer[16];
          snprintf(score_buffer, sizeof(score_buffer), "Score: %d", score);
          text_x = (glibContext.pDisplayGeometry->xSize - (strlen(score_buffer) * 6)) / 2;
          GLIB_drawString(&glibContext, score_buffer, strlen(score_buffer), text_x, 60, 0);
  
          char* restart_text = "Press BTN1";
          text_x = (glibContext.pDisplayGeometry->xSize - (strlen(restart_text) * 6)) / 2;
          GLIB_drawString(&glibContext, restart_text, strlen(restart_text), text_x, 80, 0);
          DMD_updateDisplay();
          return;
      }
    // Draw board border
    GLIB_Rectangle_t rect = { .xMin = 0, .yMin = 0, .xMax = BOARD_WIDTH * BLOCK_SIZE + 1, .yMax = BOARD_HEIGHT * BLOCK_SIZE + 1 };
    GLIB_drawRect(&glibContext, &rect);

    // Draw settled blocks
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (board[x][y]) {
                rect.xMin = x * BLOCK_SIZE + 1;
                rect.yMin = y * BLOCK_SIZE + 1;
                rect.xMax = rect.xMin + BLOCK_SIZE - 1;
                rect.yMax = rect.yMin + BLOCK_SIZE - 1;
                GLIB_drawRectFilled(&glibContext, &rect);
            }
        }
    }

    // Draw current tetromino
    for (int i = 0; i < 4; i++) {
        int x = current_position.x + current_tetromino.blocks[i].x;
        int y = current_position.y + current_tetromino.blocks[i].y;
        if (y >= 0) {
          rect.xMin = x * BLOCK_SIZE + 1;
          rect.yMin = y * BLOCK_SIZE + 1;
          rect.xMax = rect.xMin + BLOCK_SIZE - 1;
          rect.yMax = rect.yMin + BLOCK_SIZE - 1;
          GLIB_drawRectFilled(&glibContext, &rect);
        }
    }

    // --- Draw Side Panel ---
    char text_buffer[10];
    int right_panel_x = (BOARD_WIDTH * BLOCK_SIZE) + 10;

    // Score
    GLIB_drawString(&glibContext, "Score", 5, right_panel_x, 10, 0);
    snprintf(text_buffer, sizeof(text_buffer), "%d", score);
    GLIB_drawString(&glibContext, text_buffer, strlen(text_buffer), right_panel_x, 20, 0);

    // Lines
    GLIB_drawString(&glibContext, "Lines", 5, right_panel_x, 40, 0);
    snprintf(text_buffer, sizeof(text_buffer), "%d", lines_cleared);
    GLIB_drawString(&glibContext, text_buffer, strlen(text_buffer), right_panel_x, 50, 0);

    // Level
    GLIB_drawString(&glibContext, "Level", 5, right_panel_x, 70, 0);
    snprintf(text_buffer, sizeof(text_buffer), "%d", level);
    GLIB_drawString(&glibContext, text_buffer, strlen(text_buffer), right_panel_x, 80, 0);

    // Next Piece
    GLIB_drawString(&glibContext, "Next", 4, right_panel_x, 100, 0);
    for (int i = 0; i < 4; i++) {
        int x = right_panel_x + 10 + (next_tetromino.blocks[i].x * BLOCK_SIZE);
        int y = 110 + (next_tetromino.blocks[i].y * BLOCK_SIZE);
        rect.xMin = x;
        rect.yMin = y;
        rect.xMax = x + BLOCK_SIZE - 1;
        rect.yMax = y + BLOCK_SIZE - 1;
        GLIB_drawRectFilled(&glibContext, &rect);
    }

    DMD_updateDisplay();
}

// --- State Management Functions ---
game_state_t tetris_get_game_state(void)
{
  return current_game_state;
}

void tetris_set_game_state(game_state_t new_state)
{
  current_game_state = new_state;
}

GLIB_Context_t* tetris_get_glib_context(void)
{
  return &glibContext;
}

// --- Game Logic Functions ---

void tetris_move_left(void)
{
    if (current_game_state != GAME_STATE_IN_GAME) return;
    Point next_pos = current_position;
    next_pos.x--;
    if (!check_collision(next_pos, current_tetromino)) {
        current_position = next_pos;
    }
    tetris_draw_board();
}

void tetris_move_right(void)
{
    if (current_game_state != GAME_STATE_IN_GAME) return;
    Point next_pos = current_position;
    next_pos.x++;
    if (!check_collision(next_pos, current_tetromino)) {
        current_position = next_pos;
    }
    tetris_draw_board();
}

void tetris_move_down(void)
{
  if (current_game_state != GAME_STATE_IN_GAME) return;
  tetris_update();
}

void tetris_rotate(void)
{
    if (current_game_state != GAME_STATE_IN_GAME || current_tetromino.color == 2) { // Do not rotate 'O' piece
      return;
    }

    Tetromino rotated = current_tetromino;
    for (int i = 0; i < 4; i++) {
        int x = rotated.blocks[i].x;
        rotated.blocks[i].x = -rotated.blocks[i].y;
        rotated.blocks[i].y = x;
    }

    if (!check_collision(current_position, rotated)) {
        current_tetromino = rotated;
    }
    tetris_draw_board();
}

// --- Internal Helper Functions ---

static void tetris_timer_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
    (void)handle;
    (void)data;
    tetris_update();
}

static Tetromino get_random_tetromino(void)
{
    return tetrominoes[rand() % (sizeof(tetrominoes) / sizeof(Tetromino))];
}

static void spawn_new_tetromino(void)
{
    current_tetromino = next_tetromino;
    next_tetromino = get_random_tetromino();
    current_position.x = BOARD_WIDTH / 2 - 1;
    current_position.y = 0;
}

static bool check_collision(Point pos, Tetromino tet)
{
    for (int i = 0; i < 4; i++) {
        int x = pos.x + tet.blocks[i].x;
        int y = pos.y + tet.blocks[i].y;

        if (x < 0 || x >= BOARD_WIDTH || y >= BOARD_HEIGHT) {
            return true;
        }

        if (y >= 0 && board[x][y]) {
            return true;
        }
    }
    return false;
}

static void merge_tetromino(void)
{
    for (int i = 0; i < 4; i++) {
        int x = current_position.x + current_tetromino.blocks[i].x;
        int y = current_position.y + current_tetromino.blocks[i].y;
        if (y >= 0) {
            board[x][y] = current_tetromino.color;
        }
    }
}

static void tetris_set_game_speed(void)
{
  int new_speed = 500 - ((level - 1) * 50);
  if (new_speed < 50) {
    new_speed = 50;
  }
  sl_sleeptimer_stop_timer(&tetris_timer);
  sl_sleeptimer_start_periodic_timer_ms(&tetris_timer,
                                        new_speed,
                                        tetris_timer_callback,
                                        NULL,
                                        0,
                                        0);
}

static void clear_lines(void)
{
    int num_cleared_lines = 0;
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        bool line_full = true;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (board[x][y] == 0) {
                line_full = false;
                break;
            }
        }

        if (line_full) {
            num_cleared_lines++;
            for (int k = y; k > 0; k--) {
                for (int x = 0; x < BOARD_WIDTH; x++) {
                    board[x][k] = board[x][k - 1];
                }
            }
            for (int x = 0; x < BOARD_WIDTH; x++) {
                board[x][0] = 0;
            }
            y++; // Check the same line again
        }
    }
    if (num_cleared_lines > 0) {
      lines_cleared += num_cleared_lines;
      switch (num_cleared_lines) {
        case 1:
          score += 100 * level;
          break;
        case 2:
          score += 300 * level;
          break;
        case 3:
          score += 500 * level;
          break;
        case 4:
          score += 800 * level;
          break;
      }
      int new_level = (lines_cleared / 10) + 1;
      if (new_level > level) {
        level = new_level;
        tetris_set_game_speed();
      }
    }
}