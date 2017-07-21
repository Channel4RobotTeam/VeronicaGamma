
/**********************************/
/*                                */ 
/*          Arm Commands          */
/*                                */
/**********************************/

//RCServo0 --> Raise + Lower
//RCservo1 --> Pincer Open + Close

/*Arm Raise*/
void raiseChute(){
  for(int pos0 = 0 ; pos0<=170 ; pos0 += 1){
    RCServo0.write(pos0);
    delay(12);
  } 
}

/*Close Pincer*/
void closePincer(){
  for(int pos1 = 0 ; pos1<=(90) ; pos1+=1){
    RCServo1.write(pos1);
  }
}

/*Lower Arm*/
void lowerArm(){
  for(int pos0 = 170; pos0 >=0 ; pos0-=1){
    RCServo0.write(pos0);
    delay(12);
  }
}

/*Open Pincer*/
void openPincer(){
  for(int pos1 = 90; pos1>=25; pos1-=1){
    RCServo1.write(pos1);
  }
  delay(500);
  for(int pos1 = 25; pos1>=0 ; pos1 -= 1){
          RCServo1.write(pos1);
          delay(10);
  }
}

/*Shake arm in case agent gets stuck*/

void shake(){
  RCServo0.write(30);
  delay(80);
  RCServo0.write(0);
  delay(80);
  RCServo0.write(30);
  delay(80);
  RCServo0.write(0);
}


