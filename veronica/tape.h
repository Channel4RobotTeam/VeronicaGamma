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
int sideRightQRD;
int switch0 = digitalRead(0);

/* TANK VARIABLES */
bool reachedTankVar = false;

/*
 * 
 * REGULAR TAPE FOLLOWING
 *          
 */
void tapeFollow(Menu* menu, bool gateStage, bool leftCourse) {

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
    sideRightQRD = analogRead(SIDE_RIGHT_QRD);

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
        LCD.setCursor(0,1); LCD.print("SR: "); LCD.print(analogRead(SIDE_RIGHT_QRD));
        displayCount = 0; /* RESET */
      }

      /* RECOGNIZE WHEN THE CIRCLE IS REACHED AND TURN ONTO IT */
      if(topOfRamp && sideRightQRD > menu->thresh_sideRight) {
        motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
        LCD.clear(); LCD.home();
        LCD.print("TANK");
        LCD.setCursor(0, 1); LCD.print("SR: "); LCD.print(analogRead(SIDE_RIGHT_QRD));
        delay(4000);
        rightTurn(menu);
        break;
      }
    }
    
    /* PRINTS INPUTS AND OUTPUTS */
    if(displayCount == 30 && !topOfRamp) {
      /* FOR DEBUGGING THE QRDs */
      printQRDs();
      /* FOR DEBUGGING THE CURRENT COUNT (AND FRONT QRDS) FOR RECOGNIZING TOP OF RAMP*/
//      LCD.clear(); LCD.home();
//      LCD.print("L: "); LCD.print(leftQRD); LCD.print(" R: "); LCD.print(rightQRD);
//      LCD.setCursor(0,1);
//      LCD.print("Count: "); LCD.print(currCount);
      displayCount = 0; /* RESET */
    }

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
    /* INPUTS */
    leftQRD = analogRead(LEFT_QRD);
    rightQRD = analogRead(RIGHT_QRD);
    sideQRD = analogRead(SIDE_QRD);
    sideRightQRD = analogRead(SIDE_RIGHT_QRD);

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
 * FOLLOW THE OUTSIDE OF THE CIRCLE
 * 
 */
void circleFollow(Menu* menu){

  /* INITIALIZE VARIABLES */
  currCount = 0;
  displayCount = 0;
  int error = -1;
  int lastError = 0;
  int lastCount = 0;
  int sawLastTick = 0;

  /* MAIN LOOP */
  while (true) {

    /* INCREMENT COUNTERS */
    currCount = currCount + 1;
    displayCount = displayCount + 1;

    /* BREAK IF YELLOW BUTTON PUSHED */
    switch0 = digitalRead(YELLOWBUTTON);
    if (switch0 == 0) { 
      delay(1000); 
      if (switch0 == 0) { break; } 
    }
  
    /* READ INPUTS */
    leftQRD = analogRead(LEFT_QRD);
    rightQRD = analogRead(RIGHT_QRD);
    sideQRD = analogRead(SIDE_QRD);
  
    /* DETERMINE ERROR */
    int thresh = 200;
    if (leftQRD < thresh && rightQRD > thresh){ /* SLIGHTLY LEFT */
      error = -2;  
    } else if (leftQRD > thresh && rightQRD > thresh){ /* ON TAPE */
      error = +1;
    } else if (leftQRD < thresh && rightQRD < thresh){ /* OFF TAPE */
      if (lastError < 0){ /* LAST LEFT */
        error = 0;
      } else if (lastError == 0) { /* LAST RIGHT */
        error = +2;
      }
    } else { /* MUST BE SLIGHTLY RIGHT */
      error = 0;
    }
  
    /* DETERMINE CORRECTION */
    int kp = 20;
    int kd = 35;
    int proportional = error * kp;
    int derivative = ((error - lastError) / (currCount - lastCount)) * kd;
    int corr = proportional + derivative;
  
    /* APPLY CORRECTION */
    int motorSpeed = 100;
    if (abs(corr) > motorSpeed){
      if (corr < 0){
        motor.speed(LEFT_MOTOR, motorSpeed - corr + 11);
        motor.speed(RIGHT_MOTOR, 0);
      } else {
        motor.speed(LEFT_MOTOR, 0);
        motor.speed(RIGHT_MOTOR, motorSpeed + corr);
      }
    } else {
      motor.speed(LEFT_MOTOR, motorSpeed - corr + 11);
      motor.speed(RIGHT_MOTOR, motorSpeed + corr);
    }

    /* STOP AT TICKS */
    if (sideQRD > 150 && currCount - sawLastTick > 2000){
      motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
      LCD.clear(); LCD.home();
      LCD.print("AT TICK MARK");
      delay(2000);

      /* LINE UP WITH TOY FOR ARM */
      LCD.setCursor(0,1);
      LCD.print("CORRECTING");
      leftQRD = analogRead(LEFT_QRD);
      rightQRD = analogRead(RIGHT_QRD);
      unsigned long startCorr = millis();
      while (!(leftQRD > thresh && rightQRD < thresh)){
        leftQRD = analogRead(LEFT_QRD);
        rightQRD = analogRead(RIGHT_QRD);
//        motor.speed(LEFT_MOTOR, motorSpeed - 45);
        motor.speed(LEFT_MOTOR, 0);
        motor.speed(RIGHT_MOTOR, motorSpeed + 45);
        currCount = currCount + 1;
      }
      backUp(350.0);
      motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0); /* PAUSE -- ARM MOVEMENTS WOULD GO HERE */
      delay(2000);
        
      sawLastTick = currCount;  
      
    }
    
    /* PRINT VALUES */
    if (displayCount == 30){
      LCD.clear(); LCD.home();
      LCD.print("Err: "); LCD.print(error);
      LCD.setCursor(0,1);
      LCD.print("L: "); LCD.print(leftQRD); LCD.print(" R: "); LCD.print(rightQRD);
      displayCount = 0;
    }
  
    /* UPDATE LAST VALUES */
    lastError = error;
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
    else { currentError = -2; }
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
  LCD.print("SR:"); LCD.print(analogRead(SIDE_RIGHT_QRD)); LCD.print(", S: "); LCD.print(analogRead(SIDE_QRD)); LCD.print(" "); LCD.print(digitalRead(RAMP_SWITCH));
}

void printFreq() {
  LCD.clear(); LCD.home();
  LCD.print("L: "); LCD.print(analogRead(LEFT_QRD)); LCD.print(" R: "); LCD.print(analogRead(RIGHT_QRD));
  LCD.setCursor(0,1);
  LCD.print("1: "); LCD.print(analogRead(ONEKHZ)); LCD.print(" 10: "); LCD.print(analogRead(TENKHZ));  
}

