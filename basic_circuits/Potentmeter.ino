int redVal = 0;
int greenVal = 0;
int blueVal = 0;
int potVal = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  potVal = analogRead(A3);
  if (potVal <= 340){
    potVal = potVal * 0.75;
    
    redVal =256 - potVal;
    greenVal = potVal;
    blueVal = 0;
  }
  else if (potVal <= 681){
    potVal = (potVal - 341) * 0.75;
    redVal = 0;
    greenVal = 256 - potVal;
    blueVal = potVal;
  }
  else{
    potVal = (potVal - 683) * 0.75;
    redVal = potVal;
    greenVal = 0;
    blueVal = 256 - potVal;
  }
  analogWrite(13, redVal);
  analogWrite(11, greenVal);
  analogWrite(10, blueVal);
}
