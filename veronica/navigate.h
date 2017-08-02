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
void locateZipline();
void rightTurnToTape(Menu* menu, int occurrences);
void backUp();
void driveForward(unsigned long duration);
void rightTurn(Menu* menu, unsigned long duration);


/* 
 *  
 *  Drives straight forward until senses 10kHz signal and then stops 
 *  
 */
void locateZipline() {

  LCD.clear(); LCD.home();
  int displayCount = 0;
  
  while(true) {
    /* Press YELLOW RESET to switch to user input menu */
    int switch0 = digitalRead(0);
    if (switch0 == 0) { 
      delay(1000); 
      if (switch0 == 0) { break; } 
    }

    /* INPUTS */
    int signal10kHz = analogRead(TENKHZ);
  
    displayCount = displayCount + 1;
    if(displayCount == 30) {
      LCD.home();
      LCD.print("FINDING ZIPLINE");
      LCD.setCursor(0,1); LCD.print("10kHz: "); LCD.print(analogRead(TENKHZ));
      displayCount = 0;
    }

    motor.speed(LEFT_MOTOR, VELOCITY-50);
    motor.speed(RIGHT_MOTOR, VELOCITY-50);

    if(signal10kHz > THRESH_TENKHZ) {
      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      delay(2000);
      break;
    }
  }
  
}



/* 
 *  
 *  Continues to turn right until robot sees tape 
 *  
 */
void rightTurnToTape(Menu* menu, int occurrences) {
  
  int displayCount = 0;
  int count = 0;
  int tapeCount = 0;
  
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

    if(count > 1000 && (leftQRD > menu->thresh_left && rightQRD < menu->thresh_right)) {
      tapeCount = tapeCount + 1;
      if(tapeCount >= occurrences) {
        count = 0;
        motor.speed(LEFT_MOTOR, 0);
        motor.speed(RIGHT_MOTOR, 0);
        break;
      }
    }
  }
  
}



/* 
 *  
 * Drives in reverse for a set duration determined by the function's parameter
 *  
 */ 
void backUp(unsigned long duration) {
  
  unsigned long start = millis();

  while(millis() - start < duration) {
    motor.speed(LEFT_MOTOR, -75);
    motor.speed(RIGHT_MOTOR, -75);
  }

  motor.speed(LEFT_MOTOR, 0);
  motor.speed(RIGHT_MOTOR, 0);
  
}


/* 
 *  
 *  Drives forward for a set duration determined by the function's parameter
 *  
 */
void driveForward(unsigned long duration) {
  
  unsigned long start = millis();

  while(millis() - start < duration) {
    motor.speed(LEFT_MOTOR, VELOCITY - 25);
    motor.speed(RIGHT_MOTOR, VELOCITY - 25);
  }

  motor.speed(LEFT_MOTOR, 0);
  motor.speed(RIGHT_MOTOR, 0);
  
}


/*
 * 
 * Makes a right turn for a set duration determined by the function's 'duration' parameter
 * 
 */
void rightTurn(Menu* menu, unsigned long duration) {

  unsigned long start = millis();
  int displayCount = 0;
  
  while(millis() - start < duration) {
    
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
    
    motor.speed(LEFT_MOTOR, 70);
    motor.speed(RIGHT_MOTOR, -70);
  }
  
  motor.speed(LEFT_MOTOR, 0);
  motor.speed(RIGHT_MOTOR, 0);
  
}

