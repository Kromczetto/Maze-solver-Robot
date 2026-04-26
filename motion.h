#ifndef MOTION_H
#define MOTION_H

#include <SoftwareSerial.h>

enum RobotState {
    IDLE,
    DECIDE,
    WAIT_FOR_FLOOD,
    DEBUG_PAUSE,
    FORWARD,
    TURNING_LEFT,
    TURNING_RIGHT,
    TURNING_AROUND,
    POST_TURN_FORWARD
};

extern SoftwareSerial SUART;

enum NavigationAlgorithm {
    LEFT_HAND,
    FLOOD_FILL,
    TREMAUX,
    GREEDY
};

extern NavigationAlgorithm currentAlgorithm;

RobotState getRobotState();
void updateMotion();
void startDecision(); 

#endif