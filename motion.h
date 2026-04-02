#ifndef MOTION_H
#define MOTION_H

enum RobotState {
    IDLE,
    FORWARD,
    TURNING_LEFT,
    TURNING_RIGHT,
    PREPARE_TURN_LEFT,
    PREPARE_TURN_RIGHT,
    TURNING_AROUND
};

extern RobotState currentState;

void updateMotion();
void turnLeft();
void turnRight();
void turnAround();

RobotState getRobotState();
bool isRobotIdle();
void stabilize(float left, float right);
void driveStraightCorridor();

#endif