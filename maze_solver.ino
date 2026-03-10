#include <Arduino.h>

#include "motors.h"
#include "motion.h"
#include "ultrasonic.h"
#include "left_hand.h"

void setup() {
  // put your setup code here, to run once:
  initMotors();
  initSensors();
}

void loop() {
  // put your main code here, to run repeatedly:
  updateMotion();
  if (isRobotIdle()) {
    leftHandStep();
  }
}
