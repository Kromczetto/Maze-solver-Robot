#include <Arduino.h>

#include "motors.h"
#include "motion.h"
#include "ultrasonic.h"
#include "left_hand.h"

void setup() {
  initMotors();
  initSensors();
}

void loop() {
    updateMotion();

    if (isRobotIdle()) {
        leftHandStep();
    }
}
