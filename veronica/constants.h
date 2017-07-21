#pragma once // only include this file once

#include <phys253.h>
#include <LiquidCrystal.h>

/* ANALOG INPUTS */
#define LEFT_QRD 0
#define RIGHT_QRD 1
#define SIDE_QRD 2
#define EDGELEFT_QRD 3
#define EDGERIGHT_QRD 4
#define WHEEL_QRD 5
#define TOP_POT 6
#define BOT_POT 7

/* DIGITAL INPUTS */
#define ONE_IR 0
#define TEN_IR 1
#define BASKET_SWITCH 2

/* MOTOR OUTPUTS */
#define RIGHT_MOTOR 0
#define LEFT_MOTOR 1

/* SERVO OUTPUTS */
#define ARM_SERVO 0
#define CLAW_SERVO 1
#define BASKET_SERVO 2

/* MENU ITEMS */
int velocity = 100;
int kp = 15;
int ki = 0;
int kd = 35;
int thresh = 775;
int side_thresh = 930;
bool leftCourse = true;
