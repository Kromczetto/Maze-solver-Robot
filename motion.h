#ifndef MOTION_H
#define MOTION_H

enum RobotState {
    FORWARD,
    DECIDE,
    TURNING_LEFT,
    TURNING_RIGHT,
    TURNING_AROUND,
    IDLE
};

RobotState getRobotState();
void updateMotion();

void driveForward();

#endif