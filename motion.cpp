#include <Arduino.h>
#include "motion.h"
#include "motors.h"
#include "tof_sensors.h"
#include "robot_config.h"
#include "encoders.h"

RobotState currentState = FORWARD;

#define HALF_CELL_TICKS 700
#define TURN_TICKS 400
#define FRONT_TURN_THRESHOLD 14

bool turnPending = false;
RobotState nextTurn = FORWARD;

void driveForward(bool stabilize = true) {

    float left  = getLeftFiltered();
    float right = getRightFiltered();

    int baseSpeed = 150;

    int leftSpeed  = baseSpeed;
    int rightSpeed = baseSpeed;

    if (stabilize && left < OPEN_THRESHOLD && right < OPEN_THRESHOLD) {

        float Kp = 5.0;
        float maxCorrection = 40;

        float error = left - right;
        float correction = Kp * error;

        if (correction > maxCorrection) correction = maxCorrection;
        if (correction < -maxCorrection) correction = -maxCorrection;

        leftSpeed  -= correction;
        rightSpeed += correction;
    }

    setMotorSpeed(leftSpeed, rightSpeed);

    leftMotorForward();
    rightMotorForward();
}

void turnLeft() {
    setMotorSpeed(120, 120);
    leftMotorBackward();
    rightMotorForward();
}

void turnRight() {
    setMotorSpeed(120, 120);
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

    float left  = getLeftFiltered();
    float right = getRightFiltered();
    float front = getFrontFiltered();

    switch (currentState) {

        case FORWARD: {

            driveForward(!turnPending);

            long avgTicks = (getLeftTicks() + getRightTicks()) / 2;

            RobotState newDecision = FORWARD;

            if (left < OPEN_THRESHOLD && right > OPEN_THRESHOLD) {
                newDecision = TURNING_RIGHT;
            }

            else if (left > OPEN_THRESHOLD) {
                newDecision = TURNING_LEFT;
            }

            if (avgTicks > 300 && newDecision != FORWARD) {

                nextTurn = newDecision;

                if (!turnPending) {
                    turnPending = true;
                    resetEncoders();
                    return;
                }
            }

            if (turnPending) {

                long ticksAfterDetect = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

                if (nextTurn == TURNING_LEFT) {

                    if (ticksAfterDetect >= HALF_CELL_TICKS - 150) {
                        stop();
                        resetEncoders();
                        currentState = TURNING_LEFT;
                        turnPending = false;
                        return;
                    }
                }

                if (nextTurn == TURNING_RIGHT) {

                    if (front < FRONT_TURN_THRESHOLD && ticksAfterDetect > 200) {
                        stop();
                        resetEncoders();
                        currentState = TURNING_RIGHT;
                        turnPending = false;
                        return;
                    }
                }

                if (ticksAfterDetect >= HALF_CELL_TICKS) {
                    stop();
                    resetEncoders();
                    currentState = nextTurn;
                    turnPending = false;
                }
            }

            break;
        }

        case TURNING_LEFT: {

            turnLeft();

            long ticks = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

            if (ticks >= TURN_TICKS) {
                stop();
                resetEncoders();
                currentState = FORWARD;
            }

            break;
        }

        case TURNING_RIGHT: {

            turnRight();

            long ticks = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

            if (ticks >= TURN_TICKS) {
                stop();
                resetEncoders();
                currentState = FORWARD;
            }

            break;
        }

        case TURNING_AROUND: {

            turnLeft();

            long ticks = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

            if (ticks >= TURN_TICKS * 2) {
                stop();
                resetEncoders();
                currentState = FORWARD;
            }

            break;
        }

        case IDLE:
            stop();
            break;
    }
}