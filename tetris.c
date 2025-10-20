#include "tetris.h"
#include "glib.h"
//#include "memlcd.h"
#include "sl_board_control.h"
#include "sl_sleeptimer.h"
#include <string.h>
#include <stdlib.h>

static GLIB_Context_t glibContext;

static int board[BOARD_WIDTH][BOARD_HEIGHT];
static Tetromino current_tetromino;
static Point current_position;
static bool game_over;

static sl_sleeptimer_timer_handle_t tetris_timer;

static const Tetromino tetrominoes[] = {
    // I
    { { {0, 1}, {1, 1}, {2, 1}, {3, 1} }, 1 },
    // O
    { { {1, 0}, {2, 0}, {1, 1}, {2, 1} }, 2 },
    // T
    { { {1, 0}, {0, 1}, {1, 1}, {2, 1} }, 3 },
    // L
    { { {2, 0}, {0, 1}, {1, 1}, {2, 1} }, 4 },
    // J
    { { {0, 0}, {0, 1}, {1, 1}, {2, 1} }, 5 },
    // S
    { { {1, 0}, {2, 0}, {0, 1}, {1, 1} }, 6 },
    // Z
    { { {0, 0}, {1, 0}, {1, 1}, {2, 1} }, 7 }
};

static void tetris_timer_callback(sl_sleeptimer_timer_handle_t *handle, void *data);
static void spawn_new_tetromino(void);
static bool check_collision(Point pos, Tetromino tet);
static void merge_tetromino(void);
static void clear_lines(void);

void tetris_init(void)
{
    GLIB_contextInit(&glibContext);
  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;
  GLIB_clear(&glibContext);
  GLIB_setFont(&glibContext, (GLIB_Font_t *) &GLIB_FontNormal8x8);

  memset(board, 0, sizeof(board));
  game_over = false;
  spawn_new_tetromino();

  sl_sleeptimer_start_periodic_timer_ms(&tetris_timer,
                                        500,
                                        tetris_timer_callback,
                                        NULL,
                                        0,
                                        0);
}

void tetris_update(void)
{
    if (game_over) {
        return;
    }

    Point next_pos = current_position;
    next_pos.y++;

    if (check_collision(next_pos, current_tetromino)) {
        merge_tetromino();
        clear_lines();
        spawn_new_tetromino();
        if (check_collision(current_position, current_tetromino)) {
            game_over = true;
        }
    } else {
        current_position = next_pos;
    }
    tetris_draw_board();
}

void tetris_move_left(void)
{
    if (game_over) return;
    Point next_pos = current_position;
    next_pos.x--;
    if (!check_collision(next_pos, current_tetromino)) {
        current_position = next_pos;
    }
    tetris_draw_board();
}

void tetris_move_right(void)
{
    if (game_over) return;
    Point next_pos = current_position;
    next_pos.x++;
    if (!check_collision(next_pos, current_tetromino)) {
        current_position = next_pos;
    }
    tetris_draw_board();
}

void tetris_rotate(void)
{
    if (game_over) return;
    Tetromino rotated = current_tetromino;
    for (int i = 0; i < 4; i++) {
        int x = rotated.blocks[i].x;
        rotated.blocks[i].x = 1 - rotated.blocks[i].y;
        rotated.blocks[i].y = x;
    }
    if (!check_collision(current_position, rotated)) {
        current_tetromino = rotated;
    }
    tetris_draw_board();
}

void tetris_draw_board(void)
{
    GLIB_clear(&glibContext);

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
        rect.xMin = x * BLOCK_SIZE + 1;
        rect.yMin = y * BLOCK_SIZE + 1;
        rect.xMax = rect.xMin + BLOCK_SIZE - 1;
        rect.yMax = rect.yMin + BLOCK_SIZE - 1;
        GLIB_drawRectFilled(&glibContext, &rect);
    }

    if (game_over) {
        GLIB_drawString(&glibContext, "GAME OVER", 9, 50, 60, 0);
    }

    DMD_updateDisplay();
}

static void tetris_timer_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
    (void)handle;
    (void)data;
    tetris_update();
}

static void spawn_new_tetromino(void)
{
    current_tetromino = tetrominoes[rand() % (sizeof(tetrominoes) / sizeof(Tetromino))];
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

static void clear_lines(void)
{
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        bool line_full = true;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (board[x][y] == 0) {
                line_full = false;
                break;
            }
        }

        if (line_full) {
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
}
