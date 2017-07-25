/* 
 *  Channel 4 News Team
 *  Veronica
 *  July 2017
 *  
 *  Tape follows along competition surface and around tank
 */

#include "constants.h" 

/* FUNCTIONS DECLARATIONS */
void tapeFollow(Menu* menu);
void aroundTank(Menu* menu);
bool reachedTank();
double getDistance();
void recoverLostTape(Menu* menu, int currentError, int lastError);

int switch0 = digitalRead(0);

/* TAPE FOLLOWING VARIABLES */
int lastErr = 0;
int currCount = 0;
int lastCount = 0;
int displayCount = 0;
int onTapeCount = 0;

// QRDs
int leftQRD;
int rightQRD;
int sideQRD;
int frontQRD;

#define FRONT_QRD 3

/* DISTANCE TRACKING VARIABLES */
int wheelQRD;
int wheelRevolutions = 0;
double distance = 0;

/* TANK VARIABLES */
int lastTickCount = 0;
bool reachedTankVar = false;

void tapeFollow(Menu* menu, bool gateStage) {
  currCount = 0;
  displayCount = 0;
  while (true) {
    currCount = currCount + 1;
    displayCount = displayCount + 1;
    
    /* Press YELLOW RESET to switch to user input menu */
    switch0 = digitalRead(0);
    if (switch0 == 0) { 
      delay(1000); 
      if (switch0 == 0) { break; } 
    }
    if (stopbutton()) {
      delay(1000);
      if (stopbutton()){
        displayMenu(menu);
        displayCount = 0;
      }
    }

    /* INPUTS */
    leftQRD = analogRead(LEFT_QRD);
    rightQRD = analogRead(RIGHT_QRD);
    sideQRD = analogRead(SIDE_QRD);
    wheelQRD = analogRead(WHEEL_QRD);
    frontQRD = analogRead(FRONT_QRD);

    /* ERROR */
    int currErr = 0;
    if (leftQRD < menu->thresh_left && rightQRD > menu->thresh_right) { /* SLIGHTLY LEFT OF TAPE */ 
      currErr = -1; 
      onTapeCount = currCount;
    } 
    else if (leftQRD > menu->thresh_left && rightQRD < menu->thresh_right) { /* SLIGHTLY RIGHT OF TAPE */ 
      currErr = +1; 
      onTapeCount = currCount;
    } 
    else if (leftQRD < menu->thresh_left && rightQRD < menu->thresh_right){ /* COMPLETELY OFF TAPE */
//      if(lastErr == 0) {
//        reachedTankVar = true;
//        motor.speed(LEFT_MOTOR, 0);
//        motor.speed(RIGHT_MOTOR, 0);
//        break;
//      }
      if (lastErr < 0) { // OFF TO LEFT
        currErr = -2; 
//        if((currCount - onTapeCount) > 2000) {
//          motor.speed(LEFT_MOTOR, 0);
//          motor.speed(RIGHT_MOTOR, 0);
//          recoverLostTape(menu, currErr, lastErr);
//        }
      } 
      else if(lastErr > 0) { // OFF TO RIGHT
        currErr = +2; 
//        if((currCount - onTapeCount) > 2000) {
//          motor.speed(LEFT_MOTOR, 0);
//          motor.speed(RIGHT_MOTOR, 0);
//          recoverLostTape(menu, currErr, lastErr);
//        }
      }
    }

    /* CORRECTION CALCULATION */
    int proportional = currErr * menu->kp;
    int integral = 0 * menu->ki;
    int derivative = ((currErr - lastErr) / (currCount - lastCount)) * menu->kd;
    int correction = proportional + integral + derivative;

    /* CORRECTION APPLICATION */
    if (gateStage) {
      /* STAY STOPPED WHILE THE ALARM IS ON */
      while (analogRead(ONEKHZ) < menu->thresh_onekhz || analogRead(TENKHZ) > menu->thresh_tenkhz) {
        motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
      }
      break;
    }
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
      LCD.print("F: "); LCD.print(frontQRD); LCD.print("Corr: "); LCD.print(correction);
      displayCount = 0; /* RESET */
    }

    /* When wheel QRD sees white tape */
    if(wheelQRD < THRESH_WHEEL && (currCount - wheelRevolutions) > 500) {
      wheelRevolutions = wheelRevolutions + 1;
      double circumference = 2 * PI * WHEEL_RADIUS;
      distance = wheelRevolutions * circumference;
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
    currCount = currCount + 1; //changed from currCount++
    displayCount = displayCount + 1;

    /* INPUTS */
    leftQRD = analogRead(LEFT_QRD);
    rightQRD = analogRead(RIGHT_QRD);
    sideQRD = analogRead(SIDE_QRD);

    /* Press YELLOW RESET to switch to user input menu */
    switch0 = digitalRead(0);
    if (switch0 == 0) { 
      delay(1000); 
      if (switch0 == 0) { break; } 
    }
    if (stopbutton()) {
      delay(1000);
      if (stopbutton()){
        displayMenu(menu);
        displayCount = 0;
      }
    }

    /* ERROR */
    int currErr = 0;
    if (leftQRD < menu->thresh_left && rightQRD > menu->thresh_right) { /* SLIGHTLY LEFT OF TAPE */ 
      currErr = -1; 
    } 
    else if (leftQRD > menu->thresh_left && rightQRD > menu->thresh_right) { /* ON TAPE */
      currErr = 0;
    }
    else if (leftQRD > menu->thresh_left && rightQRD < menu->thresh_right) { /* SLIGHTLY RIGHT OF TAPE */ 
      currErr = 1; 
    } 
    else if (leftQRD < menu->thresh_left && rightQRD < menu->thresh_right) { /* COMPLETELY OFF TAPE */
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
    if(sideQRD > menu->thresh_side && (currCount - lastTickCount) > 1000) {
      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      lastTickCount = currCount;
      break;
    }

    lastErr = currErr;
    lastCount = currCount;
  }
}

bool reachedTank() {
  return reachedTankVar;
}

double getDistance() {
  return distance;
}

void recoverLostTape(Menu* menu, int currentError, int lastError) {
  if(currentError < 0) {
    motor.speed(LEFT_MOTOR, menu->velocity+25);
    motor.speed(RIGHT_MOTOR, menu->velocity-50);
  } else {
    motor.speed(LEFT_MOTOR, menu->velocity+25);
    motor.speed(RIGHT_MOTOR, menu->velocity-50);
  }
}

