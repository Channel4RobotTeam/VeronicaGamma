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
#include "navigate.h"
#include "tape.h"
#include "constants.h"

int command = 0;
bool leftCourse = true;
Menu* menu = new Menu();

void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);
  getUserInput();
}

void loop() {

  displayMenu(menu);
    
  switch (command) {
    
    case 0: { /* RUN FULL COURSE */

      gateStage();  /* GO FROM START TO START OF TANK */
//      rightTurn(); /* RIGHT TURN ONTO THE TAPE AROUND THE TANK */
      tankStage(); /* GO AROUND TANK AND COLLECT AGENTS */
      int countTo = 1; 
      /* NAVIGATE TO FIRST TICK MARK */
      for (int i = 0; i < countTo; i = i + 1) {
        aroundTank(menu); 
      }
      lineStage(); /* NAVIGATE TO ZIPLINE AND DROP OFF BASKET */
      
    } break;
    case 1: { tapeFollow(menu, false); } break; /* TAPE FOLLOW */
    case 2: { aroundTank(menu); } break; /* CIRCLE FOLLOW */
    case 3: { gateStage(); } break; /* GATE STAGE */
    case 4: { rampStage(); } break; /* RAMP STAGE */
    case 5: { tankStage(); } break; /* TANK STAGE */
    case 6: { lineStage(); } break; /* LINE STAGE */
  }

  getUserInput();
    
}

void getUserInput() {
  motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
  
  while (true) {
    /* use TOP_POT to toggle options, press START to select current option */
    command = knob(TOP_POT) * 7 / 1024;
    LCD.clear(); LCD.home();
    LCD.print("Press START to...");
    LCD.setCursor(0,1);
    switch (command) {
      case 0: { LCD.print("RUN FULL COURSE"); } break;
      case 1: { LCD.print("TAPE FOLLOW"); } break;
      case 2: { LCD.print("CIRCLE FOLLOW"); } break;
      case 3: { LCD.print("GATE STAGE"); } break;
      case 4: { LCD.print("RAMP STAGE"); } break;
      case 5: { LCD.print("TANK STAGE"); } break;
      case 6: { LCD.print("LINE STAGE"); } break;
    }
    if (startbutton()) {
      delay(100); if (startbutton()) { break; }
    }
    delay(100);
  }
  
  if (command == 0 || command == 6) {
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
  
  tapeFollow(menu, true);
  
}

void rampStage() {
  
  tapeFollow(menu, false);

}

void tankStage() {
  
  int tickCount = 0;

  while (tickCount < 6) {

    /* GO TO NEXT TICK */
    aroundTank(menu); 
    tickCount++;
    
    /* PICK UP AGENT */
    raiseArm();
    delay(500);
    closePincer();
    delay(1500);
    lowerArm();
    delay(1000);
    openPincer();
    shake();  // might be unnecessary
    
  }
  
}

void lineStage() {
  
}

