#include "pitches.h"
int myNumber;
int c = 5;


int melody[] =  {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
int noteDurations[] = { 4, 8, 8, 4, 4, 4, 4, 4};

void setup(){
  pinMode(8, OUTPUT);
  pinMode(7, INPUT_PULLUP);
}

void loop(){
  if (digitalRead(7) == 0){
    digitalWrite(8, HIGH);
    for (int i = 0; i < 8; i++){
      tone(8, melody[i]);
      delay(1000);
      noTone(8);
    } 
  }
}