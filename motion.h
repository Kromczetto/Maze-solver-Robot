#ifndef MOTION_H
#define MOTION_H

enum RobotState {
    IDLE,
    MOVING_FORWARD,
    TURNING_LEFT,
    TURNING_RIGHT,
    TURNING_AROUND,
    ENTERING_CELL,
    ALIGN_AFTER_TURN
};

extern RobotState currentState;
extern RobotState pendingTurn;

bool isRobotIdle();
RobotState getRobotState();

void moveForward();
void turnLeft90();
void turnRight90();
void turnAround();

void updateMotion();
void stabilizeForward();

#endif