#include "motion.h"
#include "drive.h"
#include "turns.h"
#include "navigator.h"
#include "tof_sensors.h"
#include <Arduino.h>
#include "robot_config.h"
#include "encoders.h"
#include "motors.h"

RobotState currentState = FORWARD;

bool turnPending = false;
RobotState nextTurn = FORWARD;

void updateMotion() {

    float left  = getLeftFiltered();
    float right = getRightFiltered();
    float front = getFrontFiltered();

    switch (currentState) {

        case FORWARD: {

            driveForward(true);

            long avgTicks = (getLeftTicks() + getRightTicks()) / 2;

            RobotState newDecision = getNavigationDecision(left, front, right);

            if (newDecision == TURNING_AROUND) {
                resetEncoders();
                currentState = TURNING_AROUND;
                return;
            }

            if (avgTicks > DETECT_DELAY && newDecision != FORWARD) {

                nextTurn = newDecision;

                if (!turnPending) {
                    turnPending = true;
                    resetEncoders();
                    return;
                }
            }

            if (turnPending) {

                long ticksAfterDetect = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

                if (front < 7) {
                    stop();
                    resetEncoders();
                    currentState = nextTurn;
                    turnPending = false;
                    return;
                }

                if (nextTurn != TURNING_AROUND && ticksAfterDetect >= HALF_CELL_TICKS + EXTRA_FORWARD_TICKS) {
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

            turnLeft();

            long ticks = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

            if (ticks >= TURN_TICKS_LEFT * 2) {
                stop();
                resetEncoders();
                currentState = ALIGN_AFTER_TURN;
            }

            break;
        }

        case ALIGN_AFTER_TURN: {

            float diff = left - right;

            int base = 100;

            if (abs(diff) < 1.5) {
                stop();
                resetEncoders();
                currentState = FORWARD;
                break;
            }

            if (diff > 0) {
                setMotorSpeed(base, base);
                leftMotorBackward();
                rightMotorForward();
            } else {
                setMotorSpeed(base, base);
                leftMotorForward();
                rightMotorBackward();
            }

            break;
        }

        case IDLE:
            stop();
            break;
    }
}

RobotState getRobotState() {
    return currentState;
}