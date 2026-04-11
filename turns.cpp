#include "turns.h"
#include "motors.h"

void turnLeft() {
    setMotorSpeed(100, 100);
    leftMotorBackward();
    rightMotorForward();
}

void turnRight() {
    setMotorSpeed(100, 100);
    leftMotorForward();
    rightMotorBackward();
}

void stop() {
    stopMotors();
}