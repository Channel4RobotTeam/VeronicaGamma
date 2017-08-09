#pragma once

#include <phys253.h>
#include <LiquidCrystal.h>

#include "constants.h"

class Menu {
  public:
    int velocity;
    int kp;
    int ki;
    int kd;
    int thresh;
    int thresh_back;
    int thresh_side;
    int thresh_onekhz;
    int thresh_tenkhz;
    int numMenuItems;
    String menuItemNames[13] = {"vel", "kp", "ki", "kd", "thresh", "thresh_b", "thresh_s", "onekhz", "tenkhz"};
    Menu(){
      velocity = VELOCITY;
      kp = KP;
      ki = KI;
      kd = KD;
      thresh = THRESH;
      thresh_back = THRESH_BACK;
      thresh_side = THRESH_SIDE;
      thresh_onekhz = THRESH_ONEKHZ;
      thresh_tenkhz = THRESH_TENKHZ;
      numMenuItems = 9;
    }
};

/* FUNCTIONS */
void displayMenu(Menu* veronicaMenu) {
  motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0); 
  
  while (true) {

    /* press STOP to go to start course */
    if (stopbutton()){
      delay(1500); if (stopbutton()) { break; }
    }

    /* scroll with TOP_POT , change value with BOT_POT */
    /* PRINT PROMPTS */
    int menuIndex = knob(TOP_POT) * veronicaMenu->numMenuItems / 1024;
    int newVal = knob(BOT_POT);
    LCD.clear(); LCD.home();
    LCD.print(veronicaMenu->menuItemNames[menuIndex]); LCD.print(": ");
    switch (menuIndex) {
      case 0: { LCD.print(veronicaMenu->velocity); } break;
      case 1: { LCD.print(veronicaMenu->kp); } break;
      case 2: { LCD.print(veronicaMenu->ki); } break;
      case 3: { LCD.print(veronicaMenu->kd); } break;
      case 4: { LCD.print(veronicaMenu->thresh); } break;
      case 5: { LCD.print(veronicaMenu->thresh_back); } break;
      case 6: { LCD.print(veronicaMenu->thresh_side); } break;
      case 7: { LCD.print(veronicaMenu->thresh_onekhz); } break;
      case 8: { LCD.print(veronicaMenu->thresh_tenkhz); } break;
    }
    LCD.setCursor(0,1);
    LCD.print("Set as: "); LCD.print(newVal);

    delay(100);

    /* press START to save value */
    if (startbutton()) {
      delay(100);
      if (startbutton()) {
        switch (menuIndex) { 
          case 0: { veronicaMenu->velocity = newVal; } break;
          case 1: { veronicaMenu->kp = newVal; } break;
          case 2: { veronicaMenu->ki = newVal; } break;
          case 3: { veronicaMenu->kd = newVal; } break;
          case 4: { veronicaMenu->thresh = newVal; } break;
          case 5: { veronicaMenu->thresh_back = newVal; } break;
          case 6: { veronicaMenu->thresh_side = newVal; } break;
          case 7: { veronicaMenu->thresh_onekhz = newVal; } break;
          case 8: { veronicaMenu->thresh_tenkhz = newVal; } break;
        }
      }
      delay(1000);
      if (startbutton()){
        break;
      }
    }
  }
  
}
