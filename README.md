Schematic:
![alt text](https://github.com/Atif-Ahmad/Arduino/blob/acd2c956b93e17cbe44359e6686bafc744fe3d8c/Schematic.png)

Example board:
> - + + + + - - - - - - + + 0 0
- - - - - - - - + + - - - - L 3


I decided to implement a simple obstacle avoidance game. The input would be the 4x4 keypad. The output/display would be the 16x2 LCD. The features of my game included the gameplay, start/pause/load screens, and the ability to save and load your game in different states using the atmega's EEPROM. The game idea is very simple. The player is placed on the leftmost column and can only move up or down. Obstacles vary in length from 2 to 5 characters. The speed is slow at first, but as the player scores more points the speed increases. The player has 3 lives, and when they collide with an obstacle they lose health until game over. Due to the limited size of the LCD the max score is 99. The game is played on a 16x2 LCD.

To implement obstacles, I first created an Obstacle struct which contained information like location and length. I then had a global array of obstacles (with a max size of 10, although max only 4-5 obstacles could fit on the board). To generate an obstacle, I made sure there were enough clear spaces so that when the obstacles came towards the player, they were able to avoid them. I randomly generated the obstacle’s row and length (ranging from 2+ to 5+). Once generated, I added them to the global obstacle container. Thus, shifting was just a matter of going through the obstacle array, with appropriate collision detection. Once obstacle left the screen, I removed it from the obstacle array.

To implement different game screens, I made a global enum game_state that had the values START, PLAYING, PAUSED, and GAME_OVER. Then in the main loop, I used a switch statement like switch(game_state) with case START and others.

To implement the save and load feature, I used the atmega32’s EEPROM. EEPROM is non volatile memory that can only be programmed using a specific voltage. The atmega32 can store 1024 bytes of non volatile memory, which was enough to save the state of my game. I saved the following variables: the frame (determined the rate of obstacle generation and speed of shift), the score, the player’s position (an integer for their row), the size of the obstacle container, and each of the obstacles in the obstacle container. This is all I need to properly run the game at any given instance. The load_game_from_eeprom() function was very similar, except instead of writing bytes I was reading bytes.

```c
void save_game_to_eeprom() {
eeprom_write_byte((uint8_t*)0, score);
eeprom_write_byte((uint8_t*)1, lives);
eeprom_write_byte((uint8_t*)2, playerRow);
eeprom_write_byte((uint8_t*)3, size);
eeprom_write_byte((uint8_t*)4, (uint8_t)(frame & 0xFF));
eeprom_write_byte((uint8_t*)5, (uint8_t)((frame >> 8) & 0xFF));
eeprom_write_byte((uint8_t*)6, (uint8_t)((frame >> 16) & 0xFF));
eeprom_write_byte((uint8_t*)7, (uint8_t)((frame >> 24) & 0xFF));
  for (int i = 0; i < size; i++) { //saving all the obstacles on the screen
   eeprom_write_byte((uint8_t*)(10 + i * 3), obstacles[i].row);
   eeprom_write_byte((uint8_t*)(10 + i * 3 + 1), obstacles[i].col);
   eeprom_write_byte((uint8_t*)(10 + i * 3 + 2), obstacles[i].length);
  }
}
```

The main method tied everything together. The game_state is at START. Every time I’m reading keys using the get_key() method. The player moves, pausing, resuming, saving, loading are all controlled by the user. There is a minimum 100ms wait in each iteration. The frame is integer based instead of timer based to determine the speed according to the score.

