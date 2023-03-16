#include <Arduino.h>
#include <Servo.h>

// Line color states
typedef enum {
  RED, BLACK, WHITE
} Color;

// Robot states
typedef enum {
  WAIT, LOADING, GOOD_PRESS, BAD_PRESS, ORIENTATION, FINISHED
} Which_Press;

// Game timer
#define GAME_TIMER           130000

// Game timer
#define WHITE_THRESH         30
#define RED_THRESH           700
#define IR_THRESHHOLD        460
// Sets initial robot state to WAIT
Which_Press press = WAIT;
int color1;
int color2;
int color3;
int irsensor1;
int irsensor2;
int previousTime = 0;
int previousPreviousTime = 0;
Color Which_Line1;
Color Which_Line2;
Color Which_Line3;
const int Motor_11 = 6;
const int Motor_12 = 7;
const int PWM_1 = 3;
const int Motor_21 = 8;
const int Motor_22 = 9;
const int PWM_2 = 5;
const int Starting_Switch = 12;
const int Loading_Switch = 13;
int currentTime = 0;
const int celebration_fan = 4;
const int myservo_1 = 10;
const int myservo_2 = 11;
int press_time = 0;
int impervious_time = 0;
int newTime = 0;

Servo myservo1;
Servo myservo2;

int downPosition = 0;     // arm is lowered
int upPosition = 90;      // arm is raised

void handleMoveLeft1(void);
void handleMoveRight1(void);
void handleMoveLeft2(void);
void handleMoveRight2(void);
void checkGlobalEvents(void);
void handleMoveStop(void);
void handleMoveForward(void);
void handleMoveBackLeft(void);
void handleMoveBackRight(void);
void handleMoveSpin(void);
void handleMoveBackward(void);
void dispensePress1(void);
void dispensePress2(void);
void CelebrationFan(void);
void OrientYourself(void);
void handleMoveSpin1(void);

void setup()
{
  pinMode(Motor_11, OUTPUT);
  pinMode(Motor_12, OUTPUT);
  pinMode(PWM_1, OUTPUT);
  pinMode(Motor_21, OUTPUT);
  pinMode(Motor_22, OUTPUT);
  pinMode(PWM_2, OUTPUT);
  pinMode(Starting_Switch, INPUT);
  pinMode(Loading_Switch, INPUT);
  pinMode(celebration_fan, OUTPUT);
  pinMode(myservo_1, OUTPUT);
  pinMode(myservo_2, OUTPUT);
  myservo1.attach(myservo_1);
  myservo1.write(upPosition);
  myservo2.attach(myservo_2);
  myservo2.write(downPosition);
}

void dispensePress1() {
    myservo1.write(downPosition);    // raise arm
    delay(1000);                  // time for balls to roll out
    myservo1.write(upPosition);  // lower arm
}

void dispensePress2() {
    Serial.println(1);
    myservo2.write(upPosition);    // raise arm
    delay(1000);                  // time for balls to roll out
    myservo2.write(downPosition);  // lower arm               // time for balls to roll out
}

void loop() {
  checkGlobalEvents();
  previousPreviousTime = millis();
  if (press == ORIENTATION){
    // Robot spins until IR sensor passes IR Beacon
    handleMoveSpin1();
    if(irsensor1 >= IR_THRESHHOLD){
      handleMoveStop();
      press = GOOD_PRESS;
      delay(100);
      handleMoveForward();
      delay(750);
    }
  }
  if (press == GOOD_PRESS){
    // Robot moves left if on white and right if on white.
    // It stops and goes to the next state if on black.
    if ((Which_Line1 == WHITE)){
      Serial.println(1);
      handleMoveLeft1();
    } else if ((Which_Line2 == RED)){
      handleMoveRight1();
    } else if ((Which_Line1 == BLACK) && (Which_Line2 == BLACK)){
      handleMoveStop();
      dispensePress2();
      press = BAD_PRESS;  
      handleMoveBackLeft();
      delay(1450);
      handleMoveSpin();
      delay(1400);
      handleMoveForward();
      delay(800);
      int newTime = millis();
    }
  }
  if (press == BAD_PRESS){
    // Line follows allonw black line until it crosses bad press red line
    int newNewTime = millis();
    if ((Which_Line3 != RED)){
      if ((Which_Line1 == BLACK)){
        handleMoveLeft2();
      } else if ((Which_Line2 != BLACK)){
        handleMoveRight2();
      }
    } else if ((Which_Line3 == RED)){
      Serial.println(newNewTime - newTime);
      handleMoveStop();
      delay(300);
      dispensePress1();
      press = FINISHED;

    }
  }
  if (previousPreviousTime - previousTime >= GAME_TIMER){
    // If the game time has expired, stops and turns on celebration.
    handleMoveStop();
    CelebrationFan();
  }
}


void handleMoveLeft1(void) {
  analogWrite(PWM_1, 100);
  analogWrite(PWM_2, 0);
  digitalWrite(Motor_11, LOW);
  digitalWrite(Motor_12, HIGH);
  digitalWrite(Motor_21, LOW);
  digitalWrite(Motor_22, HIGH);
}

void handleMoveRight1(void) {
  analogWrite(PWM_1, 128);
  analogWrite(PWM_2, 255);
  digitalWrite(Motor_11, HIGH);
  digitalWrite(Motor_12, LOW);
  digitalWrite(Motor_21, HIGH);
  digitalWrite(Motor_22, LOW);
}

void handleMoveLeft2(void) {
  analogWrite(PWM_1, 60);
  analogWrite(PWM_2, 160);
  digitalWrite(Motor_11, HIGH);
  digitalWrite(Motor_12, LOW);
  digitalWrite(Motor_21, LOW);
  digitalWrite(Motor_22, HIGH);
}

void handleMoveRight2(void) {
  analogWrite(PWM_1, 89);
  analogWrite(PWM_2, 200);
  digitalWrite(Motor_11, LOW);
  digitalWrite(Motor_12, HIGH);
  digitalWrite(Motor_21, HIGH);
  digitalWrite(Motor_22, LOW);
}

void handleMoveStop(void) {
  analogWrite(PWM_1, 0);
  analogWrite(PWM_2, 0);
  digitalWrite(Motor_11, HIGH);
  digitalWrite(Motor_12, HIGH);
  digitalWrite(Motor_21, HIGH);
  digitalWrite(Motor_22, HIGH);
}

void handleMoveForward(void) {
  analogWrite(PWM_1, 90);
  analogWrite(PWM_2, 120);
  digitalWrite(Motor_11, LOW);
  digitalWrite(Motor_12, HIGH);
  digitalWrite(Motor_21, HIGH);
  digitalWrite(Motor_22, LOW);
}

void handleMoveBackward(void) {
  analogWrite(PWM_1, 120);
  analogWrite(PWM_2, 120);
  digitalWrite(Motor_11, HIGH);
  digitalWrite(Motor_12, LOW);
  digitalWrite(Motor_21, LOW);
  digitalWrite(Motor_22, HIGH);
}

void handleMoveBackLeft(void){
  analogWrite(PWM_1, 120);
  analogWrite(PWM_2, 0);
  digitalWrite(Motor_11, HIGH);
  digitalWrite(Motor_12, LOW);
  digitalWrite(Motor_21, LOW);
  digitalWrite(Motor_22, LOW);
}

void handleMoveBackRight(void){
  analogWrite(PWM_1, 0);
  analogWrite(PWM_2, 120);
  digitalWrite(Motor_11, LOW);
  digitalWrite(Motor_12, LOW);
  digitalWrite(Motor_21, LOW);
  digitalWrite(Motor_22, HIGH);
}

void handleMoveSpin(void){
  analogWrite(PWM_1, 120);
  analogWrite(PWM_2, 150);
  digitalWrite(Motor_11, LOW);
  digitalWrite(Motor_12, HIGH);
  digitalWrite(Motor_21, LOW);
  digitalWrite(Motor_22, HIGH);
}

void handleMoveSpin1(void){
  analogWrite(PWM_1, 100);
  analogWrite(PWM_2, 128);
  digitalWrite(Motor_11, LOW);
  digitalWrite(Motor_12, HIGH);
  digitalWrite(Motor_21, LOW);
  digitalWrite(Motor_22, HIGH);
}

void OnSwitchHit(void){
  // If on switch is hit starts the game and starts celebration.
  if (digitalRead(Starting_Switch) == HIGH){
    CelebrationFan();
    previousTime = millis();
    press = LOADING;
  }
}

void CelebrationFan(void){
  // Turns celebration fan on for 3 seconds.
  digitalWrite(celebration_fan, HIGH);
  delay(3000);
  digitalWrite(celebration_fan, LOW);
}

void LoadingSwitchHit(void){
  // Starts the robots movement when loading switch is hit.
  if (digitalRead(Loading_Switch) == HIGH){
    press = ORIENTATION;
  }
}

void OrientYourself() {
  // Reads the values of the 4 different analog sensors.
  color1 = analogRead(0);
  color2 = analogRead(1);
  color3 = analogRead(2);
  irsensor1 = analogRead(3);
  if (color1 <= WHITE_THRESH){
    Which_Line1 = WHITE;
  } else if ((WHITE_THRESH < color1) && (color1 <= RED_THRESH)){
    Which_Line1 = RED;
  } else if (color1 > RED_THRESH){
    Which_Line1 = BLACK;
  }
  if (color2 <= WHITE_THRESH){
    Which_Line2 = WHITE;
  } else if ((WHITE_THRESH < color2) && (color2 <= RED_THRESH)){
    Which_Line2 = RED;
  } else if (color2 > RED_THRESH){
    Which_Line2 = BLACK;
  }
  if (color3 <= WHITE_THRESH){
    Which_Line3 = WHITE;
  } else if ((WHITE_THRESH < color3) && (color3 <= RED_THRESH)){
    Which_Line3 = RED;
  } else if (color3 > RED_THRESH){
    Which_Line3 = BLACK;
  }
}

void checkGlobalEvents(void){
  if (press == WAIT) {
    OnSwitchHit();
  }
  else if (press == LOADING) {
    LoadingSwitchHit();
  }
  OrientYourself();
}