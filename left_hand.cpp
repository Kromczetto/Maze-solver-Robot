#include "left_hand.h"
#include "robot_config.h"

RobotState decideLeftHand(float left, float front, float right, bool &turnAroundLeft) {

    bool leftOpen  = left  > OPEN_THRESHOLD;
    bool frontOpen = front > FRONT_THRESHOLD;
    bool rightOpen = right > OPEN_THRESHOLD;

    if (!leftOpen && !rightOpen && !frontOpen) {

        if (left > right + 2) {
            turnAroundLeft = true;
        }
        else if (right > left + 2) {
            turnAroundLeft = false;
        }
        else {
            turnAroundLeft = true;
        }

        return TURNING_AROUND;
    }

    if (leftOpen) {
        return TURNING_LEFT;
    }
    else if (frontOpen) {
        return FORWARD;
    }
    else if (rightOpen) {
        return TURNING_RIGHT;
    }

    return FORWARD;
}