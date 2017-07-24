#include <phys253.h>
#include <LiquidCrystal.h>

#include "constants.h"

class Menu {
  public:
    int velocity;
    int kp;
    int ki;
    int kd;
    int kp_circle;
    int ki_circle;
    int kd_circle;
    int thresh_left;
    int thresh_right;
    int thresh_side;
    int thresh_front;
    int thresh_onekhz;
    int thresh_tenkhz;
    int numMenuItems;
    String menuItemNames[13] = {"vel", "kp", "ki", "kd", "kp_c", "ki_c", "kd_c", "thresh_l", "thresh_r", "thresh_s", "thresh_f", "onekhz", "tenkhz"};
    Menu(){
      velocity = VELOCITY;
      kp = KP;
      ki = KI;
      kd = KD;
      kp_circle = KP_CIRCLE;
      ki_circle = KI_CIRCLE;
      kd_circle = KD_CIRCLE;
      thresh_left = THRESH_LEFT;
      thresh_right = THRESH_RIGHT;
      thresh_side = THRESH_SIDE;
      thresh_front = THRESH_FRONT;
      thresh_onekhz = THRESH_ONEKHZ;
      thresh_tenkhz = THRESH_TENKHZ;
      numMenuItems = 13;
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
      case 4: { LCD.print(veronicaMenu->kp_circle); } break;
      case 5: { LCD.print(veronicaMenu->ki_circle); } break;
      case 6: { LCD.print(veronicaMenu->kd_circle); } break;
      case 7: { LCD.print(veronicaMenu->thresh_left); } break;
      case 8: { LCD.print(veronicaMenu->thresh_right); } break;
      case 9: { LCD.print(veronicaMenu->thresh_side); } break;
      case 10: { LCD.print(veronicaMenu->thresh_front); } break;
      case 11: { LCD.print(veronicaMenu->thresh_onekhz); } break;
      case 12: { LCD.print(veronicaMenu->thresh_tenkhz); } break;
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
          case 4: { veronicaMenu->kp_circle = newVal; } break;
          case 5: { veronicaMenu->ki_circle = newVal; } break;
          case 6: { veronicaMenu->kd_circle = newVal; } break;
          case 7: { veronicaMenu->thresh_left = newVal; } break;
          case 8: { veronicaMenu->thresh_right = newVal; } break;
          case 9: { veronicaMenu->thresh_side = newVal; } break;
          case 10: { veronicaMenu->thresh_front = newVal; } break;
          case 11: { veronicaMenu->thresh_onekhz = newVal; } break;
          case 12: { veronicaMenu->thresh_tenkhz = newVal; } break;
        }
      }
      delay(1000);
      if (startbutton()){
        break;
      }
    }
  }
  
}
