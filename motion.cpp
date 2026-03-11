#include <Arduino.h>
#include "motion.h"
#include "motors.h"
#include "ultrasonic.h"
#include "robot_config.h"

enum RobotState {
    IDLE,
    MOVING_FORWARD,
    TURNING_LEFT,
    TURNING_RIGHT,
    TURNING_AROUND
};

static RobotState currentState = IDLE;

bool isRobotIdle() {
    return currentState == IDLE;
}

void moveForward() {
    leftMotorForward();
    rightMotorForward();

    currentState = MOVING_FORWARD;
} 

void turnLeft90() {
    leftMotorStop();
    rightMotorForward();

    currentState = TURNING_LEFT;
}

void turnRight90() {
    leftMotorForward();
    rightMotorStop();

    currentState = TURNING_RIGHT;
}

void turnAround() {
    leftMotorBackward();
    rightMotorForward();

    currentState = TURNING_AROUND;
}

void stabilizeForward() {
    float left = getLeftDistance();
    float right = getRightDistance();
    float front = getFrontDistance();

    const float wallDetect = 12;
    const float target = 6.0;
    const float tolerance = 2;

    if (left > wallDetect && right > wallDetect) {
        leftMotorForward();
        rightMotorForward();
        return;
    }

    if (((left < target - tolerance) && (front > 10)) || ((right < target - tolerance) && (front > 10))) {
        leftMotorStop();
        rightMotorForward();
    }

    else if (((left > target + tolerance) && (front > 10)) || ((right > target + tolerance) && (front > 10))) {
        leftMotorForward();
        rightMotorStop();
    }
    
    else {
        leftMotorForward();
        rightMotorForward();
    }
}

void moveForwardShort() {

    unsigned long start = millis();

    leftMotorForward();
    rightMotorForward();

    while (millis() - start < 200) {
        delay(1);
    }

    stopMotors();
}

void updateMotion() {
    if (currentState == IDLE) return;

    float front = getFrontDistance();
    float left = getLeftDistance();
    float right = getRightDistance();

    switch (currentState) {
        case MOVING_FORWARD:

            if (left > WALL_THRESHOLD_CM + 15) {
                stopMotors();
                currentState = IDLE;
                break;
            }

            if (right > WALL_THRESHOLD_CM + 15) {
                stopMotors();
                currentState = IDLE;
                break;
            }

            if (front < WALL_THRESHOLD_CM) {
                stopMotors();
                currentState = IDLE;
            }

            stabilizeForward();

        break;

        case TURNING_LEFT:
            
            if (right < 8 && front > WALL_THRESHOLD_CM + 25) {
                stopMotors();
                currentState = IDLE;
            }

        break;

        case TURNING_RIGHT:
            
            if (left < 8 && front > WALL_THRESHOLD_CM + 25) {
                stopMotors();
                currentState = IDLE;
            }
            
        break;
        
        case TURNING_AROUND: 

            if (front > WALL_THRESHOLD_CM) {
                stopMotors();
                currentState = IDLE;
            }
        
        break;

        default: 
        break;
    }
}
