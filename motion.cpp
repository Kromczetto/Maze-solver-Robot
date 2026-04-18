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

bool turnPending = false;
bool turnAroundLeftDirection = true;
RobotState nextTurn = FORWARD;

static long lastCellTicks = 0;
static int decisionCooldown = 0;

static int skipCellUpdate = 0;

#define TURN_3_4_THRESHOLD (TURN_TICKS_LEFT * 1.5)   
#define ALIGN_TOLERANCE 2.0                          
#define CELL_TICKS (HALF_CELL_TICKS * 2)

void updateMotion() {

    float left = getLeftFiltered();
    float right = getRightFiltered();
    float front = getFrontFiltered();

    long avgTicks = (getLeftTicks() + getRightTicks()) / 2;

    switch (currentState) {

        case FORWARD: {

            driveForward(true);

            RobotState newDecision = FORWARD;

            if (!turnPending && decisionCooldown == 0) {
                newDecision = getNavigationDecision(left, front, right);
            }

            if (avgTicks - lastCellTicks >= CELL_TICKS) {
                
                if (skipCellUpdate > 0) {
                    skipCellUpdate--;
                } else {
                    updatePosition();
                    updateWalls(left, front, right);
                }
                lastCellTicks = avgTicks;
            }

            if (newDecision == TURNING_AROUND && !turnPending) {
                turnAroundLeftDirection = (left > right);

                resetEncoders();
                lastCellTicks = 0;

                currentState = TURNING_AROUND;
                decisionCooldown = 10;
                return;
            }

            if (avgTicks > DETECT_DELAY && newDecision != FORWARD && !turnPending) {

                //  if (avgTicks - lastCellTicks < CELL_TICKS * 0.8) {
                //     updatePosition();
                //     updateWalls(left, front, right);
                // }
                
                nextTurn = newDecision;
                turnPending = true;

                resetEncoders();
                lastCellTicks = 0;
                decisionCooldown = 10;
                return;
            }

            if (turnPending) {

                long ticksAfterDetect = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

                if (front < 7) {
                    stop();
                    resetEncoders();
                    lastCellTicks = 0;

                    currentState = nextTurn;
                    turnPending = false;
                    return;
                }

                if (nextTurn != TURNING_AROUND &&
                    ticksAfterDetect >= CELL_TICKS * 0.5) {

                    stop();
                    resetEncoders();
                    lastCellTicks = 0;

                    currentState = nextTurn;
                    turnPending = false;
                    
                    skipCellUpdate = 1;
                    return;
                }
            }

            if (decisionCooldown > 0) decisionCooldown--;

            break;
        }

        case TURNING_LEFT: {

            turnLeft();

            long ticks = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

            if (ticks >= TURN_TICKS_LEFT) {
                stop();
                resetEncoders();
                updateDirection(-1);
                lastCellTicks = 0; 
                skipCellUpdate = 1;
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
                updateDirection(1);
                lastCellTicks = 0; 
                skipCellUpdate = 1;
                currentState = FORWARD;
            }

            break;
        }

        case TURNING_AROUND: {

            if (turnAroundLeftDirection) {
                turnLeft();
            } else {
                turnRight();
            }

            long ticks = (abs(getLeftTicks()) + abs(getRightTicks())) / 2;

            float diff = left - right;

            if (ticks > TURN_3_4_THRESHOLD && abs(diff) < ALIGN_TOLERANCE) {
                stop();
                resetEncoders();
                updateDirection(2);
                
                lastCellTicks = 0;
                skipCellUpdate = 1;
                currentState = FORWARD;
                return;
            }

            if (ticks >= TURN_TICKS_LEFT * 2) {
                stop();
                resetEncoders();
                updateDirection(2);

                lastCellTicks = 0;
                skipCellUpdate = 1;
                currentState = FORWARD;
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