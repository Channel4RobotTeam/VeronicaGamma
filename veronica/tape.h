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
void circleFollow(Menu* menu);

/* HELPER FUNCTIONS */
int tapeError(Menu* menu, int rightQRD, int leftQRD, int lastError); 
void correctionApplication(Menu* menu, int correction); 
void printQRDs(); 
void printFreq();

/* QRDs and YELLOW button */
int leftQRD;
int rightQRD;
int sideQRD;
int sideRightQRD;
int switch0 = digitalRead(0);


/*
 * 
 * REGULAR TAPE FOLLOWING
 *          
 */
void tapeFollow(Menu* menu, bool gateStage) {

  /* TAPE FOLLOWING VARIABLES */
  int currCount = 0;
  int displayCount = 0;
  int lastCount = 0;
  
  int lastErr = 0;
  
  bool topOfRamp = false;
  
  unsigned long startTime = millis();
  unsigned long reachedTopOfRamp;
  
  int proportional = 0;
  int derivative = 0;
  int currErr = 0;
  
  /* MAIN LOOP */
  while (true) {
    int startLoop = millis();
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
   if(leftQRD > menu->thresh_left && rightQRD > menu->thresh_right) {
      currErr = 0;
   }
   if (leftQRD < menu->thresh_left && rightQRD > menu->thresh_right) { /* SLIGHTLY LEFT OF TAPE */ 
      currErr = -1;
    } 
    else if (leftQRD > menu->thresh_left && rightQRD < menu->thresh_right) { /* SLIGHTLY RIGHT OF TAPE */ 
      currErr = 1; 
    } 
    else if (leftQRD < menu->thresh_left && rightQRD < menu->thresh_right) { /* COMPLETELY OFF TAPE */
      if (lastErr < 0) { // OFF TO LEFT
        currErr = -3; 
      } 
      else if(lastErr > 0) { // OFF TO RIGHT
        currErr = 3; 
      }
    }
    
    /* CORRECTION CALCULATION */
    proportional = currErr * menu->kp;
//    derivative = ((currErr - lastErr) / (currCount - lastCount)) * menu->kd;
    int correction = proportional; // + derivative;

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

    if (gateStage) { /* GATE STAGE */
      
      /* STAY STOPPED WHILE THE ALARM IS ON */
      int j = 0;
      while(analogRead(ONEKHZ) < 30) {
        j = j + 1;
        
        if(j == 30) {
          LCD.clear(); LCD.home();
          LCD.print("waiting...");
          LCD.setCursor(0, 1);
          LCD.print("1kHz: "); LCD.print(analogRead(ONEKHZ));
        }
        
        motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
      }
      if(analogRead(ONEKHZ) > 85) {
        break;
      }
    } else { /* RAMP STAGE */
      
      /* RECOGNIZE WHEN THE TOP OF THE RAMP IS REACHED */
      if (digitalRead(RAMP_SWITCH) == 0) {
        topOfRamp = true;
        reachedTopOfRamp = millis();
      }

      if (topOfRamp && (millis() - reachedTopOfRamp) > 250.0){
        menu->velocity = 150; /* lower the speed for tape following around top */
//        leftTurnToTape(menu, 1);
      }

      /* RECOGNIZE WHEN THE CIRCLE IS REACHED AND TURN ONTO IT */
      if(topOfRamp && sideRightQRD > menu->thresh_sideRight && (millis() - reachedTopOfRamp) > 2000.0) {
        motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
        LCD.clear(); LCD.home();
        LCD.print("TANK"); 
        LCD.setCursor(0, 1); LCD.print("SR: "); LCD.print(analogRead(SIDE_RIGHT_QRD));
        break;
      }
      
      if (displayCount == 300 && topOfRamp) {
        LCD.clear(); LCD.home(); 
        LCD.print("RAMP TOP  "); LCD.print((millis()/1000) - reachedTopOfRamp);
        LCD.setCursor(0,1); LCD.print("SR: "); LCD.print(analogRead(SIDE_RIGHT_QRD));
        displayCount = 0; /* RESET */
      }
    }
    
    /* PRINTS INPUTS AND OUTPUTS */
    if(displayCount == 300 && !topOfRamp) {
//      /* FOR DEBUGGING THE QRDs */
      printQRDs();
//        printFreq();
      /* FOR DEBUGGING THE CURRENT COUNT (AND FRONT QRDS) FOR RECOGNIZING TOP OF RAMP*/
//      LCD.clear(); LCD.home();
//      LCD.print("L: "); LCD.print(analogRead(LEFT_QRD) > menu->thresh_left); LCD.print(" R: "); LCD.print(analogRead(RIGHT_QRD) > menu->thresh_right);
//      LCD.setCursor(0,1);
//      LCD.print("loop time: "); LCD.print((millis() - startLoop));
//      Serial.print("loop time: "); Serial.println(millis() - startLoop);
//      Serial.print("left: "); Serial.print(analogRead(LEFT_QRD) > menu->thresh_left);

      displayCount = 0; /* RESET */
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
void circleFollow(Menu* menu, int tickCount) {

  /* INITIALIZE VARIABLES */
  int currCount = 0;
  int displayCount = 0;
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
    if (leftQRD < thresh && rightQRD > thresh) { /* SLIGHTLY LEFT */
      error = -2;  
    } else if (leftQRD > thresh && rightQRD > thresh) { /* ON TAPE */
      error = +1;
    } else if (leftQRD < thresh && rightQRD < thresh) { /* OFF TAPE */
      if (lastError < 0) { /* LAST LEFT */
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
    if (abs(corr) > motorSpeed) {
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
    if (sideQRD > 150 && currCount - sawLastTick > 2000) {
      motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
      LCD.clear(); LCD.home();
      LCD.print("AT TICK MARK");

      /* LINE UP WITH TOY FOR ARM */
      if (tickCount != 1){
        LCD.setCursor(0,1);
        LCD.print("CORRECTING");
        leftQRD = analogRead(LEFT_QRD);
        rightQRD = analogRead(RIGHT_QRD);
        unsigned long startCorr = millis();
        while (!(leftQRD > thresh && rightQRD < thresh)){
          leftQRD = analogRead(LEFT_QRD);
          rightQRD = analogRead(RIGHT_QRD);
          motor.speed(LEFT_MOTOR, 0);
          motor.speed(RIGHT_MOTOR, motorSpeed + 45);
          currCount = currCount + 1;
        }
        backUp(400.0);
        motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0); /* PAUSE -- ARM MOVEMENTS WOULD GO HERE */
      }
        
      sawLastTick = currCount;  

      break;
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
 * FOLLOW THE OUTSIDE OF THE CIRCLE IN THE CLOCKWISE DIRECTION
 * 
 */
void clockwiseCircleFollow(Menu* menu) {

  /* INITIALIZE VARIABLES */
  int currCount = 0;
  int displayCount = 0;
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
    int thresh = menu->thresh_left;
    if (leftQRD > thresh && rightQRD < thresh) { /* SLIGHTLY RIGHT */
      error = +2;  
    } else if (leftQRD > thresh && rightQRD > thresh) { /* ON TAPE */
      error = +1;
    } else if (leftQRD < thresh && rightQRD < thresh) { /* OFF TAPE */
      if (lastError > 0) { /* LAST RIGHT */
        error = 0;
      } else if (lastError == 0) { /* LAST LEFT */
        error = -2;
      }
    } else { /* MUST BE SLIGHTLY LEFT */
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
    if (abs(corr) > motorSpeed) {
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
    if (sideQRD > 150 && currCount - sawLastTick > 2000) {
      motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
      LCD.clear(); LCD.home();
      LCD.print("AT TICK MARK");

      /* LINE UP WITH TOY FOR ARM */
      LCD.setCursor(0,1);
      LCD.print("CORRECTING");
      leftQRD = analogRead(LEFT_QRD);
      rightQRD = analogRead(RIGHT_QRD);
      unsigned long startCorr = millis();
      while (!(leftQRD < thresh && rightQRD > thresh)){
        leftQRD = analogRead(LEFT_QRD);
        rightQRD = analogRead(RIGHT_QRD);
        motor.speed(LEFT_MOTOR, motorSpeed + 45);
        motor.speed(RIGHT_MOTOR, 0);
        currCount = currCount + 1;
      }
      backUp(400.0);
      motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0); /* PAUSE -- ARM MOVEMENTS WOULD GO HERE */
        
      sawLastTick = currCount;  

      break;
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
 * TAPE FOLLOWING HELPER FUNCTIONS 
 * 
 */
int tapeError(Menu* menu, int rightQRD, int leftQRD, int lastError) {
  int currentError = 0;
  
  if (leftQRD < menu->thresh_left && rightQRD > menu->thresh_right) { /* SLIGHTLY LEFT OF TAPE */ 
    currentError = -5;
  } 
  else if (leftQRD > menu->thresh_left && rightQRD < menu->thresh_right) { /* SLIGHTLY RIGHT OF TAPE */ 
    currentError = 5; 
  } 
  else if (leftQRD < menu->thresh_left && rightQRD < menu->thresh_right) { /* COMPLETELY OFF TAPE */
    if (lastError < 0) { // OFF TO LEFT
      currentError = -10; 
    } 
    else if(lastError > 0) { // OFF TO RIGHT
      currentError = 10; 
    }
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
  LCD.print("1: "); LCD.print(analogRead(ONEKHZ)); LCD.print(" 10: "); LCD.print(analogRead(TENKHZ)); LCD.print(" "); LCD.print(digitalRead(RAMP_SWITCH));  
}

