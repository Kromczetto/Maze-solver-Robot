#include <Arduino.h>
#include "motion.h"
#include "motors.h"
#include "ultrasonic.h"
#include "robot_config.h"

static RobotState currentState = IDLE;

enum PendingTurn {
    NONE,
    LEFT,
    RIGHT
};

static PendingTurn pendingTurn = NONE;

static unsigned long turnAroundStart = 0;
static unsigned long turnStart = 0;
static unsigned long centerStart = 0;
static unsigned long forwardStart = 0;

bool isRobotIdle() {
    return currentState == IDLE;
}

RobotState getRobotState() {
    return currentState;
}

void moveForward() {

    leftMotorForward();
    rightMotorForward();

    forwardStart = millis();
    currentState = MOVING_FORWARD;
}

void moveToCenter() {

    leftMotorForward();
    rightMotorForward();

    centerStart = millis();
    currentState = MOVING_TO_CENTER;
}

void turnLeft90() {

    pendingTurn = LEFT;
    moveToCenter();
}

void turnRight90() {

    pendingTurn = RIGHT;
    moveToCenter();
}

void turnAround() {

    leftMotorBackward();
    rightMotorForward();

    turnAroundStart = millis();
    currentState = TURNING_AROUND;
}

void stabilizeForward(float left, float front) {

    if (currentState != MOVING_FORWARD) return;

    if (millis() - forwardStart > 250) {
        
        if (front < 7) {
            stopMotors();
            currentState = IDLE;
            return;
        }

    }

    const float wallDetect = 12;
    const float target = 6.0;
    const float tolerance = 1.5;

    if (left > wallDetect) {
        return;
    }

    if (left < target - tolerance) {
        leftMotorStop();
        rightMotorForward();
    }

    else if (left > target + tolerance) {
        leftMotorForward();
        rightMotorStop();
    }

    else {
        leftMotorForward();
        rightMotorForward();
    }
}

void updateMotion() {

    if (currentState == IDLE) return;

    float front = getFrontDistance();
    float left = getLeftDistance();
    float right = getRightDistance();

    if (currentState == MOVING_FORWARD && front < 7 && left < 10 && right < 10) {
        turnAround();
        return;
    }

    switch (currentState) {

        case MOVING_FORWARD:

            if (getLeftDistance() > WALL_THRESHOLD_CM + 15) {
                stopMotors();
                currentState = IDLE;
                break;
            }

            if (getRightDistance() > WALL_THRESHOLD_CM + 15) {
                stopMotors();
                currentState = IDLE;
                break;
            }

            stabilizeForward(left, front);
            break;

        case MOVING_TO_CENTER:

            if (millis() - centerStart > 300) {

                if (pendingTurn == LEFT) {
                    rightMotorStop();
                    leftMotorForward();

                    turnStart = millis();
                    currentState = TURNING_LEFT;
                }
                else if (pendingTurn == RIGHT) {
                    rightMotorForward();
                    leftMotorStop();

                    turnStart = millis();
                    currentState = TURNING_RIGHT;
                }
                else {
                    stopMotors();
                    currentState = IDLE;
                }

                pendingTurn = NONE;
            }

        break;

        case TURNING_LEFT:

            if (millis() - turnStart > 1000) {
                stopMotors();
                currentState = IDLE;
            }

        break;

        case TURNING_RIGHT:

            if (millis() - turnStart > 1000) {
                stopMotors();
                currentState = IDLE;
            }

        break;

        case TURNING_AROUND:

            if (millis() - turnAroundStart > 2000) {
                stopMotors();
                currentState = IDLE;
            }

        break;

        default:
        break;
    }
}