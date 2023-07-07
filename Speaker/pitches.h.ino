#include "pitches.h"

int[] melody =  {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
int noteDurations[] = { 4, 8, 8, 4, 4, 4, 4, 4};

void setup(){
  for (int i = 0; i < 8; i++{
    int duration = 1000/noteDurations[i];
    tone(8, melody[i]);
    int pause = duration * 1.30;
    delay(pause);
    noTone(8);
  } 
}

void loop(){

}