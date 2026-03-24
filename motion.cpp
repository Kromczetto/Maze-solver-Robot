#include <Arduino.h>
#include "motion.h"
#include "motors.h"
#include "tof_sensors.h"

#define FRONT_STOP 10
#define ENTER_TIME 120
#define TURN_TIME 260
#define ALIGN_TIME 180

RobotState currentState = IDLE;
RobotState pendingTurn = IDLE;

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
    setState(TURNING_LEFT);
}

void turnRight90() {
    setState(TURNING_RIGHT);
}

void turnAround() {
    setState(TURNING_AROUND);
}

void stabilizeForward() {

    float front = getFrontDistance();

    if (front <= FRONT_STOP) {
        stopMotors();
        setState(IDLE);
        return;
    }

    float left  = getLeftDistance();
    float right = getRightDistance();

    float error = left - right;

    if (abs(error) < 3) {
        leftMotorForward();
        rightMotorForward();
    }
    else if (error > 0) {
        leftMotorForward();
        rightMotorStop();
    }
    else {
        leftMotorStop();
        rightMotorForward();
    }
}

void updateMotion() {

    switch (currentState) {

        case IDLE:
            stopMotors();
        break;

        case MOVING_FORWARD:
            stabilizeForward();
        break;

        case ENTERING_CELL:
        {
            leftMotorForward();
            rightMotorForward();

            if (millis() - stateStartTime > ENTER_TIME) {

                if (pendingTurn == TURNING_LEFT)
                    setState(TURNING_LEFT);
                else if (pendingTurn == TURNING_RIGHT)
                    setState(TURNING_RIGHT);
                else if (pendingTurn == TURNING_AROUND)
                    setState(TURNING_AROUND);
                else
                    setState(MOVING_FORWARD);

                pendingTurn = IDLE;
            }
        }
        break;

        case TURNING_LEFT:
        {
            if (millis() - stateStartTime < TURN_TIME) {
                leftMotorForward();
                rightMotorBackward();
                return;
            }
            setState(ALIGN_AFTER_TURN);
        }
        break;

        case TURNING_RIGHT:
        {
            if (millis() - stateStartTime < TURN_TIME) {
                leftMotorBackward();
                rightMotorForward();
                return;
            }
            setState(ALIGN_AFTER_TURN);
        }
        break;

        case TURNING_AROUND:
        {
            if (millis() - stateStartTime < TURN_TIME * 2) {
                leftMotorForward();
                rightMotorBackward();
                return;
            }
            setState(ALIGN_AFTER_TURN);
        }
        break;

        case ALIGN_AFTER_TURN:
        {
            float left  = getLeftDistance();
            float right = getRightDistance();

            float error = left - right;

            if (abs(error) < 2) {
                leftMotorForward();
                rightMotorForward();

                if (millis() - stateStartTime > ALIGN_TIME) {
                    setState(MOVING_FORWARD);
                }
                return;
            }

            if (error > 0) {
                leftMotorForward();
                rightMotorBackward();
            } else {
                leftMotorBackward();
                rightMotorForward();
            }
        }
        break;
    }
}