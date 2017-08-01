/********************************/
/*                              */
/*          THE BRAIN!          */
/*                              */
/********************************/

/*
 * 
 * Channel 4 News Team
 * Veronica
 * July 2017
 * 
 * Includes startup code and directs the robot's actions.
 * 
 */

#include <phys253.h>
#include <LiquidCrystal.h>

#include "actuators.h"
#include "tape.h"
#include "constants.h"

int command = 0;
bool leftCourse = true;
Menu* menu = new Menu();
unsigned long start;

void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);
  RCServo2.write(90);
  getUserInput();
}

void loop() {

  displayMenu(menu);
    
  switch (command) {
    
    case 0: { /* RUN FULL COURSE */

      start = millis();

      gateStage();  /* GO FROM START TO START OF TANK */
      rampStage(leftCourse); /* GO THROUGH GATE, UP RAMP, UP TO TANK*/
      if(!leftCourse) {
        driveForward(500.0);
      }
      rightTurnToTape(menu);
      tankStage(); /* GO AROUND TANK AND COLLECT AGENTS */
      int countTo = 0;
      if(leftCourse) { countTo = 2; }
      else { countTo = 4; }
      /* NAVIGATE TO SECOND OR FOURTH TICK MARK */
      for (int i = 0; i < countTo; i = i + 1) {
        aroundTank(menu); 
      }
      if(!leftCourse) {
        rightTurn(menu, 500.0);
      }
      lineStage(); /* NAVIGATE TO ZIPLINE AND DROP OFF BASKET */
      
    } break;
    
    case 1: { tapeFollow(menu, false /*gateStage*/, true /*leftCourse*/); } break; /* TAPE FOLLOW */ 
    
    case 2: { /* CIRCLE FOLLOW */
      while(true) {
        aroundTank(menu); 
        if (digitalRead(0) == 0) { 
          delay(1000); 
          if (digitalRead(0) == 0) { break; } 
        }
      }
    } break; 
    
    case 3: { /* TEST ARM */
      raiseArm();
      delay(500);
      closePincer();
      delay(1500);
      lowerArm();
      delay(1000);
      openPincer();
      shake();  // might be unnecessary
    } break; 
    
    case 4: { gateStage(); } break; /* GATE STAGE */
    
    case 5: { rampStage(leftCourse); } break; /* RAMP STAGE */
    
    case 6: { tankStage(); } break; /* TANK STAGE */
    
    case 7: { lineStage(); } break; /* LINE STAGE */

    case 8: { /* MISC TEST */ 
      rampStage(leftCourse);
      delay(1000);
      driveForward(500.0);
      delay(1000);
      rightTurnToTape(menu);
      delay(1000);
      aroundTank(menu);
//      rightTurn(menu, 500.0);
    } break;
    
  }

  getUserInput();
    
}

void getUserInput() {
  motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
  
  while (true) {
    /* use TOP_POT to toggle options, press START to select current option */
    command = knob(TOP_POT) * 9 / 1024;
    LCD.clear(); LCD.home();
    LCD.print("Press START to...");
    LCD.setCursor(0,1);
    switch (command) {
      case 0: { LCD.print("RUN FULL COURSE"); } break;
      case 1: { LCD.print("TAPE FOLLOW"); } break;
      case 2: { LCD.print("CIRCLE FOLLOW"); } break;
      case 3: { LCD.print("TEST ARM"); } break;
      case 4: { LCD.print("GATE STAGE"); } break;
      case 5: { LCD.print("RAMP STAGE"); } break;
      case 6: { LCD.print("TANK STAGE"); } break;
      case 7: { LCD.print("LINE STAGE"); } break;
      case 8: { LCD.print("MISC TEST"); } break;
    }
    if (startbutton()) {
      delay(100); if (startbutton()) { break; }
    }
    delay(100);
  }
  
  if (command == 0 || command == 5 || command == 7) {
    while (true) {
      /* use TOP_POT to toggle options, press START to select current option */
      leftCourse = 1 - knob(TOP_POT) * 2 / 1024;
      LCD.clear(); LCD.home();
      LCD.print("Using surface...");
      LCD.setCursor(0,1);
      if (leftCourse) {
        LCD.print("LEFT COURSE");
      } else {
        LCD.print("RIGHT COURSE");
      }
      if (startbutton()) {
        delay(100); if (startbutton()) { break; }
      }
      delay(100);
    }
  }
  
}

void gateStage() { 
  
  bool gateStage = true;
  tapeFollow(menu, gateStage, leftCourse /*doesn't matter*/);
  
}

void rampStage(bool leftCourse) {
  
  bool gateStage = false;
  tapeFollow(menu, gateStage, leftCourse);
  
}

void tankStage() {
  
  for(int tickCount = 1; tickCount <= 6; tickCount++) {

    /* Press YELLOW RESET to switch to user input menu */
    int switch0 = digitalRead(YELLOWBUTTON);
    if (switch0 == 0) { 
      delay(1000); 
      if (switch0 == 0) { break; } 
    }

    /* TODO clean this up */
//    bool skipTick = false;
//    if (tickCount = 1 && millis() - start > 63500){
//      skipTick = true;
//    } else if (tickCount = 2 && millis() - start > 68500){
//      skipTick = true;
//    } else if (tickCount = 3 && millis() - start > 73500){
//      skipTick = true;
//    } else if (tickCount = 4 && millis() - start > 78500){
//      skipTick = true;
//    } else if (tickCount = 5 && millis() - start > 83500){
//      skipTick = true;
//    } else if (tickCount = 6 && millis() - start > 98500){
//      skipTick = true;
//    }

    /* GO TO NEXT TICK */
    aroundTank(menu); 
    delay(1000);

//    if (!skipTick){
//    /* PICK UP AGENT */
//      delay(2000);
//      raiseArm();
//      delay(500);
//      closePincer();
//      delay(1500);
//      lowerArm();
//      delay(1000);
//      openPincer();
//    }
    
  }
  
}

void lineStage() { 
  unsigned long duration = 1000.0;
  
  locateZipline(true); /* TRAVELS TOWARDS ZIPLINE FROM APPROPRIATE TICK MARK */
  delay(1000);
  backUp(duration); /* REALIGN */
//  raiseArm(); /* GET ARM OUT OF THE WAY */
  delay(1000);
//  raiseLift(); /* RAISE THE LIFT WITH THE BASKET ON IT */
//  delay(2000);
  driveForward(duration); /* DRIVES THE BASKET ONTO THE ZIPLINE */
//  lowerLift(); /* LOWERS THE LIFT TO RELEASE THE BASKET */
  
}

