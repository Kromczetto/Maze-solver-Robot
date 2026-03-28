#ifndef MOTION_H
#define MOTION_H

enum RobotState {
    IDLE,
    MOVING_FORWARD,
    TURNING_RIGHT,
    TURNING_LEFT,
    TURNING_AROUND
};

extern RobotState currentState;

bool isRobotIdle();
RobotState getRobotState();

void moveForward();
void turnLeft90();
void turnRight90();
void turnAround();

void updateMotion();
void stabilizeForward();

#endif