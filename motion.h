#ifndef MOTION_H
#define MOTION_H

enum RobotState {
    IDLE,
    DECIDE,
    FORWARD,
    TURNING_LEFT,
    TURNING_RIGHT,
    TURNING_AROUND,
    POST_TURN_FORWARD
};

enum NavigationAlgorithm {
    LEFT_HAND,
    FLOOD_FILL
};

extern NavigationAlgorithm currentAlgorithm;

RobotState getRobotState();
void updateMotion();
void startDecision(); 

#endif