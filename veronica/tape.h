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

/* HELPER FUNCTIONS */
int tapeError(Menu* menu, int rightQRD, int leftQRD, int lastError); 
int tankError(Menu* menu, int rightQRD, int leftQRD, int lastError); 
void correctionApplication(Menu* menu, int correction); 
void printToLCD(int rightQRD, int leftQRD, int sideQRD, int wheelQRD); 

/* TAPE FOLLOWING VARIABLES */
int lastErr = 0;
int currCount = 0;
int lastCount = 0;
int displayCount = 0;
int onTapeCount = 0;

// QRDs & button
int leftQRD;
int rightQRD;
int sideQRD;
int frontQRD;
int switch0 = digitalRead(0);

/* DISTANCE TRACKING VARIABLES */
// if distance tracking is not needed inside aroundTank() then put these variables in tapeFollow function
int wheelQRD;
double distance = 0;
int wheelCount = 0;
int wheelRevolutions = 0;

/* TANK VARIABLES */
bool reachedTankVar = false;

void tapeFollow(Menu* menu, bool gateStage) {
  
//  /* WAIT UNTIL VERONICA IS ON TAPE */
//  LCD.clear(); LCD.home();
//  LCD.print("Place on tape!");
//  LCD.setCursor(0,1);
//  LCD.print("L: "); LCD.print(analogRead(LEFT_QRD)); LCD.print(" R: "); LCD.print(analogRead(RIGHT_QRD));
//  while(analogRead(LEFT_QRD) < THRESH_LEFT && analogRead(RIGHT_QRD) < THRESH_RIGHT){
//    motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
//    LCD.setCursor(0,1);
//    LCD.print("L: "); LCD.print(analogRead(LEFT_QRD)); LCD.print(" R: "); LCD.print(analogRead(RIGHT_QRD));
//    delay(10);
//  }
  
  currCount = 0;
  displayCount = 0;
  distance = 0;
  wheelRevolutions = 0;
  wheelCount = 0;
  while (true) {
    currCount = currCount + 1;
    displayCount = displayCount + 1;
    
    /* Press YELLOW RESET to switch to user input menu */
    switch0 = digitalRead(0);
    if (switch0 == 0) { 
      delay(1000); 
      if (switch0 == 0) { break; } 
    }
//    if (stopbutton()) {
//      delay(1000);
//      if (stopbutton()) {
//        motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
//        LCD.clear(); LCD.home();
//        LCD.print("Press START to");
//        LCD.setCursor(0,1);
//        LCD.print("go to menu");
//        int i = 0;
//        for (int i = 0; i < 3000; i = i + 1) {
//          if (startbutton()) {
//            delay(100);
//            if (startbutton()) { displayMenu(menu); displayCount = 0; }
//          }
//          delay(1); 
//        }
//      }
//    }

    /* INPUTS */
    leftQRD = analogRead(LEFT_QRD);
    rightQRD = analogRead(RIGHT_QRD);
    sideQRD = analogRead(SIDE_QRD);
    wheelQRD = analogRead(WHEEL_QRD);

    /* ERROR */
    int currErr = tapeError(menu, rightQRD, leftQRD, lastErr);
    
    /* CORRECTION CALCULATION */
    int proportional = currErr * menu->kp;
    int integral = 0 * menu->ki;
    int derivative = ((currErr - lastErr) / (currCount - lastCount)) * menu->kd;
    int correction = proportional + integral + derivative;

    /* CORRECTION APPLICATION */
    correctionApplication(menu, correction);
    if (gateStage) {
      /* STAY STOPPED WHILE THE ALARM IS ON */
      // 1kHz high is disarmed, 10kHz high is alarmed
      if(analogRead(ONEKHZ) < 100 || (analogRead(ONEKHZ) > 600 && analogRead(ONEKHZ) < 1023) /*&& analogRead(TENKHZ) > menu->thresh_tenkhz*/) { //change back to WHILE later
        motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
        LCD.clear(); LCD.home();
        LCD.print("WAITING...");
        delay(3000);
        gateStage = false; //TODO: change to a break statement
      }
    }

    /* When wheel QRD sees white tape */
    if(wheelQRD < THRESH_WHEEL && (currCount - wheelCount) > 100) {
      wheelRevolutions = wheelRevolutions + 1;
      double circumference = 2 * PI * WHEEL_RADIUS;
      distance = wheelRevolutions * circumference;
      wheelCount = currCount;
    }
    
    /* PRINTS INPUTS AND OUTPUTS */
    if(displayCount == 30) {
      printToLCD(rightQRD, leftQRD, sideQRD, wheelQRD);
      displayCount = 0; /* RESET */
    }
  
    lastErr = currErr;
    lastCount = currCount;
  }
}

void aroundTank(Menu* menu) {

  /* WAIT UNTIL VERONICA IS ON TAPE */
  LCD.clear(); LCD.home();
  LCD.print("Place Veronica");
  LCD.setCursor(0,1);
  LCD.print("on tape!");
  while(analogRead(LEFT_QRD) < THRESH_LEFT && analogRead(RIGHT_QRD) < THRESH_RIGHT){
    motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
  }
  
  currCount = 0;
  displayCount = 0;
  int lastTickCount = 0;
  while (true) {
    currCount = currCount + 1; 
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
//    if (stopbutton()) {
//      delay(1000);
//      if (stopbutton()){
//        displayMenu(menu);
//        displayCount = 0;
//      }
//    }

    /* ERROR */
    int currErr = tankError(menu, rightQRD, leftQRD, lastErr);

    /* CORRECTION CALCULATION */
    int proportional = currErr * menu->kp_circle;
    int integral = 0 * menu->ki_circle;
    int derivative = ((currErr - lastErr) / (currCount - lastCount)) * menu->kd_circle;
    int correction = proportional + integral + derivative;

    /* CORRECTION APPLICATION */
    correctionApplication(menu, correction);

    /* PRINTS INPUTS AND OUTPUTS */
    if(displayCount == 30) {
      LCD.clear(); LCD.home();
      LCD.print("L: "); LCD.print(leftQRD); LCD.print(" R: "); LCD.print(rightQRD);
      LCD.setCursor(0,1);
      LCD.print("S: "); LCD.print(sideQRD); LCD.print(" W: "); LCD.print(correction);
      displayCount = 0; /* RESET */
    }

    /* When side QRD senses tape */
    if(sideQRD > menu->thresh_side && (currCount - lastTickCount) > 1000) {
      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      lastTickCount = currCount;
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

/* HELPER FUNCTIONS FOR TAPE FOLLOWING */
int tapeError(Menu* menu, int rightQRD, int leftQRD, int lastError) {
  int currentError = 0;
  if (leftQRD < menu->thresh_left && rightQRD > menu->thresh_right) { /* SLIGHTLY LEFT OF TAPE */ 
      currentError = -1;
  } 
  else if (leftQRD > menu->thresh_left && rightQRD < menu->thresh_right) { /* SLIGHTLY RIGHT OF TAPE */ 
    currentError = +1; 
  } 
  else if (leftQRD < menu->thresh_left && rightQRD < menu->thresh_right){ /* COMPLETELY OFF TAPE */
    if (lastError < 0) { // OFF TO LEFT
      currentError = -2; 
    } 
    else if(lastError > 0) { // OFF TO RIGHT
      currentError = +2; 
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
    else { currentError = 0; }
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

void printToLCD(int rightQRD, int leftQRD, int sideQRD, int wheelQRD) {
  LCD.clear(); LCD.home();
  LCD.print("L: "); LCD.print(leftQRD); LCD.print(" R: "); LCD.print(rightQRD);
  LCD.setCursor(0,1);
  LCD.print("W: "); LCD.print(wheelQRD); LCD.print("d: "); LCD.print(distance);
//LCD.print("1: "); LCD.print(analogRead(ONEKHZ)); LCD.print("10: "); LCD.print(analogRead(TENKHZ));
}

