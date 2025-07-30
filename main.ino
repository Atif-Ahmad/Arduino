#include "avr.h"
#include "lcd.h"
#include "game.h"
#include <avr/eeprom.h>

typedef enum { START,
               PLAYING,
               PAUSED,
               GAME_OVER } GameState;
GameState game_state = START;
  unsigned long long frame = 0;


char keypad[4][4] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

char get_key() {
  static char last_key = 0;
  char current_key = 0;

  for (int row = 0; row < 4; row++) {
    SET_BIT(PORTC, 0);
    SET_BIT(PORTC, 1);
    SET_BIT(PORTC, 2);
    SET_BIT(PORTC, 3);
    CLR_BIT(PORTC, row);

    for (int col = 0; col < 4; col++) {
      if (!GET_BIT(PINC, col + 4)) {
        current_key = keypad[row][col];
        break;
      }
    }

    if (current_key) break;
  }

  // Only return key if it’s a new press
  if (current_key && current_key != last_key) {
    last_key = current_key;
    avr_wait(20);  // debounce
    return current_key;
  }

  if (!current_key) {
    last_key = 0;  // reset when key is released
  }

  return 0;
}


void avr_wait(unsigned short msec) {
  TCCR0 = 3;
  while (msec--) {
    TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
    SET_BIT(TIFR, TOV0);
    while (!GET_BIT(TIFR, TOV0))
      ;
  }
  TCCR0 = 0;
}

void setup() {
  DDRB = 1;
  DDRC = 0x0F;
  PORTC = 0xF0;
  lcd_init();
  init_board();
}

#include <avr/eeprom.h>

void save_game_to_eeprom() {
  eeprom_write_byte((uint8_t*)0, score);
  eeprom_write_byte((uint8_t*)1, lives);
  eeprom_write_byte((uint8_t*)2, playerRow);
  eeprom_write_byte((uint8_t*)3, size);

  eeprom_write_byte((uint8_t*)4, (uint8_t)(frame & 0xFF));
  eeprom_write_byte((uint8_t*)5, (uint8_t)((frame >> 8) & 0xFF));
  eeprom_write_byte((uint8_t*)6, (uint8_t)((frame >> 16) & 0xFF));
  eeprom_write_byte((uint8_t*)7, (uint8_t)((frame >> 24) & 0xFF));

  for (int i = 0; i < size; i++) {
    eeprom_write_byte((uint8_t*)(10 + i * 3), obstacles[i].row);
    eeprom_write_byte((uint8_t*)(10 + i * 3 + 1), obstacles[i].col);
    eeprom_write_byte((uint8_t*)(10 + i * 3 + 2), obstacles[i].length);
  }
}

void load_game_from_eeprom() {
  score = eeprom_read_byte((const uint8_t*)0);
  lives = eeprom_read_byte((const uint8_t*)1);
  playerRow = eeprom_read_byte((const uint8_t*)2);
  size = eeprom_read_byte((const uint8_t*)3);

  frame = eeprom_read_byte((const uint8_t*)4);
  frame |= ((unsigned long long)eeprom_read_byte((const uint8_t*)5)) << 8;
  frame |= ((unsigned long long)eeprom_read_byte((const uint8_t*)6)) << 16;
  frame |= ((unsigned long long)eeprom_read_byte((const uint8_t*)7)) << 24;

  for (int i = 0; i < size; i++) {
    Obstacle o;
    o.row = eeprom_read_byte((const uint8_t*)(10 + i * 3));
    o.col = eeprom_read_byte((const uint8_t*)(10 + i * 3 + 1));
    o.length = eeprom_read_byte((const uint8_t*)(10 + i * 3 + 2));
    obstacles[i] = o;

    for (int j = 0; j < o.length; j++) {
      int col = o.col + j;
      if (col >= 0 && col < 14) board[o.row][col] = '+';
    }
  }

  // Redraw player
  board[0][0] = '-';
  board[1][0] = '-';
  board[playerRow][0] = '>';
}


void update() {
  for (int row = 0; row < 2; row++) {
    lcd_pos(row, 0);
    for (int col = 0; col < 14; col++) {
      lcd_put(board[row][col]);
    }
  }

  lcd_pos(0, 14);
  char scoreStr[3];  // Make sure it's large enough
  sprintf(scoreStr, "%2d", score);
  lcd_puts2(scoreStr);

  lcd_pos(1, 14);
  lcd_put('L');
  lcd_put('0' + lives);  // Convert int to char
}

int get_difficulty_for_score(int score) {
  if (score > 30) return 2;
  if (score > 15) return 3;
  if (score > 10) return 5;
  if (score > 5) return 10;
  return 15;
}


int main(void) {
  setup();
  init_board();

  GameState game_state = START;
  int difficulty = 15;

  show_start_screen();

  while (1) {
    char key = get_key();

    switch (game_state) {
      case START:
        if (key == 'A') {
          difficulty = 15;
          game_state = PLAYING;
          lcd_clr();
          update();               // Draw initial state
        } else if (key == '7') {  // Load
          load_game_from_eeprom();
          difficulty = get_difficulty_for_score(score);
          lcd_clr();
          lcd_pos(0, 0);
          lcd_puts2("Loading");
          avr_wait(1000);
          lcd_clr();
          lcd_pos(0, 0);
          lcd_puts2("Loaded");
          avr_wait(1000);
          show_pause_screen();
          game_state = PAUSED;
        }
        break;

      case PLAYING:
        if (key == '5') {
          game_state = PAUSED;
          show_pause_screen();
          break;
        }

        if (key == '6' || key == '9') {
          move_player(key);
        }

        if (frame % difficulty == 0) {
          spawn_obst();
          shift_obst();
        }

        if (score > 5) difficulty = 10;
        if (score > 10) difficulty = 5;
        if (score > 15) difficulty = 3;
        if (score > 30) difficulty = 2;

        update();

        if (hit) {
          hit = false;
          for (int i = 0; i < 5; i++){
            lcd_pos(playerRow, 0);
            lcd_put(' ');
            avr_wait(10);
            lcd_put('>');
          }
        }

        if (lives <= 0) {
          game_state = GAME_OVER;
          show_game_over_screen();
        }

        frame++;
        break;

      case PAUSED:
        if (key == '5') {
          game_state = PLAYING;
          lcd_clr();
          update();
        } else if (key == '8') {  // Quit
          score = 0;
          lives = 3;
          frame = 0;
          lcd_clr();
          clear_board();

          show_start_screen();
          game_state = START;
          for (int i = 0; i < size; i++) {
            Obstacle o = obstacles[i];
            for (int j = 0; j < o.length; j++) {
              int col = o.col + j;
              if (col >= 0 && col < 14) board[o.row][col] = '-';
            }
          }
          size = 0;
        } else if (key == '4') {  // Save
          save_game_to_eeprom();
          lcd_clr();
          lcd_pos(0, 0);
          lcd_puts2("Saving");
          avr_wait(1000);
          lcd_clr();
          lcd_pos(0, 0);
          lcd_puts2("Game Saved");
          avr_wait(1000);
          show_pause_screen();
        }
        break;

      case GAME_OVER:
        if (key == '#') {
          // Reset game state
          clear_board();

          score = 0;
          lives = 3;
          size = 0;
          frame = 0;
          lcd_clr();
          update();
          game_state = PLAYING;
        } else {
          clear_board();
          avr_wait(5000);
          score = 0;
          lives = 3;
          size = 0;
          frame = 0;
          lcd_clr();
          show_start_screen();
          game_state = START;
        }
        break;
    }

    avr_wait(100);
  }
}