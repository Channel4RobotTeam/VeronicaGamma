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

  /* GET USER INPUT */  
  while (true) {
    /* press STOP to toggle options, press START to select current option */
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
    if (stopbutton()) {
      delay(100); if (stopbutton()) { command = command + 1; }
    }
    if (startbutton()) {
      delay(100); if (startbutton()) { break; }
    }
    delay(100);
  }
  if (command == 0 || command == 6) {
    while (true) {
      /* press STOP to toggle options, press START to select current option */
      LCD.clear(); LCD.home();
      LCD.print("Using surface...");
      LCD.setCursor(0,1);
      if (leftCourse) {
        LCD.print("LEFT COURSE");
      } else {
        LCD.print("RIGHT COURSE");
      }
      if (stopbutton()) {
        delay(100); if (stopbutton()) { leftCourse = !leftCourse; };
      }
      if (startbutton()) {
        delay(100); if (stopbutton()) { break; }
      }
      delay(100);
    }
  }
}

void loop() {
    
    switch (command) {
      
      case 0: { // FULL COURSE
        
      } break;
      
      case 1: { // TAPE FOLLOW
        
        /* CALL MENU */
        displayMenu(menu);
        /* TAPE FOLLOW */
        tapeFollow(menu);
        
      } break;
      
      case 2: { // CIRCLE FOLLOW
        
        /* CALL MENU */
        displayMenu(menu);
        /* TAPE FOLLOW */
        aroundTank(menu);
        
      } break;
      
      case 3: { // GATE STAGE
        
      } break;
      
      case 4: { // RAMP STAGE
        
      } break;
      
      case 5: { // TANK STAGE

        /* CALL MENU */
        displayMenu(menu);

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
        
      } break;
      
      case 6: { // LINE STAGE
        
      } break;
    }

}
