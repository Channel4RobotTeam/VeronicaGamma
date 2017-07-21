/* 
 *  Veronica
 *  July 2017
 *  
 *  Tape follows along competition surface and around pool
 */

#include "constants.h" 

/* FUNCTIONS DECLARATIONS */
void tapeFollow();
void aroundTank();
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

void tapeFollow() {
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
    if (leftQRD < thresh && rightQRD > thresh) { /* SLIGHTLY LEFT OF TAPE */ 
      currErr = -1; 
    } 
    else if (leftQRD > thresh && rightQRD < thresh) { /* SLIGHTLY RIGHT OF TAPE */ 
      currErr = +1; 
    } 
    else if (leftQRD < thresh && rightQRD < thresh){ /* COMPLETELY OFF TAPE */
      if (lastErr < 0) { currErr = -2; } 
      else if(lastErr > 0) { currErr = +2; }
      else { /* COMPLETELY OFF TAPE AFTER AN ERROR OF 0 */  
        reachedTankVar = true;
        aroundTank();
      }
    }

    /* CORRECTION CALCULATION */
    int proportional = currErr * kp;
    int integral = 0 * ki;
    int derivative = ((currErr - lastErr) / (currCount - lastCount)) * kd;
    int correction = proportional + integral + derivative;

    /* CORRECTION APPLICATION */
    if (velocity + correction < 0 || velocity - correction < 0) {
      if (correction < 0) {
        motor.speed(LEFT_MOTOR, velocity - correction);
        motor.speed(RIGHT_MOTOR, 0);
      } else {
        motor.speed(LEFT_MOTOR, 0);
        motor.speed(RIGHT_MOTOR, velocity + correction);
      }
    } else {
      motor.speed(LEFT_MOTOR, velocity - correction);
      motor.speed(RIGHT_MOTOR, velocity + correction);
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

void aroundTank() {
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
    if (leftQRD < thresh && rightQRD > thresh) { /* SLIGHTLY LEFT OF TAPE  -> desired position */ 
      currErr = 0; 
    } 
    else if (leftQRD > thresh && rightQRD > thresh) { /* ON TAPE -> right of desired position */
      currErr = 1;
    }
    else if (leftQRD > thresh && rightQRD < thresh) { /* SLIGHTLY RIGHT OF TAPE -> very right of desired position */ 
      currErr = 1; 
    } 
    else if (leftQRD < thresh && rightQRD < thresh) { /* COMPLETELY OFF TAPE */
      if(lastErr > 0) { currErr = 2; } // extremely right of desired position
      else { currErr = -1; }
    }

    /* CORRECTION CALCULATION */
    int proportional = currErr * kp;
    int integral = 0 * ki;
    int derivative = ((currErr - lastErr) / (currCount - lastCount)) * kd;
    int correction = proportional + integral + derivative;

    /* CORRECTION APPLICATION */
    if (velocity + correction < 0 || velocity - correction < 0) {
      if (correction < 0) {
        motor.speed(LEFT_MOTOR, velocity - correction);
        motor.speed(RIGHT_MOTOR, 0);
      } else {
        motor.speed(LEFT_MOTOR, 0);
        motor.speed(RIGHT_MOTOR, velocity + correction);
      }
    } else {
      motor.speed(LEFT_MOTOR, velocity - correction);
      motor.speed(RIGHT_MOTOR, velocity + correction);
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
    if(sideQRD > side_thresh && (currCount - lastTickCount) > 700) {
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



