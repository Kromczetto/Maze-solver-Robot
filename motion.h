#ifndef MOTION_H
#define MOTION_H

enum RobotState {
    FORWARD,
    DECIDE,
    TURNING_LEFT,
    TURNING_RIGHT,
    TURNING_AROUND,
    ALIGN_AFTER_TURN,
    IDLE
};

RobotState getRobotState();
void updateMotion();

void driveForward();

#endif