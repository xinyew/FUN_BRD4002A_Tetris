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
void tetris_pause_game(void);
void tetris_resume_game(void);
void tetris_save_game(void);
void tetris_load_from_slot(int slot_index);
void tetris_delete_slot(int slot_index);
void tetris_get_slot_name(int slot_index, char* buffer, size_t buffer_size);
bool tetris_has_saved_game(void);

void tetris_get_high_scores(uint32_t scores[5]);
void tetris_add_high_score(uint32_t score);
bool tetris_is_high_score(uint32_t score);
void tetris_process_action(void);
GLIB_Context_t* tetris_get_glib_context(void);

#endif // TETRIS_H
