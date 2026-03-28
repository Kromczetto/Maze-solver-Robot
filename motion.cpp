#include <Arduino.h>
#include "motion.h"
#include "motors.h"
#include "tof_sensors.h"
#include "mpu.h"

#define OPEN 25
#define FRONT_BLOCK 15

RobotState currentState = IDLE;

static void setState(RobotState newState) {
    currentState = newState;

    if (newState == TURNING_LEFT ||
        newState == TURNING_RIGHT ||
        newState == TURNING_AROUND) {
        resetAngle();
    }
}

RobotState getRobotState() {
    return currentState;
}

bool isRobotIdle() {
    return currentState == IDLE;
}

void updateMotion() {

    float front = getFrontDistance();
    float left  = getLeftDistance();
    float right = getRightDistance();

    bool leftOpen  = left  > OPEN;
    bool frontOpen = front > FRONT_BLOCK;
    bool rightOpen = right > OPEN;

    float angle = getAngle();

    switch (currentState) {

        case IDLE:
            stopMotors();
        break;

        case MOVING_FORWARD:

            if (leftOpen) {
                stopMotors();
                setState(TURNING_LEFT);
                break;
            }

            if (!frontOpen) {

                stopMotors();

                if (rightOpen) {
                    setState(TURNING_RIGHT);
                }
                else {
                    setState(TURNING_AROUND);
                }

                break;
            }

            leftMotorForward();
            rightMotorForward();
        break;

        case TURNING_LEFT:

            leftMotorForward();
            rightMotorBackward();

            if (angle >= 85) { 
                setState(MOVING_FORWARD);
            }
        break;

        case TURNING_RIGHT:

            leftMotorBackward();
            rightMotorForward();

            if (angle <= -85) {
                setState(MOVING_FORWARD);
            }
        break;

        case TURNING_AROUND:

            leftMotorForward();
            rightMotorBackward();

            if (abs(angle) >= 170) {
                setState(MOVING_FORWARD);
            }
        break;
    }
}