#ifndef MOTION_H
#define MOTION_H

enum RobotState {
    FORWARD,
    TURNING_LEFT,
    TURNING_RIGHT,
    TURNING_AROUND,
    ALIGN_AFTER_TURN,
    IDLE
};

enum NavigationAlgorithm {
    LEFT_HAND,
    FLOOD_FILL
};

extern NavigationAlgorithm currentAlgorithm;

RobotState getRobotState();
void updateMotion();

#endif