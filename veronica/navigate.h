/* 
 *  Channel 4 News Team
 *  Veronica
 *  July 2017
 *  
 *  Uses IR detection locate zipline
 */

#include "constants.h" 

/* FUNCTION DECLARATIONS */
void locateZipline();
void turnToZipline(bool course);
void rightTurn();

int signal10kHz = 0;
int lastSignal10kHz = 0;
int count = 0;
int correctCount = 0;
bool right = true;
int maxSignal = 0;
int turnCount = 0;

void locateZipline() {
  
  while(true) {
    count = count + 1;
    
    signal10kHz = analogRead(TENKHZ);
   
    if(signal10kHz > THRESH_TENKHZ && signal10kHz > lastSignal10kHz) {
      correctCount = count;
      motor.speed(LEFT_MOTOR, VELOCITY);
      motor.speed(RIGHT_MOTOR, VELOCITY);
    } else {
      if((count - correctCount) > 1000 && right == true) {
        motor.speed(LEFT_MOTOR, VELOCITY);
        motor.speed(RIGHT_MOTOR, VELOCITY + 30);
        right = false;
      } else if((count - correctCount) > 1000 && right == false) {
        motor.speed(LEFT_MOTOR, VELOCITY);
        motor.speed(RIGHT_MOTOR, VELOCITY-30);
        right = true;
      }
    }

    /* When robot reaches set distance from zipline */
    if(signal10kHz > maxSignal) {
      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      break;
    }
    
    lastSignal10kHz = signal10kHz; 
  }
}

void turnToZipline(bool course) {
  turnCount = 0;
  
  while(true) {
    if(course == true && turnCount < 1000) { // LEFT COURSE default
      motor.speed(LEFT_MOTOR, VELOCITY + 25);
      motor.speed(RIGHT_MOTOR, -25);
    } else if (course == false && turnCount < 1000) {
      motor.speed(LEFT_MOTOR, -25);
      motor.speed(RIGHT_MOTOR, VELOCITY + 25);
    } else if(turnCount >= 1000) {
      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      break;
    }
  }
}

void rightTurn() {
  int rightTurnCount = 0;
  
  while(true) {
    motor.speed(LEFT_MOTOR, VELOCITY-50);
    motor.speed(RIGHT_MOTOR, -25);
    rightTurnCount++;

    if(rightTurnCount > 1000) {
      break;
    }
  }
}

