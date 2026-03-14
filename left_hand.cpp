#include "left_hand.h"
#include "motion.h"
#include "ultrasonic.h"
#include "robot_config.h"
#include "motion.h"

void leftHandStep() {

    float front = getFrontDistance();
    float left = getLeftDistance(); 
    float right = getRightDistance(); 

    if (left > WALL_THRESHOLD_CM + 5) { 

        moveToCenter();
        turnRight90();
        return;
    }

    if (front > WALL_THRESHOLD_CM + 5) {

        moveForward();
        return;
    }

    if (right > WALL_THRESHOLD_CM  + 5) { 

        moveToCenter();
        turnLeft90();
        return;
    }

    turnAround();
}