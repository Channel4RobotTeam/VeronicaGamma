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

/* STAGES */
#define GATE_STAGE 0
#define RAMP_STAGE 1
#define TANK_STAGE 2
#define LINE_STAGE 3

/* DEFAULTS MENU VALUES */
#define VELOCITY 100
#define KP 15
#define KI 0
#define KD 35
#define KP_CIRCLE 15
#define KI_CIRCLE 0
#define KD_CIRCLE 35
#define THRESH 775
#define THRESH_SIDE 930

/* DISTANCE TRACKING VALUES */
#define THRESH_WHEEL 900 
#define WHEEL_RADIUS 4.4 
