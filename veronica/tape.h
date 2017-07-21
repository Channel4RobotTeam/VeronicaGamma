/* 
 *  Veronica
 *  July 2017
 *  
 *  Tape follows along competition surface and around pool
 */

#include "constants.h" 
#include "menu.h"

/* FUNCTIONS DECLARATIONS */
void tapeFollow(Menu* menu);
void aroundTank(Menu* menu);
bool reachedTank();

/* TAPE FOLLOWING VARIABLES */
int lastErr = 0;
int currCount = 0;
int lastCount = 0;
int displayCount = 0;
int leftQRD;
int rightQRD;
int sideQRD;

/* TANK VARIABLES */
int lastTickCount = 0;
bool reachedTankVar = false;

void tapeFollow(Menu* menu) {
  currCount = 0;
  displayCount = 0;
  while (true) {
    currCount = currCount + 1;
    displayCount = displayCount + 1;
    
    /* Press STOP to switch to menu */
    if (stopbutton()) { 
      delay(100); 
      if (stopbutton()) { break; } 
    }

    /* INPUTS */
    leftQRD = analogRead(LEFT_QRD);
    rightQRD = analogRead(RIGHT_QRD);
    sideQRD = analogRead(SIDE_QRD);

    /* ERROR */
    int currErr = 0;
    if (leftQRD < menu->thresh && rightQRD > menu->thresh) { /* SLIGHTLY LEFT OF TAPE */ 
      currErr = -1; 
    } 
    else if (leftQRD > menu->thresh && rightQRD < menu->thresh) { /* SLIGHTLY RIGHT OF TAPE */ 
      currErr = +1; 
    } 
    else if (leftQRD < menu->thresh && rightQRD < menu->thresh){ /* COMPLETELY OFF TAPE */
      if (lastErr < 0) { currErr = -2; } 
      else if(lastErr > 0) { currErr = +2; }
      else { /* COMPLETELY OFF TAPE AFTER AN ERROR OF 0 */  
        reachedTankVar = true;
        aroundTank(menu);
      }
    }

    /* CORRECTION CALCULATION */
    int proportional = currErr * menu->kp;
    int integral = 0 * menu->ki;
    int derivative = ((currErr - lastErr) / (currCount - lastCount)) * menu->kd;
    int correction = proportional + integral + derivative;

    /* CORRECTION APPLICATION */
    if (menu->velocity + correction < 0 || menu->velocity - correction < 0) {
      if (correction < 0) {
        motor.speed(LEFT_MOTOR, menu->velocity - correction);
        motor.speed(RIGHT_MOTOR, 0);
      } else {
        motor.speed(LEFT_MOTOR, 0);
        motor.speed(RIGHT_MOTOR, menu->velocity + correction);
      }
    } else {
      motor.speed(LEFT_MOTOR, menu->velocity - correction);
      motor.speed(RIGHT_MOTOR, menu->velocity + correction);
    }

    /* PRINTS INPUTS AND OUTPUTS */
    if(displayCount == 30) {
      LCD.clear(); LCD.home();
      LCD.print("L: "); LCD.print(leftQRD); LCD.print(" R: "); LCD.print(rightQRD);
      LCD.setCursor(0,1);
      LCD.print("Corr: "); LCD.print(correction);
      displayCount = 0; /* RESET */
    }

    lastErr = currErr;
    lastCount = currCount;
  }
}

void aroundTank(Menu* menu) {
  currCount = 0;
  displayCount = 0;
  lastTickCount = 0;
  while (true) {
    currCount++;
    displayCount = displayCount + 1;

    /* Read Inputs */
    leftQRD = analogRead(LEFT_QRD);
    rightQRD = analogRead(RIGHT_QRD);
    sideQRD = analogRead(SIDE_QRD);
    
    /* Press STOP to switch to menu */
    if (stopbutton()) { 
       break;  
    }

    /* ERROR */
    int currErr = 0;
    if (leftQRD < menu->thresh && rightQRD > menu->thresh) { /* SLIGHTLY LEFT OF TAPE  -> desired position */ 
      currErr = -1; 
    } 
    else if (leftQRD > menu->thresh && rightQRD > menu->thresh) { /* ON TAPE -> right of desired position */
      currErr = 0;
    }
    else if (leftQRD > menu->thresh && rightQRD < menu->thresh) { /* SLIGHTLY RIGHT OF TAPE -> very right of desired position */ 
      currErr = 1; 
    } 
    else if (leftQRD < menu->thresh && rightQRD < menu->thresh) { /* COMPLETELY OFF TAPE */
      if(lastErr == 0 || lastErr > 0) { currErr = 3; } // extremely right of desired position
      else { currErr = 0; }
    }

    /* CORRECTION CALCULATION */
    int proportional = currErr * menu->kp_circle;
    int integral = 0 * menu->ki_circle;
    int derivative = ((currErr - lastErr) / (currCount - lastCount)) * menu->kd_circle;
    int correction = proportional + integral + derivative;

    /* CORRECTION APPLICATION */
    if (menu->velocity + correction < 0 || menu->velocity - correction < 0) {
      if (correction < 0) {
        motor.speed(LEFT_MOTOR, menu->velocity - correction);
        motor.speed(RIGHT_MOTOR, 0);
      } else {
        motor.speed(LEFT_MOTOR, 0);
        motor.speed(RIGHT_MOTOR, menu->velocity + correction);
      }
    } else {
      motor.speed(LEFT_MOTOR, menu->velocity - correction);
      motor.speed(RIGHT_MOTOR, menu->velocity + correction);
    }

    /* PRINTS INPUTS AND OUTPUTS */
    if(displayCount == 30) {
      LCD.clear(); LCD.home();
      LCD.print("L: "); LCD.print(leftQRD); LCD.print(" R: "); LCD.print(rightQRD);
      LCD.setCursor(0,1);
      LCD.print("S: "); LCD.print(sideQRD); LCD.print(" Cor: "); LCD.print(correction);
      displayCount = 0; /* RESET */
    }

    /* When side QRD senses tape */
    if(sideQRD > menu->thresh_side && (currCount - lastTickCount) > 700) {
      lastTickCount = currCount;
      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      LCD.clear(); LCD.home();
      LCD.print("AT TICK MARK");
      LCD.setCursor(0, 1);
      LCD.print("S: "); LCD.print(sideQRD);
     //delay(1000); // amount of time required to pick up toy
    }

    lastErr = currErr;
    lastCount = currCount;
  }
}

bool reachedTank() {
  return reachedTankVar;
}



