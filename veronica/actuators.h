
/**********************************/
/*                                */ 
/*          Arm Commands          */
/*                                */
/**********************************/

#include "menu.h"

//RCServo0 --> Raise + Lower
//RCservo1 --> Pincer Open + Close

/*Arm Raise*/
void raiseArm() {
  for(int pos0 = 0 ; pos0<=170 ; pos0 += 1){
    RCServo0.write(pos0);
    delay(12);
  } 
}

/*Close Pincer*/
void closePincer() {
  for(int pos1 = 10 ; pos1<=(90) ; pos1+=1){
    RCServo1.write(pos1);
  }
}

/*Lower Arm*/
void lowerArm() {
  for(int pos0 = 170; pos0 >=0 ; pos0-=1){
    RCServo0.write(pos0);
    delay(12);
  }
}

/*Open Pincer*/
void openPincer() {
  for(int pos1 = 90; pos1>=25; pos1-=1){
    RCServo1.write(pos1);
  }
  delay(500);
  for(int pos1 = 25; pos1>=10 ; pos1 -= 1){
          RCServo1.write(pos1);
          delay(10);
  }
}

/*Shake arm in case agent gets stuck*/

void shake() {
  RCServo0.write(30);
  delay(80);
  RCServo0.write(0);
  delay(80);
  RCServo0.write(30);
  delay(80);
  RCServo0.write(0);
}

void raiseLift() {

  unsigned long start = millis();

  LCD.clear(); LCD.home();
  LCD.print("RAISING");
  
  /* RAISE THE LIFT */
  while (millis() - start < 10000) { /* TODO find proper duration */
    motor.speed(LIFT_MOTOR, 175);
  }
  motor.speed(LIFT_MOTOR, 0);
  
}

void lowerLift() {
  
  unsigned long start = millis();

  LCD.clear(); LCD.home();
  LCD.print("LOWERING");

  /* LOWER THE LIFT */
  while (millis() - start < 5000){ /* TODO find proper duration */
    motor.speed(LIFT_MOTOR, 5);
  }
  motor.speed(LIFT_MOTOR, 0);
  
}

