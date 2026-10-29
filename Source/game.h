#ifndef __GAME_H
#define __GAME_H

extern volatile int flow;
extern volatile int started;

extern void draw_block( int row, int col, uint16_t color);
extern void set_piece(void);
extern void delete_piece();
extern void piece_fall(int speed);
extern void shift_piece(int side);
extern void rotate_piece(void);
extern void hard_drop();
extern int clear_line(int row);
extern void shift_down(int row);
extern void color_field(void);
extern void init_game_display(void);
extern void update_score_display(void);
extern void play(void);
extern void pause(void);
extern void start();
extern void game_over();

#endif