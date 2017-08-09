#include <phys253.h>
#include <LiquidCrystal.h>

#include "actuators.h"
#include "tape.h"
#include "constants.h"

/* FUNCTION DECLARATIONS */
void leftHandCourse();
void rightHandCourse();
void gateStage();
void rampStage();
void tankStage();
void lineStage();

bool leftCourse = true;
Menu* menu = new Menu();
unsigned long start;

void setup() {
  
  #include <phys253setup.txt>
  Serial.begin(9600);

  /* set motors to neutral positions */
  motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
  RCServo1.write(10);
  analogWrite(37,0);

  /* get left or right course */
  LCD.clear(); LCD.home();
  LCD.print("<stop> for LHS");
  LCD.setCursor(0,1);
  LCD.print("<start> for RHS");
  while(true){
    if (stopbutton()){
      delay(100);
      if (stopbutton()){
        leftCourse = true;
        break;
      }
    }
    if (startbutton()){
      delay(100);
      if (startbutton()){
        leftCourse = false;
        break;
      }
    }
  }

  /* get start time */
  start = millis();
  
}

void loop() {

  LCD.clear(); LCD.home();

  /* call the course code */
  if (leftCourse){
    LCD.print(" running LHS"); 
    leftHandCourse(); 
  } else { 
    LCD.print("running RHS");
    rightHandCourse(); 
  }

  /* set motors to neutral positions */
  motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);
  RCServo1.write(10);
  analogWrite(37,0);

  /* get left or right course */
  LCD.clear(); LCD.home();
  LCD.print("<stop> for LHS");
  LCD.setCursor(0,1);
  LCD.print("<start> for RHS");
  while (true) {
    if (stopbutton()){
      delay(100);
      if (stopbutton()){
        leftCourse = true;
        break;
      }
    }
    if (startbutton()){
      delay(100);
      if (startbutton()){
        leftCourse = false;
        break;
      }
    }
  }

}

/*
 * 
 * RUN THE FULL COURSE ON THE LEFT HAND SURFACE
 * 
 */
void leftHandCourse(){
  
  /* detect gate IR */
  gateStage();
  
  /* detect top of ramp and circle*/
  rampStage();

  /* turn onto circle */
  driveForward(460.0);
  rightTurnToTape(menu, 1);
  leftQRD = analogRead(LEFT_QRD);
  rightQRD = analogRead(RIGHT_QRD);
  rightTurn(menu, 300.0);
  while(!(leftQRD > menu->thresh)){
    motor.speed(LEFT_MOTOR, 80); motor.speed(RIGHT_MOTOR, 100);
    leftQRD = analogRead(LEFT_QRD);
    rightQRD = analogRead(RIGHT_QRD);
  }
  motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);

  /* skip first 5 ticks to get to first agent */
  int skipTicks = 5;
  for (int i = 0; i < skipTicks; i = i + 1){
    circleFollow(menu);
  }

  /* go around tank and collect agents */
  tankStage();

  /* go to tick to start zipline escape */
  int countTo = 2;
  for (int i = 0; i < countTo; i = i + 1){
    circleFollow(menu);
  }

  /* drive basket onto zipline */
  lineStage();
  
}

/*
 * 
 * RUN THE FULL COURSE ON THE RIGHT HAND SURFACE
 * 
 */
void rightHandCourse(){

  /* detect gate IR */
  gateStage();
  
  /* detect top of ramp and circle*/
  rampStage();

  /* turn onto circle */
  driveForward(350.0);
  rightTurnToTape(menu, 1);
  motor.speed(LEFT_MOTOR, 0); motor.speed(RIGHT_MOTOR, 0);

  /* go around tank and collect agents */
  tankStage();

  /* turn around */
  rightTurnToTape(menu, 1);

  /* go to tick to start zipline escape */
  int countTo = 2;
  for (int i = 0; i < countTo; i = i + 1){
    clockwiseCircleFollow(menu);
  }

  /* drive basket onto zipline */
  lineStage();
  
}

/*
 * 
 * STOP FOR THE IR GATE
 * 
 */
void gateStage() { 
  
  bool gateStage = true;
  tapeFollow(menu, gateStage);
  
}

/*
 * 
 * RECOGNIZE THE TOP OF THE RAMP AND THE START OF THE CIRCLE
 * 
 */
void rampStage() {
  
  bool gateStage = false;
  tapeFollow(menu, gateStage);
  
}

/*
 * 
 * COLLECT THE AGENTS AROUND THE CIRCLE
 * 
 */
void tankStage() {

  int maxTickCount = 6;
  
  for(int tickCount = 1; tickCount <= maxTickCount; tickCount++) {

    /* Press YELLOW RESET to switch to user input menu */
    int switch0 = digitalRead(YELLOWBUTTON);
    if (switch0 == 0) { 
      delay(1000); 
      if (switch0 == 0) { break; } 
    }

    /* GO TO NEXT TICK */
    circleFollow(menu);
    delay(500);

    /* PICK UP AGENT */
    raiseArm();
    delay(100);
    closePincer();
    delay(500);
    lowerArm();
    delay(500);
    openPincer();
    
  }
  
}

/*
 * 
 * GET BASKET ONTO THE ZIPLINE
 * 
 */
void lineStage() {
  
  driveForward(1800.0);
  delay(1000);
  raiseArm(); /* GET ARM OUT OF THE WAY */
  delay(1000);
  raiseLift(); /* RAISE THE LIFT WITH THE BASKET ON IT */
  delay(2000);
  driveForward(1800.0); /* DRIVES THE BASKET ONTO THE ZIPLINE */
  lowerLift(); /* LOWERS THE LIFT TO RELEASE THE BASKET */
  backUp(5700.0); /* GETS OUT OF THE WAY OF THE BASKET'S DESCENT */
  lowerArm();
  
}
