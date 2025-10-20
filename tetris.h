#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>

#include "game_state.h"
#include "glib.h"

#define BOARD_WIDTH   10
#define BOARD_HEIGHT  21
#define BLOCK_SIZE    6

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point blocks[4];
    int color;
} Tetromino;

void tetris_init(void);
void tetris_update(void);
void tetris_move_left(void);
void tetris_move_right(void);
void tetris_move_down(void);
void tetris_rotate(void);
void tetris_draw_board(void);

// New state management functions
game_state_t tetris_get_game_state(void);
void tetris_set_game_state(game_state_t new_state);
void tetris_start_new_game(int starting_level);
GLIB_Context_t* tetris_get_glib_context(void);

#endif // TETRIS_H
