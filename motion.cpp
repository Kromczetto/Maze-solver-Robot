#include <Arduino.h>
#include "motion.h"
#include "motors.h"
#include "tof_sensors.h"
#include "robot_config.h"
#include "encoders.h"

RobotState currentState = FORWARD;

#define HALF_CELL_TICKS 700
#define TURN_TICKS 400

bool turnPending = false;
RobotState nextTurn = FORWARD;

void driveForward() {

    float left  = getLeftDistance();
    float right = getRightDistance();

    int baseSpeed = 150;

    int leftSpeed  = baseSpeed;
    int rightSpeed = baseSpeed;

    float Kp = 5.0;
    float maxCorrection = 40;

    float error = left - right;
    float correction = Kp * error;

    if (correction > maxCorrection) correction = maxCorrection;
    if (correction < -maxCorrection) correction = -maxCorrection;

    leftSpeed  -= correction;
    rightSpeed += correction;

    leftSpeed  = constrain(leftSpeed, 100, 200);
    rightSpeed = constrain(rightSpeed, 100, 200);

    setMotorSpeed(leftSpeed, rightSpeed);

    leftMotorForward();
    rightMotorForward();
}

void turnLeft() {
    setMotorSpeed(90, 90);
    leftMotorBackward();
    rightMotorForward();
}

void turnRight() {
    setMotorSpeed(90, 90);
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

    float left  = getLeftDistance();
    float right = getRightDistance();

    switch (currentState) {

        case FORWARD: {

            driveForward();

            long avgTicks = (getLeftTicks() + getRightTicks()) / 2;

            if (!turnPending && avgTicks > 300) {

                bool leftOpen  = left  > OPEN_THRESHOLD;
                bool rightOpen = right > OPEN_THRESHOLD;

                if (leftOpen) {
                    nextTurn = TURNING_LEFT;
                }
                else if (rightOpen) {
                    nextTurn = TURNING_RIGHT;
                }
                else {
                    break; 
                }

                turnPending = true;

                resetEncoders();  
                return;     
            }

            if (turnPending) {

                long ticksAfterDetect = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

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