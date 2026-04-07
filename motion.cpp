#include <Arduino.h>
#include "motion.h"
#include "motors.h"
#include "tof_sensors.h"
#include "robot_config.h"
#include "encoders.h"

RobotState currentState = FORWARD;

#define HALF_CELL_TICKS 500

#define TURN_TICKS_LEFT 400
#define TURN_TICKS_RIGHT 400

#define DETECT_DELAY 450
#define EXTRA_FORWARD_TICKS 200

#define FRONT_THRESHOLD 15   

bool turnPending = false;
RobotState nextTurn = FORWARD;

bool turnAroundLeft = true;

void driveForward(bool stabilize = true) {

    float left  = getLeftFiltered();
    float right = getRightFiltered();

    int baseLeft  = 100;
    int baseRight = 130;

    int leftSpeed  = baseLeft;
    int rightSpeed = baseRight;

    static float lastError = 0;

    if (stabilize) {

        float Kp = 2.2;
        float Kd = 0.5;

        float error = 0;

        bool leftWall  = left  < OPEN_THRESHOLD;
        bool rightWall = right < OPEN_THRESHOLD;

        if (leftWall && rightWall) {

            float targetOffset = 2.0;
            error = (left - right) - targetOffset;
        }

        else if (leftWall) {

            float targetLeft = 10.0;
            error = (left - targetLeft);
        }

        else if (rightWall) {

            float targetRight = 10.0;
            error = -(right - targetRight);
        }

        else {

            int straightCorrection = 15;

            leftSpeed  = baseLeft  + straightCorrection;
            rightSpeed = baseRight - straightCorrection;

            lastError = 0;

            setMotorSpeed(leftSpeed, rightSpeed);
            leftMotorForward();
            rightMotorForward();
            return;
        }

        float derivative = error - lastError;
        float correction = Kp * error + Kd * derivative;

        float maxCorrection = 50;
        correction = constrain(correction, -maxCorrection, maxCorrection);

        leftSpeed  = baseLeft  - correction;
        rightSpeed = baseRight + correction;

        lastError = error;
    }

    setMotorSpeed(leftSpeed, rightSpeed);
    leftMotorForward();
    rightMotorForward();
}

void turnLeft() {
    setMotorSpeed(100, 100);
    leftMotorBackward();
    rightMotorForward();
}

void turnRight() {
    setMotorSpeed(100, 100);
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

            driveForward(true);

            long avgTicks = (getLeftTicks() + getRightTicks()) / 2;

            RobotState newDecision = FORWARD;

            if (left < OPEN_THRESHOLD && right < OPEN_THRESHOLD && front < FRONT_THRESHOLD) {

                turnAroundLeft = (left > right);
                newDecision = TURNING_AROUND;
            }

            else if (left < OPEN_THRESHOLD && right > OPEN_THRESHOLD) {
                newDecision = TURNING_RIGHT;
            }

            else if (left > OPEN_THRESHOLD) {
                newDecision = TURNING_LEFT;
            }

            if ((newDecision == TURNING_AROUND) ||
                (avgTicks > DETECT_DELAY && newDecision != FORWARD)) {

                nextTurn = newDecision;

                if (!turnPending) {
                    turnPending = true;
                    resetEncoders();
                    return;
                }
            }

            if (turnPending) {

                long ticksAfterDetect = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

                if (
                    (nextTurn == TURNING_AROUND && ticksAfterDetect >= 100) || 
                    (nextTurn != TURNING_AROUND && ticksAfterDetect >= HALF_CELL_TICKS + EXTRA_FORWARD_TICKS)
                ) {
                    stop();
                    resetEncoders();
                    currentState = nextTurn;
                    turnPending = false;
                    return;
                }
            }

            break;
        }

        case TURNING_LEFT: {

            turnLeft();

            long ticks = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

            if (ticks >= TURN_TICKS_LEFT) {
                stop();
                resetEncoders();
                currentState = FORWARD;
            }

            break;
        }

        case TURNING_RIGHT: {

            turnRight();

            long ticks = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

            if (ticks >= TURN_TICKS_RIGHT) {
                stop();
                resetEncoders();
                currentState = FORWARD;
            }

            break;
        }

        case TURNING_AROUND: {

            if (turnAroundLeft) {
                turnLeft();
            } else {
                turnRight();
            }

            long ticks = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

            if (ticks >= TURN_TICKS_LEFT * 2) {
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