#include <phys253.h>
#include <LiquidCrystal.h>

#include "constants.h"

/* FUNCTION DECLARATIONS */
void menu();

/* FUNCTIONS */
void menu() {
  motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
  
  while (true) {

    /* press STOP to go to start course */
    if (stopbutton()){
      delay(100); if (stopbutton()) { break; }
    }

    /* scroll with TOP_POT  */

    
  }
}

