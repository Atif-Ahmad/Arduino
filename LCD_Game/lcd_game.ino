#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int score = 0;
int high_score = 0;
unsigned long myTime; //keeping track of how many milliseconds elapsed

String array[2][16];

int row = 0;  //row for first obstacle
int row2 = 0; //row for second obstacle
int p_row = 0; //row for player
int counter = 3; //index counter when obstacle disappears

bool game_over = false;
bool start = false;
bool paused = false;
bool firstActive = false; //whether first obstacle is on board
bool secondActive = false; //whether second obstacle is on board

//button pins and buzzer pin
const int first = A0; 
const int second = A1;
const int third = A2;
const int fourth = A3;
const int buzzpin = A5;

int difficulty = 1024; //delay for shifting obstacles

//initialize board
void begin() { 
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 16; ++j)
      array[i][j] = "-";
  }
  array[0][0] = ">";
}

//playing initial tone before game startts
void tone(int dt, int duration){
  for (int i = 1; i <= duration; ++i) {
    digitalWrite(buzzpin, HIGH);
    delay(dt);
    digitalWrite(buzzpin, LOW);
    delay(dt);
    check_button();
    if (start) return;
  }
}

//for in game sfx (obstacle clearing)
void spec_tone(int dt, int duration){
  for (int i = 1; i <= duration; ++i) {
    digitalWrite(buzzpin, HIGH);
    delay(dt);
    digitalWrite(buzzpin, LOW);
    delay(dt);
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(first, INPUT_PULLUP);
  pinMode(second, INPUT_PULLUP);
  pinMode(third, INPUT_PULLUP);
  pinMode(fourth, INPUT_PULLUP);
  pinMode(buzzpin, OUTPUT);
  lcd.begin(16, 2);
  begin();
}

//updating LCD with contents
void print() {
  for (int j = 0; j < 2; ++j) {
    for (int i = 0; i < 14; ++i) {
      lcd.setCursor(i, j);
      lcd.print(array[j][i]);
    }
  }

  lcd.setCursor(14, 0);
  lcd.print(score);

  if (score > 10) {
    lcd.setCursor(15, 0);
    lcd.print(score % 10);
  }
}

//generates both obstacles if conditions are met
void generate_obst() {

  int c = 0;
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 7; ++j) {
      if (array[i][13 - j] != "+")  c++;
      else break;
    }
  }

  //first obstacle generates if the first 7 spaces are open on both rows
  if (c == 14 && !firstActive) {
    row = random(0, 2);
    array[row][13] = "+";
    firstActive = true;
  }

  c = 0; 
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (array[i][13 - j] != "+")  c++; 
      else break;
    }
  }

  //second obstacle generates if the first 4 spaces are open on both rows
  if (c == 8 && !secondActive) {
    if (row == 0) row2 = 1;
    if (row == 1) row2 = 0;
    array[row2][13] = "+";
    secondActive = true;
  }
}

//shifts obstacle by finding the location of the leftmost + and moves it by one spot
void shift(int indicator) {
  int input_row;
  if (indicator == 0) input_row = row;
  if (indicator == 1) input_row = row2;
  int loc = 0;
  for (int i = 0; i < 14; ++i) {
    if (array[input_row][i] == "-" || array[input_row][i] == ">") loc++;
    else break;
  }

  if (loc == 14); //no + means obstacle wasn't generated
  else if (loc > 10) {
    array[input_row][loc - 1] = "+";
  } else if (loc == 0) {
    //counter used to shift obstacle out of the screen
    array[input_row][loc + counter] = "-";
    counter--;
    if (counter == -1 && !game_over) {
      counter = 3;
      score++; //incrementing score every time obstacle disappears
      spec_tone(2, 25);
      high_score = max(score, high_score); 
      if (indicator == 0) firstActive = false;
      if (indicator == 1) secondActive = false;
    }
  } else {
    array[input_row][loc + 3] = "-";
    array[input_row][loc - 1] = "+";
  }
  game_over = !(array[0][0] == ">" || array[1][0] == ">");
}

//function to move player character up or down
void shiftPos(int secondState, int thirdState) {
  array[p_row][0] = "-";
  if (p_row == 0 && thirdState == 0) {
    p_row++;
  } else if (p_row == 1 && secondState == 0) {
    p_row--;
  }
  if (array[p_row][0] != "+") array[p_row][0] = ">";
  else game_over = true;
}

//record button input and take appropriate action
void check_button() {
  int firstState = digitalRead(first); //leftmost button
  int secondState = digitalRead(second); //second buttton
  int thirdState = digitalRead(third); //third button
  int fourthState = digitalRead(fourth); //rightmost button

  if (firstState == 0) {
    start = true;
    game_over = false; 
  } else if (start && (secondState == 0 || thirdState == 0)) shiftPos(secondState, thirdState);
  else if (fourthState == 0) {
    paused = true;
  }
}


void loop() {
  // put your main code here, to run repeatedly:

  if (!start) {
    lcd.setCursor(0, 0);
    lcd.print("Left B : Start");
    check_button();

    tone(2, 100);
    tone(3, 100);
    tone(1, 100);
    tone(4, 100);
    tone(1, 100);
  }

  if (start && !game_over && !paused) {
    print(); //constantly update board
    check_button(); //constantly check user input
    myTime = millis(); //checking how much time elapsed

    // setting the speed of obstacles moving
    if (score > 5) difficulty = 512;
    if (score > 10) difficulty = 256;
    if (score > 15) difficulty = 128;

    //calling the generate & shift obstacles
    if (myTime % difficulty <= 24) {
      generate_obst();
      shift(0);
      shift(1);
    }
  } 

  //game over screen
  else if (start && !paused) {
    start = false;
    array[p_row][0] = "X";
    print();
    tone(40, 50);
    lcd.clear();
    lcd.print("Score :");
    lcd.setCursor(7, 0);
    lcd.print(score);
    lcd.setCursor(0, 1);
    lcd.print("High Score :");
    lcd.print(high_score);
    lcd.setCursor(12, 1);
    score = 0;
    begin();
    delay(3000);
    lcd.clear();
    firstActive = false;
    secondActive = false;
    difficulty = 1024;
  }

  //pause screen
  if (start && paused) {
    lcd.clear();
    while (paused) {
      lcd.setCursor(0, 0);
      lcd.print("Left B: Resume");
      if (digitalRead(first) == 0) paused = false;
    }
  }
}