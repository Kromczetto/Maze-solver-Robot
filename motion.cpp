#include <Arduino.h>
#include "motion.h"
#include "motors.h"
#include "tof_sensors.h"
#include "robot_config.h"
#include "encoders.h"

RobotState currentState = FORWARD;

// 🔥 KALIBRACJA
#define TICKS_PER_CELL 2400   // MUSISZ dostroić!
#define TURN_TICKS 800        // ~90 stopni

void driveForward() {

    int baseLeft  = 140;
    int baseRight = 150;

    setMotorSpeed(baseLeft, baseRight);

    leftMotorForward();
    rightMotorForward();
}

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

RobotState getRobotState() {
    return currentState;
}

void updateMotion() {

    float front = getFrontDistance();
    float left  = getLeftDistance();
    float right = getRightDistance();

    long avgTicks = (getLeftTicks() + getRightTicks()) / 2;

    switch (currentState) {

        // ===== JEDŹ 25 CM =====
        case FORWARD:

            driveForward();

            if (avgTicks >= TICKS_PER_CELL) {
                stop();
                resetEncoders();
                currentState = DECIDE;
            }

            break;

        // ===== DECYZJA NA ŚRODKU =====
        case DECIDE:

            if (left > OPEN_THRESHOLD) {
                resetEncoders();
                currentState = TURNING_LEFT;
            }
            else if (front > OPEN_THRESHOLD) {
                currentState = FORWARD;
            }
            else if (right > OPEN_THRESHOLD) {
                resetEncoders();
                currentState = TURNING_RIGHT;
            }
            else {
                resetEncoders();
                currentState = TURNING_AROUND;
            }

            break;

        // ===== SKRĘTY =====
        case TURNING_LEFT:

            turnLeft();

            if (getLeftTicks() >= TURN_TICKS) {
                stop();
                resetEncoders();
                currentState = FORWARD;
            }

            break;

        case TURNING_RIGHT:

            turnRight();

            if (getRightTicks() >= TURN_TICKS) {
                stop();
                resetEncoders();
                currentState = FORWARD;
            }

            break;

        case TURNING_AROUND:

            turnLeft();

            if (getLeftTicks() >= TURN_TICKS * 2) {
                stop();
                resetEncoders();
                currentState = FORWARD;
            }

            break;

        case IDLE:
            stop();
            break;
    }
}