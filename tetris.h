#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>

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
void tetris_rotate(void);
void tetris_draw_board(void);

#endif // TETRIS_H
