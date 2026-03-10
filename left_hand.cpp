#include "left_hand.h"
#include "motion.h"
#include "ultrasonic.h"

void leftHandStep() {
    float front = getFrontDistance();

    if(front > 20) {
        moveForwardCell();
    }
}