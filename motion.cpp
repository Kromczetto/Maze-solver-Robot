#include "motion.h"
#include "drive.h"
#include "turns.h"
#include "navigator.h"
#include "tof_sensors.h"
#include <Arduino.h>
#include "robot_config.h"
#include "encoders.h"
#include "motors.h"
#include "maze.h"

RobotState currentState = IDLE;

bool turnAroundLeftDirection = true;

void startDecision() {
    currentState = DECIDE;
}

void updateMotion() {

    float left = getLeftFiltered();
    float right = getRightFiltered();
    float front = getFrontFiltered();

    long avgTicks = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

    switch (currentState) {
        case DECIDE: {

            updateWalls(left, front, right);

            floodFillStart();
            while (!isFloodFillDone()) {
                floodFillStep();
            }

            RobotState decision = getNavigationDecision(left, front, right);

            if (decision == TURNING_LEFT) {
                resetEncoders();
                currentState = TURNING_LEFT;
            }
            else if (decision == TURNING_RIGHT) {
                resetEncoders();
                currentState = TURNING_RIGHT;
            }
            else if (decision == TURNING_AROUND) {
                turnAroundLeftDirection = (left > right);
                resetEncoders();
                currentState = TURNING_AROUND;
            }
            else {
                resetEncoders();
                currentState = FORWARD;
            }

            return;
        }

        case FORWARD: {

            driveForward(true);

            if (avgTicks >= TICKS_PER_CELL || front < 6) {

                stopMotors();

                updatePosition();

                if (isAtGoal()) {
                    currentState = IDLE;
                    return;
                }

                resetEncoders();
                currentState = DECIDE;  
                return;
            }

            break;
        }

        case TURNING_LEFT: {

            turnLeft();

            long ticks = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

            if (ticks >= TURN_TICKS_LEFT) {
                stopMotors();
                updateDirection(-1);
                resetEncoders();
                currentState = POST_TURN_FORWARD;
            }

            break;
        }

        case TURNING_RIGHT: {

            turnRight();

            long ticks = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

            if (ticks >= TURN_TICKS_RIGHT) {
                stopMotors();
                updateDirection(1);
                resetEncoders();
                currentState = POST_TURN_FORWARD;
            }

            break;
        }

        case TURNING_AROUND: {

            if (turnAroundLeftDirection) turnLeft();
            else turnRight();

            long ticks = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

            if (ticks >= TURN_TICKS_LEFT * 2) {
                stopMotors();
                updateDirection(2);
                resetEncoders();
                currentState = FORWARD;
            }

            break;
        }

        case POST_TURN_FORWARD: {

            driveForward(true);

            long ticks = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

            if (ticks >= POST_TURN_FORWARD_TICKS) {
                stopMotors();
                resetEncoders();
                currentState = FORWARD;
            }

            break;
        }

        case IDLE:
            stopMotors();
            break;
    }
}

RobotState getRobotState() {
    return currentState;
}