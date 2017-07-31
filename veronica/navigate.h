/* 
 *  Channel 4 News Team
 *  Veronica
 *  July 2017
 *  
 *  Uses IR detection locate zipline
 */

#include "constants.h" 
#include "menu.h"

/* FUNCTION DECLARATIONS */
void locateZipline(bool leftCourse);
void rightTurn(Menu* menu);
void backUp();
void driveForward(unsigned long duration);
void statRightTurn(Menu* menu);



/* 
 *  
 *  Drives straight forward until senses 10kHz signal and then stops 
 *  
 */
void locateZipline(bool leftCourse) {
  
  int count = 0;
  
  while(true) {
    /* Press YELLOW RESET to switch to user input menu */
    int switch0 = digitalRead(0);
    if (switch0 == 0) { 
      delay(1000); 
      if (switch0 == 0) { break; } 
    }

    /* INPUTS */
    int signal10kHz = analogRead(TENKHZ);
  
    count = count + 1;
    if(count == 30) {
      LCD.clear(); LCD.home();
      LCD.print("FINDING ZIPLINE");
      LCD.setCursor(0,1); LCD.print("10kHz: "); LCD.print(analogRead(TENKHZ));
      count = 0;
    }

    /* IF ON LEFT COURSE */
    if(leftCourse) {
      motor.speed(LEFT_MOTOR, VELOCITY-50);
      motor.speed(RIGHT_MOTOR, VELOCITY-50);
  
      if(signal10kHz > THRESH_TENKHZ) {
        motor.speed(LEFT_MOTOR, 0);
        motor.speed(RIGHT_MOTOR, 0);
        delay(3000);
        break;
      }
    } else { /* IF ON RIGHT COURSE */
      // TODO: go to middle tick mark && drive on angle to zipline?
    }
  }
  
}



/* 
 *  
 *  Continues to turn right until robot sees tape 
 *  
 */
void rightTurn(Menu* menu) {
  
  int displayCount = 0;
  int count = 0;
  
  while(true) {
    count = count + 1;
    
    /* Press YELLOW RESET to switch to user input menu */
    int switch0 = digitalRead(0);
    if (switch0 == 0) { 
      delay(1000); 
      if (switch0 == 0) { break; } 
    }
    
    /* INPUTS */
    int leftQRD = analogRead(LEFT_QRD);
    int rightQRD = analogRead(RIGHT_QRD);
    
    displayCount = displayCount + 1;
    if(displayCount == 30) {
      LCD.clear(); LCD.home();
      LCD.print("RIGHT TURN");
      LCD.setCursor(0,1); LCD.print("L: "); LCD.print(leftQRD); LCD.print(" R: "); LCD.print(rightQRD);
      displayCount = 0;
    }
    
    motor.speed(LEFT_MOTOR, VELOCITY-70);
    motor.speed(RIGHT_MOTOR, 25);

    if(count > 1000 && (leftQRD > menu->thresh_left || rightQRD > menu->thresh_right)) {
      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      delay(2000);
      break;
    }
  }
  
}



/* 
 *  
 *  Backs up for 1000 loops 
 *  
 */ 
//TODO: check how far this number of loops corresponds to
void backUp() {
  
  int count = 0;
  unsigned long start = millis();

  while(millis() - start < 1000) {
    count = count + 1;
    motor.speed(LEFT_MOTOR, -75);
    motor.speed(RIGHT_MOTOR, -75);
    delay(100);
  }

  motor.speed(LEFT_MOTOR, 0);
  motor.speed(RIGHT_MOTOR, 0);
  delay(2000);
  
}


/* 
 *  
 *  Drives forward for 1000 loops
 *  
 */
void driveForward(unsigned long duration) {

  int count = 0;
  unsigned long start = millis();

  while(millis() - start < duration) {
    count = count + 1;
    motor.speed(LEFT_MOTOR, VELOCITY - 25);
    motor.speed(RIGHT_MOTOR, VELOCITY - 25);
    delay(100);
  }

  motor.speed(LEFT_MOTOR, 0);
  motor.speed(RIGHT_MOTOR, 0);
  delay(2000);
  
}

void statRightTurn(Menu* menu) {

  int displayCount = 0;
  int count = 0;
  
  while(true) {
    count = count + 1;
    
    /* Press YELLOW RESET to switch to user input menu */
    int switch0 = digitalRead(0);
    if (switch0 == 0) { 
      delay(1000); 
      if (switch0 == 0) { break; } 
    }
    
    displayCount = displayCount + 1;
    if(displayCount == 30) {
      LCD.clear(); LCD.home();
      LCD.print(" stat RIGHT TURN");
      displayCount = 0;
    }
    
    motor.speed(LEFT_MOTOR, 50);
    motor.speed(RIGHT_MOTOR, -50);
    delay(1);

    if(count > 1000) {
      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      delay(2000);
      break;
    }
  }
  
}

