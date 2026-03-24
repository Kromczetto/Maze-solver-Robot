#include "left_hand.h"
#include "motion.h"
#include "tof_sensors.h"

#define OPEN 30

void leftHandStep() {

    if (!isRobotIdle()) return;

    float front = getFrontDistance();
    float left  = getLeftDistance();
    float right = getRightDistance();

    bool leftOpen  = left  > OPEN;
    bool frontOpen = front > OPEN;
    bool rightOpen = right > OPEN;

    if (!leftOpen && !frontOpen && !rightOpen) {
        pendingTurn = TURNING_AROUND;
        currentState = ENTERING_CELL;
        return;
    }

    if (leftOpen) {
        pendingTurn = TURNING_LEFT;
        currentState = ENTERING_CELL;
        return;
    }

    if (frontOpen) {
        moveForward();
        return;
    }

    if (rightOpen) {
        pendingTurn = TURNING_RIGHT;
        currentState = ENTERING_CELL;
        return;
    }
}