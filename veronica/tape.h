/* 
 *  Channel 4 News Team
 *  Veronica
 *  July 2017
 *  
 *  Tape follows along competition surface and around tank
 */

#include "constants.h" 
#include "navigate.h"

/* FUNCTIONS DECLARATIONS */
void tapeFollow(Menu* menu);
void aroundTank(Menu* menu);
void recoverLostTape(Menu* menu, int currentError, int lastError);

/* HELPER FUNCTIONS */
int tapeError(Menu* menu, int rightQRD, int leftQRD, int lastError); 
int tankError(Menu* menu, int rightQRD, int leftQRD, int lastError); 
void correctionApplication(Menu* menu, int correction); 
void printQRDs(); 
void printFreq();

/* TAPE FOLLOWING VARIABLES */
int lastErr = 0;
int currCount = 0;
int lastCount = 0;
int displayCount = 0;
int offTapeCount = 0;
int posErrCount = 0;
int negErrCount = 0;
int noErrCount = 0;

// QRDs & yellow button
int leftQRD;
int rightQRD;
int sideQRD;
int frontQRD;
int wheelQRD;
int switch0 = digitalRead(0);

/* TANK VARIABLES */
bool reachedTankVar = false;

/*
 * 
 * REGULAR TAPE FOLLOWING
 *          
 */
void tapeFollow(Menu* menu, bool gateStage, bool leftCourse) {

  int turnValue = 0;
  int lastTurnValue = 0;
  currCount = 0;
  displayCount = 0;
  bool topOfRamp = false;
  bool onRamp = false;
  int turnCount = 0;
  unsigned long startTime = millis();
  int proportional = 0;
  int integral = 0;
  int derivative = 0;
  int maxIntegral = 60; 
  
  /* MAIN LOOP */
  while (true) {
    currCount = currCount + 1;
    displayCount = displayCount + 1;
    
    /* Press YELLOW RESET to switch to user input menu */
    switch0 = digitalRead(YELLOWBUTTON);
    if (switch0 == 0) { 
      delay(1000); 
      if (switch0 == 0) { break; } 
    }

    /* INPUTS */
    leftQRD = analogRead(LEFT_QRD);
    rightQRD = analogRead(RIGHT_QRD);
    sideQRD = analogRead(SIDE_QRD);
    wheelQRD = analogRead(WHEEL_QRD);

    /* ERROR */
    int currErr = tapeError(menu, rightQRD, leftQRD, lastErr);
    
    /* CORRECTION CALCULATION */
    proportional = currErr * menu->kp;
//    integral = currErr * (currCount - lastCount) * menu->ki + integral;
//    if (integral > maxIntegral) {
//      integral = maxIntegral;
//    } else if (integral < 0){
//      integral = 0;
//    }
    derivative = ((currErr - lastErr) / (currCount - lastCount)) * menu->kd;
    int correction = proportional + integral + derivative;

    /* DETERMINE WHETHER TURNING */
    if (posErrCount > 100 ){
      turnValue = -1; /* LEFT TURN */
    } else if (negErrCount > 100){
      turnValue = +1; /* RIGHT TURN */
    } else if (noErrCount > 100) {
      turnValue = 0 ;
    }

    /* CORRECTION APPLICATION */
    correctionApplication(menu, correction);
    
    if (gateStage) { /* GATE STAGE */
      
      /* STAY STOPPED WHILE THE ALARM IS ON */
      // 1kHz high is disarmed, 10kHz high is alarmed
      if(analogRead(ONEKHZ) < 100 && (millis() - startTime) > 3500) { 
        LCD.clear(); LCD.home();
        LCD.print("WAITING...");
        int j = 0;
        while (analogRead(ONEKHZ) < 100){
          motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
          j = j + 1;
          if (j == 30) {
            LCD.setCursor(0,1);
            LCD.print("1kHz: "); LCD.print(analogRead(ONEKHZ));
            j = 0;
          }
        }
        delay(2000);
        break; /* GATE STAGE ENDS WHEN DOOR DISARMS */
      }
      
    } else { /* RAMP STAGE */
      
      /* RECOGNIZE WHEN THE TOP OF THE RAMP IS REACHED */
      if (digitalRead(RAMP_SWITCH) == 0) {
        topOfRamp = true;
      }

      if (displayCount == 30 && topOfRamp) {
        LCD.clear(); LCD.home(); 
        LCD.print("TOP OF RAMP");
        displayCount = 0; /* RESET */
      }

      if (leftCourse) { 
        if (lastTurnValue == +1 && turnValue == 0){
          turnCount = turnCount + 1;
        } 
      } else {
        if (lastTurnValue = -1 && turnValue == 0){
          turnCount = turnCount + 1;
        }
      }
      
      /* RECOGNIZE WHEN THE CIRCLE IS REACHED AND TURN ONTO IT */
      if (leftCourse && topOfRamp){
//        if (turnValue = +1){ /* RIGHT TURN INTO TANK */
//          if (turnCount < 3){
//            turnCount = turnCount + 1;
//          } else {
//            motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
//            LCD.clear(); LCD.home();
//            LCD.print("RIGHT TURN");
//            delay(2000);
//            rightTurn(menu);
//            break;
//          }
        if (turnCount >= 2){
          motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
          LCD.clear(); LCD.home();
          LCD.print("RIGHT TURN");
          delay(2000);
          rightTurn(menu);
          break;
        }
      } else if (!leftCourse && topOfRamp) { /* RIGHT COURSE */
        if (turnValue = -1){ /* LEFT TURN INTO TANK */
//          if (turnCount < 3){
//            turnCount = turnCount + 1;
//          } else {
//            motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
//            LCD.clear(); LCD.home();
//            LCD.print("LEFT TURN");
//            delay(2000);
//            break;
//          }
          if (turnCount >= 2){
            motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
            LCD.clear(); LCD.home();
            LCD.print("LEFT TURN");
            delay(2000);
            break;
          }
        }
      }
      
    }
    
    /* PRINTS INPUTS AND OUTPUTS */
    if(displayCount == 30 && !topOfRamp) {
      /* FOR DEBUGGING THE QRDs */
      printQRDs();
      /* FOR DEBUGGING RECOGNIZING TURNS */
//      LCD.clear(); LCD.home();
//      if (negErrCount > 100){
//        LCD.print("Right turn!");
//      } else if (posErrCount > 100) {
//        LCD.print("Left turn!");
//      } else if (noErrCount > 100) {
//        LCD.print("STRAIGHT ;)");
//      }
      /* FOR DEBUGGING THE CURRENT COUNT (AND FRONT QRDS) FOR RECOGNIZING TOP OF RAMP*/
//      LCD.clear(); LCD.home();
//      LCD.print("L: "); LCD.print(leftQRD); LCD.print(" R: "); LCD.print(rightQRD);
//      LCD.setCursor(0,1);
//      LCD.print("Count: "); LCD.print(currCount);
      displayCount = 0; /* RESET */
    }

    lastTurnValue = turnValue;
    lastErr = currErr;
    lastCount = currCount;
  }
}

/*
 * 
 * TAPE FOLLOWING FOR GOING AROUND TANK (AND STOPPING AT TICK MARKS)
 * 
 */
void aroundTank(Menu* menu) {
  
  currCount = 0;
  displayCount = 0;
  int lastTickCount = 0;
  int proportional = 0;
  int integral = 0;
  int derivative = 0;
  int maxIntegral = 60;
  
  while (true) {
    currCount = currCount + 1; 
    displayCount = displayCount + 1;

    /* INPUTS */
    leftQRD = analogRead(LEFT_QRD);
    rightQRD = analogRead(RIGHT_QRD);
    sideQRD = analogRead(SIDE_QRD);

    /* Press YELLOW RESET to switch to user input menu */
    switch0 = digitalRead(YELLOWBUTTON);
    if (switch0 == 0) { 
      delay(1000); 
      if (switch0 == 0) { break; } 
    }

    /* ERROR */
    int currErr = tankError(menu, rightQRD, leftQRD, lastErr);

    /* CORRECTION CALCULATION */
    proportional = currErr * menu->kp_circle;
//    integral = currErr * (currCount - lastCount) * menu->ki + integral;
//    if (integral > maxIntegral) {
//      integral = maxIntegral;
//    } else if (integral < 0){
//      integral = 0;
//    }
    derivative = ((currErr - lastErr) / (currCount - lastCount)) * menu->kd_circle;
    int correction = proportional + integral + derivative;

    /* CORRECTION APPLICATION */
    correctionApplication(menu, correction);

    /* PRINTS INPUTS AND OUTPUTS */
    if(displayCount == 30) {
      printQRDs();
      displayCount = 0; /* RESET */
    }

    /* When side QRD senses tape */
    if(sideQRD > menu->thresh_side && (currCount - lastTickCount) > 1000) {
      motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
      lastTickCount = currCount;
      break;
    }

    lastErr = currErr;
    lastCount = currCount;
  }
}

/*
 * 
 * MISC HELPER FUNCTIONS
 * 
 */
void recoverLostTape(Menu* menu, int currentError, int lastError) {
  if(currentError < 0) {
    motor.speed(LEFT_MOTOR, menu->velocity+25);
    motor.speed(RIGHT_MOTOR, menu->velocity-50);
  } else {
    motor.speed(LEFT_MOTOR, menu->velocity+25);
    motor.speed(RIGHT_MOTOR, menu->velocity-50);
  }
}

/*
 * 
 * TAPE FOLLOWING HELPER FUNCTIONS 
 * 
 */
int tapeError(Menu* menu, int rightQRD, int leftQRD, int lastError) {
  int currentError = 0;
  if (leftQRD > menu->thresh_left && rightQRD > menu->thresh_right) { /* COMPLETELY ON TAPE */
    noErrCount = noErrCount + 1;
    negErrCount = 0;
    posErrCount = 0;
  }
  if (leftQRD < menu->thresh_left && rightQRD > menu->thresh_right) { /* SLIGHTLY LEFT OF TAPE */ 
    currentError = -1;
    if (noErrCount > 0) {
      noErrCount = noErrCount + 1;
    } else {
      negErrCount = negErrCount + 1;
    }
    posErrCount = 0;
    offTapeCount = 0;
  } 
  else if (leftQRD > menu->thresh_left && rightQRD < menu->thresh_right) { /* SLIGHTLY RIGHT OF TAPE */ 
    currentError = 1; 
    offTapeCount = 0;
    if (noErrCount > 0) {
      noErrCount = noErrCount + 1;
    } else {
      posErrCount = posErrCount + 1;
    }
    negErrCount = 0;
  } 
  else if (leftQRD < menu->thresh_left && rightQRD < menu->thresh_right){ /* COMPLETELY OFF TAPE */
    if (lastError < 0) { // OFF TO LEFT
      currentError = -2; 
      negErrCount = negErrCount + 1;
      posErrCount = 0;
      noErrCount = 0;
    } 
    else if(lastError > 0) { // OFF TO RIGHT
      currentError = 2; 
      posErrCount = posErrCount + 1;
      negErrCount = 0;
      noErrCount = 0;
    }
    offTapeCount = offTapeCount + 1;
  }

  return currentError;
}

int tankError(Menu* menu, int rightQRD, int leftQRD, int lastError) {
  int currentError = 0;
  if (leftQRD < menu->thresh_left && rightQRD > menu->thresh_right) { /* SLIGHTLY LEFT OF TAPE */ 
      currentError = -1; 
    } 
  else if (leftQRD > menu->thresh_left && rightQRD > menu->thresh_right) { /* ON TAPE */
    currentError = 0;
  }
  else if (leftQRD > menu->thresh_left && rightQRD < menu->thresh_right) { /* SLIGHTLY RIGHT OF TAPE */ 
    currentError = 1; 
  } 
  else if (leftQRD < menu->thresh_left && rightQRD < menu->thresh_right) { /* COMPLETELY OFF TAPE */
    if(lastError == 0 || lastError > 0) { currentError = 3; } // extremely right of desired position
    else { currentError = -1; }
  }

  return currentError;
}

void correctionApplication(Menu* menu, int correction) {
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
}

void printQRDs() {
  LCD.clear(); LCD.home();
  LCD.print("L: "); LCD.print(analogRead(LEFT_QRD)); LCD.print(", R: "); LCD.print(analogRead(RIGHT_QRD));
  LCD.setCursor(0,1);
  LCD.print(", S: "); LCD.print(analogRead(SIDE_QRD)); LCD.print(" "); LCD.print(digitalRead(RAMP_SWITCH));
}

void printFreq() {
  LCD.clear(); LCD.home();
  LCD.print("L: "); LCD.print(analogRead(LEFT_QRD)); LCD.print(" R: "); LCD.print(analogRead(RIGHT_QRD));
  LCD.setCursor(0,1);
  LCD.print("1: "); LCD.print(analogRead(ONEKHZ)); LCD.print(" 10: "); LCD.print(analogRead(TENKHZ));  
}

