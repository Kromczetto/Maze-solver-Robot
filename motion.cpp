#include <Arduino.h>
#include "motion.h"
#include "motors.h"
#include "tof_sensors.h"
#include "robot_config.h"
#include "encoders.h"

RobotState currentState = FORWARD;

#define TICKS_PER_CELL 2400
#define TURN_TICKS 800

void driveForward() {
    setMotorSpeed(150, 150);
    leftMotorForward();
    rightMotorForward();
}

void turnLeft() {
    setMotorSpeed(130, 130);
    leftMotorBackward();
    rightMotorForward();
}

void turnRight() {
    setMotorSpeed(130, 130);
    leftMotorForward();
    rightMotorBackward();
}

void stop() {
    stopMotors();
}

RobotState getRobotState() {
    return currentState;
}

void updateMotion() {

    float front = getFrontDistance();
    float left  = getLeftDistance();
    float right = getRightDistance();

    long avgTicks = (getLeftTicks() + getRightTicks()) / 2;

    switch (currentState) {

        case FORWARD:

            driveForward();

            if (avgTicks >= TICKS_PER_CELL) {
                stop();
                resetEncoders();
                currentState = DECIDE;
            }

            break;

        case DECIDE:

            if (left > OPEN_THRESHOLD) {
                resetEncoders();
                currentState = TURNING_LEFT;
            }
            else if (front > OPEN_THRESHOLD) {
                resetEncoders();
                currentState = FORWARD;
            }
            else if (right > OPEN_THRESHOLD) {
                resetEncoders();
                currentState = TURNING_RIGHT;
            }
            else {
                resetEncoders();
                currentState = TURNING_AROUND;
            }

            break;

        case TURNING_LEFT:

            turnLeft();

            if (getLeftTicks() >= TURN_TICKS) {
                stop();
                resetEncoders();
                currentState = FORWARD;
            }

            break;

        case TURNING_RIGHT:

            turnRight();

            if (getRightTicks() >= TURN_TICKS) {
                stop();
                resetEncoders();
                currentState = FORWARD;
            }

            break;

        case TURNING_AROUND:

            turnLeft();

            if (getLeftTicks() >= TURN_TICKS * 2) {
                stop();
                resetEncoders();
                currentState = FORWARD;
            }

            break;

        case IDLE:
            stop();
            break;
    }
}