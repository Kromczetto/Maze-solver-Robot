#include <Arduino.h>
#include "motion.h"
#include "motors.h"
#include "robot_config.h"

enum RobotState {
    IDLE,
    MOVING_FORWARD,
    TURNING_LEFT,
    TURNING_RIGHT,
    TURNING_AROUND
};

static RobotState currentState = IDLE;

static unsigned long stateStartTime = 0;
static unsigned long stateDuration = 0;

bool isRobotIdle() {
    return currentState == IDLE;
}

void moveForwardCell() {
    leftMotorForward();
    rightMotorForward();

    currentState = MOVING_FORWARD;
    stateStartTime = millis();
    stateDuration = MOVE_ONE_CELL_TIME_MS;
}

void turnLeft90() {
    leftMotorStop();
    rightMotorForward();

    currentState = TURNING_LEFT;
    stateStartTime = millis();
    stateDuration = TURN_90_TIME_MS;
}

void turnRight90() {
    rightMotorStop();
    leftMotorForward();

    currentState = TURNING_RIGHT;
    stateStartTime = millis();
    stateDuration = TURN_90_TIME_MS;
}

void turnAround() {
    leftMotorBackward();
    rightMotorForward();

    currentState = TURNING_AROUND;
    stateStartTime = millis();
    stateDuration = TURN_180_TIME_MS;
}

void updateMotion() {
    if (currentState == IDLE) {
        return;
    }

    if (millis() - stateStartTime >= stateDuration) {
        stopMotors();
        currentState = IDLE;
    }
}