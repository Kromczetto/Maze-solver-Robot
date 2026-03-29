#ifndef MOTION_H
#define MOTION_H

enum RobotState {
    IDLE,
    MOVING_FORWARD,
    TURNING_LEFT,
    TURNING_RIGHT,
    TURNING_AROUND
};

extern RobotState currentState;

void updateMotion();

RobotState getRobotState();
bool isRobotIdle();
void stabilize(float left, float right);
void driveStraightCorridor();

#endif