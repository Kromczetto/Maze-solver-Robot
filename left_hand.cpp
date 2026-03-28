// #include "left_hand.h"
// #include "motion.h"
// #include "tof_sensors.h"

// #define OPEN 25
// #define FRONT_BLOCK 15

// void leftHandStep() {

//     RobotState state = getRobotState();

//     // nie przerywaj skrętu
//     if (state == TURNING_LEFT || state == TURNING_RIGHT || state == TURNING_AROUND)
//         return;

//     float front = getFrontDistance();
//     float left  = getLeftDistance();
//     float right = getRightDistance();

//     bool leftOpen  = left  > OPEN;
//     bool frontOpen = front > FRONT_BLOCK;
//     bool rightOpen = right > OPEN;

//     if (front < FRONT_BLOCK) {

//         if (leftOpen) {
//             turnLeft90();
//             return;
//         }

//         if (rightOpen) {
//             turnRight90();
//             return;
//         }

//         turnAround();
//         return;
//     }

//     if (leftOpen) {
//         turnLeft90();
//         return;
//     }

//     moveForward();
// }