#include <Arduino.h>
#include "motion.h"
#include "motors.h"
#include "tof_sensors.h"

#define TURN_TIME 130
#define BACK_TIME 220

#define FRONT_WALL 12

#define CENTER_MIN 8
#define CENTER_MAX 12

static RobotState currentState = IDLE;
static unsigned long stateStartTime = 0;

bool isRobotIdle() {
    return currentState == IDLE;
}

RobotState getRobotState() {
    return currentState;
}

static void setState(RobotState newState) {
    currentState = newState;
    stateStartTime = millis();
}

void moveForward() {
    setState(MOVING_FORWARD);
}

void turnLeft90() {
    stopMotors();
    delay(50);
    setState(TURNING_LEFT);
}

void turnRight90() {
    stopMotors();
    delay(50);
    setState(TURNING_RIGHT);
}

void turnAround() {
    stopMotors();
    delay(50);
    setState(TURNING_AROUND);
}

void stabilizeForward() {
    float left = getLeftDistance();

    if (left < CENTER_MIN) {

        leftMotorStop();
        rightMotorForward();
    } 
    else if (left > CENTER_MAX) {

        leftMotorForward();
        rightMotorStop();
    } 
    else {
        leftMotorForward();
        rightMotorForward();
    }
}

void moveToCenter() {
    stabilizeForward();
}

void updateMotion() {

    float front = getFrontDistance();

    switch (currentState) {

        case IDLE:
   
            stopMotors();
        break;

        case MOVING_FORWARD:
       
            if (front < FRONT_WALL) {
                stopMotors();
                currentState = IDLE;
                break;
            }

            stabilizeForward();
        
        break;

       
        case TURNING_LEFT:

            leftMotorForward();
            rightMotorBackward();

            if (millis() - stateStartTime > TURN_TIME) {
                setState(IDLE);
            }
        
        break;

        case TURNING_RIGHT:
    
        
            leftMotorBackward();
            rightMotorForward();

            if (millis() - stateStartTime > TURN_TIME) {
                setState(IDLE);
            }
        
        break;

        case TURNING_AROUND:
        
            leftMotorBackward();
            rightMotorForward();

            if (millis() - stateStartTime > BACK_TIME) {
                setState(IDLE);
            }
        
        break;
    }
}