#ifndef MOTION_H
#define MOTION_H

enum RobotState {
    IDLE,
    MOVING_FORWARD,
    TURNING_LEFT,
    TURNING_RIGHT,
    TURNING_AROUND
};

bool isRobotIdle();
RobotState getRobotState();

void moveForward();
void turnLeft90();
void turnRight90();
void turnAround();

void updateMotion();
void moveToCenter();
void stabilizeForward();

#endif