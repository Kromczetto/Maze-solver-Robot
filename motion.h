#ifndef MOTION_H
#define MOTION_H

enum RobotState {
    IDLE,
    FORWARD,
    TURNING_LEFT,
    TURNING_RIGHT
};

extern RobotState currentState;

void updateMotion();
void turnLeft();
void turnRight();

RobotState getRobotState();
bool isRobotIdle();
void stabilize(float left, float right);
void driveStraightCorridor();

#endif