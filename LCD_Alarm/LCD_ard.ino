#include <LiquidCrystal.h>
#include "TimeLib.h"
#include "pitches.h"

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7); 

int melody[] =  {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
int noteDurations[] = { 4, 8, 8, 4, 4, 4, 4, 4};
bool pressed = false;

int h = 22; //manually set time
int min = 59; //since I don't have access
int sec = 55; //to ethernet shield
int d = 23;
int m = 6;
int y = 2023;
time_t t;
unsigned long previousMillis = 0;
int l1 = 1; //my arduino buttons show 0 when pressed
int c1;         //and 1 when released
int user_h = 0;
int user_m = 0;
unsigned long start = millis();
unsigned long mills = 0;

void setup() {
  unsigned long begin = millis();
  setTime(h, min, sec, d, m, y);
  lcd.begin(16,2);
  Serial.begin(9600);
  pinMode(0, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, OUTPUT);
  lcd.clear();
  
  lcd.println("Left for hour");
  delay(1000);
  lcd.clear();
  lcd.println("Press right when done");
  
  while (digitalRead(0) == 1){
    Serial.println(digitalRead(0));
    if (digitalRead(7) == 0){
      user_h++;
      lcd.clear();
      if (user_h > 23)
        user_h = 0;
      printTime(user_h, 0, 0);
      delay(200);
      
    }
  }
  
  
  lcd.clear();
  lcd.println("Now left for mins"); 
  delay(1000);
  lcd.clear();
  lcd.println("Press right when done");
 
  while (digitalRead(0) == 1){
    if (digitalRead(7) == 0){
      user_m++;
      if (user_m > 60)  
        user_m = 0;
      lcd.clear();
      printTime(user_h, user_m, 0);
      delay(200);
    }
  }
  lcd.clear(); 
  lcd.println("All set");
  delay(1000);
  lcd.clear();
  printTime(user_h, user_m, 0);
  Serial.print(user_h);
  Serial.print(":");
  Serial.println(user_m);
  unsigned long end = millis();
  unsigned long time = end - begin;
  sec +=time/1000;
  lcd.clear();
  lcd.print("Right for snooze");
  delay(3000);
  sec+=3;
  mills = millis() - start;
}

void loop() {
  // put your main code here, to run repeatedly:
  // lcd.setCursor(0,1);
  mills = millis();
  if (mills / 1000 >= 1){
    sec++;
  }
  if (sec == 60){
    min++;
    sec = 0;
  }
  else if (sec > 60){
    min += sec / 60;
    sec = sec % 60;
  }
  if (min == 60){
    h++;
    min = 0;
  }
  else if (min > 60){
    h+= min / 60;
    min = min % 60;
  }
  if (h == 24){
    d++;
    h = 0;
  } else if (h > 24){ 
    d += h / 24;
    h = h % 24;
  }
  lcd.clear();
  setTime(h, min, sec, d, m, y);
  printTime(h, min, sec);
  // Serial.println(user_h == h);
  // Serial.println("----");
  // Serial.println(min == user_m);
  if (user_h == h && min == user_m && pressed == false){
    unsigned long begin = millis();
    digitalWrite(8, HIGH);
    for (int i = 0; i < 8 && !pressed; i++){
      if (digitalRead(0) == 0)
        pressed = true;
      tone(8, melody[i]);
      delay(1000);
      noTone(8);
    }
    unsigned long endd = millis();
    unsigned long time = endd - begin;
    sec += time/1000;
  }
  delay(995);
}

void printTime(int H, int M, int S){
  if (H < 10)
  lcd.print(0);
  lcd.print(H);
  lcd.print(":");
  if (M < 10)
    lcd.print(0);
  lcd.print(M);
  lcd.print(":");
  if (S < 10)
    lcd.print(0);
  lcd.print(S);
}



