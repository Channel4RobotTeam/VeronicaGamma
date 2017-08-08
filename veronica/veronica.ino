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
  RCServo1.write(10);
  analogWrite(37,0);
  getUserInput();
}

void loop() {

  displayMenu(menu);
    
  switch (command) {
    
    case 0: { /* RUN FULL COURSE */

      start = millis();

      gateStage();  /* GO FROM START TO START OF TANK */
      rampStage(); /* GO THROUGH GATE, UP RAMP, UP TO TANK*/
      if(!leftCourse) {
        driveForward(400.0);
        rightTurnToTape(menu, 1);
      } else {
        driveForward(400.0);
        motor.speed(LEFT_MOTOR, 35); motor.speed(RIGHT_MOTOR, -115);
        delay(1000);
        rightTurnToTape(menu, 2);
      }
      tankStage(); /* GO AROUND TANK AND COLLECT AGENTS */
      if(!leftCourse) { /* TURN AROUND */
        rightTurnToTape(menu, 1);
        rightTurnToTape(menu, 1);
        rightTurnToTape(menu, 1);
      }
      int countTo = 2;
//      if(leftCourse) { countTo = 2; }
//      else { countTo = 4; }
      /* NAVIGATE TO SECOND OR FOURTH TICK MARK */
      for (int i = 0; i < countTo; i = i + 1) {
        circleFollow(menu); 
      }
//      if(!leftCourse) {
//        rightTurn(menu, 5000.0);
//      }
      lineStage(); /* NAVIGATE TO ZIPLINE AND DROP OFF BASKET */
      
    } break;
    
    case 1: { tapeFollow(menu, false /*gateStage*/); } break; /* TAPE FOLLOW */ 
    
    case 2: { /* CIRCLE FOLLOW */
      while(true) {
        circleFollow(menu); 
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
    } break; 
    
    case 4: { gateStage(); } break; /* GATE STAGE */
    
    case 5: { rampStage(); } break; /* RAMP STAGE */
    
    case 6: { tankStage(); } break; /* TANK STAGE */
    
    case 7: { lineStage(); } break; /* LINE STAGE */

    case 8: { /* MISC TEST */ 
        
      tankStage(); /* GO AROUND TANK AND COLLECT AGENTS */
      if(!leftCourse) { /* TURN AROUND */
        rightTurnToTape(menu, 1);
        rightTurnToTape(menu, 1);
        rightTurnToTape(menu, 1);
      }
      int countTo = 2;
      for (int i = 0; i < countTo; i = i + 1) {
        circleFollow(menu); 
      }
      lineStage(); /* NAVIGATE TO ZIPLINE AND DROP OFF BASKET */

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
  
  if (command == 0 || command == 5) {
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
  tapeFollow(menu, gateStage);
  
}

void rampStage() {
  
  bool gateStage = false;
  tapeFollow(menu, gateStage);
  
}

void tankStage() {
  
  for(int tickCount = 1; tickCount <= 6; tickCount++) {

    /* Press YELLOW RESET to switch to user input menu */
    int switch0 = digitalRead(YELLOWBUTTON);
    if (switch0 == 0) { 
      delay(1000); 
      if (switch0 == 0) { break; } 
    }

//    bool skipTick = skipTickBool(tickCount);
//    LCD.clear(); LCD.home();
//    LCD.print("Skip Tick? "); LCD.print(skipTick);
//    delay(2000);

    /* GO TO NEXT TICK */
    circleFollow(menu); 
    delay(500);

//    if (!skipTick){
    /* PICK UP AGENT */
      raiseArm();
      delay(100);
      closePincer();
      delay(500);
      lowerArm();
      delay(500);
      openPincer();
//    }
    
  }
  
}


void lineStage() {
   
//  locateZipline(); /* TRAVELS TOWARDS ZIPLINE FROM APPROPRIATE TICK MARK */
//  driveForward(2300.0); /* DRIVES FORWARD A HARDCODED DISTANCE TO ZIPLINE */
  rightTurn(menu, 600.0); driveForward(200.0); leftTurn(menu, 550.0);
  driveForward(1500.0);
  delay(1000);
  raiseArm(); /* GET ARM OUT OF THE WAY */
  delay(1000);
  raiseLift(); /* RAISE THE LIFT WITH THE BASKET ON IT */
  delay(2000);
  driveForward(2000.0); /* DRIVES THE BASKET ONTO THE ZIPLINE */
  lowerLift(); /* LOWERS THE LIFT TO RELEASE THE BASKET */
  backUp(5900.0); /* GETS OUT OF THE WAY OF THE BASKET'S DESCENT */
  lowerArm();
  
}


bool skipTickBool(int tickCount) {
  
    unsigned long firstDropTime = 60000.0;

    if((millis() - start) > (firstDropTime + 5.0 * (unsigned long) tickCount)) {
      return true;
    } else {
      return false;
    }
    
}


