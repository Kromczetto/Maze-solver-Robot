#ifndef MOTION_H
#define MOTION_H

enum RobotState {
    IDLE,
    MOVING_FORWARD,
    MOVING_TO_CENTER,
    TURNING_LEFT,
    TURNING_RIGHT,
    TURNING_AROUND
};

bool isRobotIdle();

void moveForward();
void turnLeft90();
void turnRight90();
void turnAround();

void updateMotion();
void stabilizeForward(float left, float front);
void moveToCenter();

RobotState getRobotState();

#endif