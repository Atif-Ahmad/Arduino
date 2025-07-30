#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "data.h"


/*
Example board:

> - + + + + - - - - - - + + 0 0
- - - - - - - - + + - - - - L 3 
*/

char board[2][16];
int score = 0;
int lives = 3;
bool hit = false;
int playerRow = 0;  //player starts top left
int recent_rows = 0;
int prev_row = 0;

void init_board() {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 16; j++) {
      board[i][j] = '-';
    }
  }
  board[0][0] = '>';   // Player starts top-left
  board[0][14] = '0';  // Score placeholder
  board[0][15] = '0'; 
  board[1][14] = 'L';  // Lives placeholder
  board[1][15] = '3';  //3 lives
}

void spawn_obst() {
  int min_spaces = rand() % 3;
  for (int i = 0; i < size; i++) {
    if (obstacles[i].col + obstacles[i].length > 13 - min_spaces) {
      return;
    }
  }

  Obstacle o;
  o.row = rand() % 2;
  if (o.row == prev_row) recent_rows++;
  if (recent_rows == 3){
    if (o.row == 0) o.row = 1;
    else o.row = 0;
    recent_rows = 0;
  }
  o.col = 15;
  o.length = 2 + rand() % 4;
  insert_obstacle(o);
}

void shift_obst() {
  for (int i = size - 1; i >= 0; i--) {
    Obstacle o = obstacles[i];
    remove_obstacle(o);

    for (int j = 0; j < o.length; j++) {
      int col = o.col + j;
      if (col >= 0 && col < 14) board[o.row][col] = '-';
    }
    o.col--;

    bool hitPlayer = false;

    if (o.col <= 0 && (o.col + o.length >= 1) && o.row == playerRow) {
      hitPlayer = true;
      lives--;
      hit = true;
    }

    if (!hitPlayer) {
      if (o.col + o.length >= 1) {
        for (int j = 0; j < o.length; j++) {
          int col = o.col + j;
          if (col >= 0 && col < 14) board[o.row][col] = '+';
        }
        insert_obstacle(o);
      } else score++;
    }
  }
}


void show_start_screen() {
  lcd_clr();
  lcd_pos(0, 0);
  lcd_puts2(" Play -> A ");
  lcd_pos(1, 0);
  lcd_puts2(" Load -> 7 ");
}

void show_pause_screen() {
  lcd_clr();
  lcd_pos(0, 0);
  lcd_puts2("Paused    8:Quit");
  lcd_pos(1, 0);
  lcd_puts2("5:Resume  4:Save");
}

void show_game_over_screen() {
  lcd_clr();
  lcd_pos(0, 0);
  lcd_puts2("GAME OVER PLAY:#");

  lcd_pos(1, 0);
  char buffer[16];
  sprintf(buffer, "Score: %d", score);
  lcd_puts2(buffer);
}

void move_player(char key) {
  board[playerRow][0] = '-';

  if (key == '9') playerRow = 0;
  if (key == '6') playerRow = 1;

  if (board[playerRow][0] == '+') {
    lives--;
    hit = true;

    for (int i = 0; i < size; i++) {
      Obstacle o = obstacles[i];
      if (o.row == playerRow && o.col <= 0 && o.col + o.length > 0) {
        for (int j = 0; j < o.length; j++) {
          int col = o.col + j;
          if (col >= 0 && col < 14) board[o.row][col] = '-';
        }
        remove_obstacle(o);
        break;
      }
    }
  }
  board[playerRow][0] = '>';
}


void clear_board() {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 14; j++) {  
      board[i][j] = '-';
    }
  }
  board[playerRow][0] = '>';
}