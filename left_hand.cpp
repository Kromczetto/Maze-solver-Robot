#include "left_hand.h"
#include "motion.h"
#include "tof_sensors.h"

void leftHandStep() {

    if (!isRobotIdle()) return;

    float front = getFrontDistance();
    float left  = getLeftDistance();
    float right = getRightDistance();

    if (left > 30) {
        turnLeft90();
        return;
    }

    if (front > 15) {
        moveForward();
        return;
    }

    if (right > 30) {
        turnRight90();
        return;
    }

    turnAround();
}