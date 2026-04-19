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

RobotState currentState = FORWARD;

bool turnAroundLeftDirection = true;

void updateMotion() {

    float left = getLeftFiltered();
    float right = getRightFiltered();
    float front = getFrontFiltered();

    long avgTicks = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

    switch (currentState) {

        case FORWARD: {

            driveForward(true);

            if (avgTicks >= TICKS_PER_CELL) {

                stopMotors();

                updatePosition();
                updateWalls(left, front, right);

                RobotState decision = getNavigationDecision(left, front, right);
                
                //FOR DEBUG PURPOSE!
                stopMotors();
                delay(500);

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
                }

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
                currentState = FORWARD;
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
                currentState = FORWARD;
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

        case IDLE:
            stopMotors();
            break;
    }
}

RobotState getRobotState() {
    return currentState;
}