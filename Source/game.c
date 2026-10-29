#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include <stdio.h>
#include <stdlib.h>
#include <timer.h>

#define ROWS 20
#define COLS 10
#define BLOCK_SIZE 12 // Size of each square

#define Grey           0xF7DE
#define Blue           0x001F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0
#define DarkGrey			 0x4208

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


static const unsigned short colors[7] = {Grey, Blue, Green, Yellow, Cyan, Magenta, Red};
const int piece_shapes[7][4][2] = {
    { {0,0}, {0,1}, {0,2}, {0,3} }, 	// I
    { {0,0}, {1,0}, {0,1}, {1,1} }, 	// O
    { {0,0}, {-1,0}, {1,0}, {0,1} }, 	// T
    { {0,0}, {0,1}, {0,2}, {-1,2} },	// J
    { {0,0}, {0,1}, {0,2}, {1,2} },		// L
    { {0,0}, {1,0}, {0,1}, {-1,1} }, 	// S
    { {0,0}, {-1,0}, {0,1}, {1,1} } 	// Z
};
extern volatile int field[ROWS][COLS];
extern volatile int flow;
volatile int new_piece[4][2];									
volatile int shape;															// Shape of the piece
volatile int starting_X = 4, starting_Y = 0; 		// Starting position of the piece
extern volatile int high_score;
extern volatile int score;
extern volatile int cleared_lines;

void draw_block(int row, int col, uint16_t color) {
		int i, j;
    
    for (i = 1; i < BLOCK_SIZE - 1; i++) {
        for (j = 1; j < BLOCK_SIZE - 1; j++) {
            LCD_SetPoint(col * BLOCK_SIZE + j, row * BLOCK_SIZE + i, color);
        }
    }		
}

void set_piece(void) {
		shape = rand() % 7;
		int i;
		
		// Copy the standard coordinates based on the shape
		for (i = 0; i < 4; i++) {
        new_piece[i][0] = piece_shapes[shape][i][0] + starting_X;
        new_piece[i][1] = piece_shapes[shape][i][1] + starting_Y;
				// Put the piece on the field
				field[new_piece[i][1]][new_piece[i][0]] = shape + 1;
    }
		
}

void delete_piece(void) {
		int i;
		for (i = 0; i < 4; i++) {
				field[new_piece[i][1]][new_piece[i][0]] = 0;
				draw_block(new_piece[i][1], new_piece[i][0], Black);
		}	
}

void piece_fall(int speed) {
		if (flow==1) {
				int i, j;
				int lines = 0;
			
				// Check for any collision
				for (i = 0; i < 4; i++) {						
						// If collision happens
						if (new_piece[i][1] + 1 >= ROWS || field[new_piece[i][1] + 1][new_piece[i][0]] > 7) {
								__disable_irq();
							
								for (j = 0; j < 4; j++) {
										// The piece is considered placed
										field[new_piece[j][1]][new_piece[j][0]] += 10;
								}			
								
								if (speed == 1) {
										score += 10;
								}
								
								// Check for cleared lines
								for (i = ROWS - 1; i >= 0; i--) {
										if (clear_line(i) == 0) {
												lines++;
												cleared_lines++;
												score += 100;
												shift_down(i);
												i++;
										}
								}
								
								// Update the scores displayed
								update_score_display();
									
								// If 4 lines are cleared --> TETRIS
								if (lines == 4) {
										score += 200;
										GUI_Text(100, 300, (uint8_t *) " TETRIS! ", White, Red);
								}
								
								// Check if it's game over
								game_over();	
								
								// Spawn new piece
								if (flow == 1) {
										set_piece();
								}
								
								__enable_irq();
								return;
						}
				}
				
				// Cancel the piece
				delete_piece();

				// Rewrite the piece in the new position
				for (i = 0; i < 4; i++) {
						new_piece[i][1]++;
						field[new_piece[i][1]][new_piece[i][0]] = shape + 1;	
						draw_block(new_piece[i][1], new_piece[i][0], colors[shape]);
				}	
				
				// SOFT DROP
				if (speed == 2) {
						int i, j;
			
						// Check for any collision
						for (i = 0; i < 4; i++) {						
								// If collision happens
								if (new_piece[i][1] + 1 >= ROWS || field[new_piece[i][1] + 1][new_piece[i][0]] > 7) {
										__disable_irq();
									
										for (j = 0; j < 4; j++) {
												// The piece is considered placed
												field[new_piece[j][1]][new_piece[j][0]] += 10;
										}			
										
										score += 10;
										
										// Check for cleared lines
										for (i = ROWS - 1; i >= 0; i--) {
												if (clear_line(i) == 0) {
														lines++;
														cleared_lines++;
														score += 100;
														shift_down(i);
														i++;
												}
										}
											
										// Update the scores displayed
										update_score_display();
										
										// If 4 lines are cleared --> TETRIS
										if (lines == 4) {
												score += 200;
												GUI_Text(100, 300, (uint8_t *) " TETRIS! ", White, Red);
										}
										
										// Check if it's game over
										game_over();	
										
										// Spawn new piece
										if (flow == 1) {
												set_piece();
										}
										
										__enable_irq();
										return;
								}
						}
						
						// Cancel the piece
						delete_piece();

						// Rewrite the piece in the new position
						for (i = 0; i < 4; i++) {
								new_piece[i][1]++;
								field[new_piece[i][1]][new_piece[i][0]] = shape + 1;	
								draw_block(new_piece[i][1], new_piece[i][0], colors[shape]);
						}
				}	
		}
}

void shift_piece(int side) {
		if (flow==1) {
				__disable_irq();
				int i, j;

				// Check for any collision
				for (i = 0; i < 4; i++) {											
						if (side==0) {
								// Collision to LEFT
								if (new_piece[i][0] - 1 < 0 || field[new_piece[i][1]][new_piece[i][0] - 1] > 7) {
										__enable_irq();
										return;
								}
						}
						else if (side==1) {
								// Collision to RIGHT
								if (new_piece[i][0] + 1 >= COLS || field[new_piece[i][1]][new_piece[i][0] + 1] > 7) {
										__enable_irq();
										return;
								}			
						}
				}
				
				// Cancel the piece
				delete_piece();

				// Rewrite the piece in the new position
				for (i = 0; i < 4; i++) {
						if (side == 0) {
								// Place the new block
								new_piece[i][0]--;
								field[new_piece[i][1]][new_piece[i][0]] = shape + 1;	
								draw_block(new_piece[i][1], new_piece[i][0], colors[shape]);
						}
						else if (side == 1) {
								// Place the new block
								new_piece[i][0]++;
								field[new_piece[i][1]][new_piece[i][0]] = shape + 1;	
								draw_block(new_piece[i][1], new_piece[i][0], colors[shape]);
						}				
				}
				
				// Check if it's game over
				game_over();
				
				__enable_irq();
		}
		
}

void rotate_piece(void) {
    if (flow == 1) {
				__disable_irq();
        int i;
        int temp_piece[4][2];
        int rotation = 1;
			
				int center_x = new_piece[0][0];
				int center_y = new_piece[0][1];

        // Check for any collision
        for (i = 0; i < 4; i++) {
            // Apply rotation
            temp_piece[i][0] = center_x + center_y - new_piece[i][1];
            temp_piece[i][1] = -center_x + center_y + new_piece[i][0];

            // Check for any collision
            if (temp_piece[i][0] < 0 || temp_piece[i][0] >= COLS || temp_piece[i][1] >= ROWS || (temp_piece[i][1] >= 0 && field[temp_piece[i][1]][temp_piece[i][0]] > 7)) {
                rotation = 0;
                break;
            }
        }

        // If possible, update rotation to piece
        if (rotation) {
            // Cancel the piece
            delete_piece();

            // Rewrite the piece in the new position
            for (i = 0; i < 4; i++) {    
								new_piece[i][0] = temp_piece[i][0];
                new_piece[i][1] = temp_piece[i][1];
								field[new_piece[i][1]][new_piece[i][0]] = shape + 1;	
								draw_block(new_piece[i][1], new_piece[i][0], colors[shape]);
            }
        }
				__enable_irq();
    }
}

void hard_drop() {
		if (flow==1) {
				__disable_irq();
				int i, j;
				int lines = 0;
				int deepest_line;
				int max_drop = ROWS - 1;

				// Check for any collision
				for (i = 0; i < 4; i++) {
						for (j = 0; (new_piece[i][1] + j) < ROWS - 1; j++) {
								if (field[new_piece[i][1] + j + 1][new_piece[i][0]] > 7) {
										break;
								}
						}
						if (max_drop > j) {
								max_drop = j;
						}
				}
				
				// Cancel the piece
				delete_piece();

				
				// Rewrite the piece in the new position
				for (i = 0; i < 4; i++) {					
						// Place the new block
						new_piece[i][1] += max_drop;					
						field[new_piece[i][1]][new_piece[i][0]] += 10;	
						draw_block(new_piece[i][1], new_piece[i][0], colors[shape]);
				}
				
				score += 10;
				
				// Check for cleared lines			
				for (i = ROWS - 1; i >= 0; i--) {
						if (clear_line(i) == 0) {
								lines++;
								cleared_lines++;
								score += 100;
								shift_down(i);
								i++;
						}
				}
				
				// Update the scores displayed
				update_score_display();
						
				// If 4 lines are cleared --> TETRIS
				if (lines == 4) {
						score += 200;
						GUI_Text(100, 300, (uint8_t *) " TETRIS! ", White, Red);
				}
				
				// Check if it's game over
				game_over();
				
				if (flow == 1) {
						set_piece();
				}
					
				__enable_irq();				
		}
}

int clear_line(int row) {
		int i, j;
		int clear = 0;
		int defeat = 0;
		
		// Check for any collision
		for (i = 0; i < COLS; i++) {
				if (field[row][i] > 7) {
						clear++;
				}			
		}
		if (clear == COLS) {
				// Clear the completed line
				for (i = 0; i < COLS; i++) {
						field[row][i] = 0;
						draw_block(row, i, Black);
				}
				return 0;
					
		}
		return 1;
}

void shift_down(int row) {
		int i, j;
		// Shift down by one every block above the line cleared
		for (j = row; j > 0; j--) {
				for (i = 0; i < COLS; i++) {
						// Copy down the line above
						int val_above = field[j - 1][i];
						int val_below = field[j][i];
						
						if (val_below != val_above) {	
								field[j][i] = val_above;
								if (val_above > 10) {							
										draw_block(j, i, colors[val_above - 11]);
								}
								else {
										draw_block(j, i, Black);
								}
						}
				}					
		} 
		
		// Delete line above
		for (i = 0; i < COLS; i++) {
				if (field[0][i] != 0) { // Only if it was not empty
            field[0][i] = 0;
            draw_block(0, i, Black);
        }
		}
}

void game_over() {
		int i, j;

		for (i = 0; i < COLS; i++) {
				if (field[0][i] > 7 || field[1][i] > 7) {
						flow = 2;											
						GUI_Text(90, 150, (uint8_t *) " GAME OVER ", White, Red);
				}
		}
		
		if (score > high_score) {
				high_score = score;
		}
		return;
}

void color_field(void) {
		int i=0, j=0;
	
		for (i=0; i<ROWS; i++) {
				for (j=0; j<COLS; j++) {
						if (field[i][j] != 0 && field[i][j] < 7) {
								draw_block(i, j, colors[field[i][j] - 1]);
						}
						else if (11 <= field[i][j] && field[i][j] <= 17) {
								draw_block(i, j, colors[field[i][j] - 11]);
						}
				}
		}
}

void play(void) {
		flow = 1;
		GUI_Text(40, 290, (uint8_t *) "Play", White, Black);
}

void pause(void) {
		flow = 0;
		GUI_Text(40, 290, (uint8_t *) "Pause", White, Black);
}

void init_game_display(void) {
    int r, c;
    for(r = 0; r <= ROWS * BLOCK_SIZE; r++) {
        LCD_SetPoint(0, r, White); // Left bound
        LCD_SetPoint(COLS * BLOCK_SIZE, r, White); // Right bound
    }
    for(c = 0; c <= COLS * BLOCK_SIZE; c++) {
        LCD_SetPoint(c, 0, White); // Upper bound
        LCD_SetPoint(c, ROWS * BLOCK_SIZE, White); // Lower bound
    }
		
		for(r = 1; r < 20; r++) {
				LCD_DrawLine(0, r * BLOCK_SIZE, COLS * BLOCK_SIZE, r * BLOCK_SIZE, DarkGrey);
		}
    for(r = 1; r < 10; r++) {
        LCD_DrawLine(r * BLOCK_SIZE, 0, r * BLOCK_SIZE, ROWS * BLOCK_SIZE, DarkGrey);
    }
		
		for (r = 0; r < ROWS; r++) {
				for (c = 0; c < COLS; c++) {
						field[r][c] = 0;
				}
		}

}

void update_score_display() {
    char punt[20];
    
    sprintf(punt, "%d", score);
    GUI_Text(125, 115, (uint8_t *)punt, White, Black);
    
    sprintf(punt, "%d", cleared_lines);
    GUI_Text(125, 165, (uint8_t *)punt, White, Black);
}

void start(void) {
		char punt[20];
	
		if (high_score <= 0) {
				high_score = 0;
		}
	
		score = 0;
		cleared_lines = 0;
		
		
		LCD_Clear(Black);
		GUI_Text(100, 280, (uint8_t *) " TETRIS ", Red, White);

		sprintf(punt, "%d", high_score);
		GUI_Text(125, 50, (uint8_t *) "High Score: ", White, Black);
		GUI_Text(125, 65,(uint8_t *)punt, White, Black);
	
		sprintf(punt, "%d", score);
		GUI_Text(125, 100, (uint8_t *) "Current Score: ", White, Black);
		GUI_Text(125, 115,(uint8_t *)punt, White, Black);
	
		sprintf(punt, "%d", cleared_lines);
		GUI_Text(125, 150, (uint8_t *) "Lines Cleared: ", White, Black);
		GUI_Text(125, 165,(uint8_t *)punt, White, Black);
	  
    init_game_display();
	
    flow = 0;   
	
    set_piece();
	
		disable_RIT();
		init_RIT(0x002DC6C0);
		enable_RIT();
}