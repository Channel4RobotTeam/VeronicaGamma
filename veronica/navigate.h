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
void rightTurn(Menu* menu);

bool right = true;
int turnCount = 0;

void locateZipline() {
  
  int count = 0;
  
  while(true) {
    /* Press YELLOW RESET to switch to user input menu */
    int switch0 = digitalRead(0);
    if (switch0 == 0) { 
      delay(1000); 
      if (switch0 == 0) { break; } 
    }

    count = count + 1;
    if(count == 30) {
      LCD.clear(); LCD.home();
      LCD.print("FINDING ZIPLINE");
      LCD.setCursor(0,1); LCD.print("10kHz: "); LCD.print(analogRead(TENKHZ));
    }

    motor.speed(LEFT_MOTOR, VELOCITY-50);
    motor.speed(RIGHT_MOTOR, VELOCITY-50);

    int signal10kHz = analogRead(TENKHZ);

    if(signal10kHz > THRESH_TENKHZ) {
      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      delay(3000);
      break;
    }
  }
  
}

void rightTurn(Menu* menu) {
  int count = 0;
  
  while(true) {
    /* Press YELLOW RESET to switch to user input menu */
    int switch0 = digitalRead(0);
    if (switch0 == 0) { 
      delay(1000); 
      if (switch0 == 0) { break; } 
    }
    
    /* INPUTS */
    int leftQRD = analogRead(LEFT_QRD);
    int rightQRD = analogRead(RIGHT_QRD);
    
    count = count + 1;
    if(count == 30) {
      LCD.clear(); LCD.home();
      LCD.print("RIGHT TURN");
      LCD.setCursor(0,1); LCD.print("L: "); LCD.print(leftQRD); LCD.print(" R: "); LCD.print(rightQRD);
    }
    
    motor.speed(LEFT_MOTOR, VELOCITY-50);
    motor.speed(RIGHT_MOTOR, -75);

    if(leftQRD > menu->thresh_left || rightQRD > menu->thresh_right) {
      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      delay(1000);
      break;
    }
  }
  
}

