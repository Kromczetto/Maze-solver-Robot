#include "turns.h"
#include "motors.h"

void turnLeft() {
    setMotorSpeed(110, 110);
    leftMotorBackward();
    rightMotorForward();
}

void turnRight() {
    setMotorSpeed(110, 110);
    leftMotorForward();
    rightMotorBackward();
}

void stop() {
    stopMotors();
}