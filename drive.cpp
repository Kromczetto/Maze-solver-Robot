#include <Arduino.h>
#include "drive.h"
#include "motors.h"
#include "tof_sensors.h"
#include "encoders.h"
#include "robot_config.h"

void driveForward(bool stabilize) {

    float left  = getLeftFiltered();
    float right = getRightFiltered();

    int baseLeft  = 100;
    int baseRight = 130;

    int leftSpeed  = baseLeft;
    int rightSpeed = baseRight;

    static float lastError = 0;

    if (stabilize) {

        float Kp = 2.2;
        float Kd = 0.5;

        float error = 0;

        bool leftWall  = left  < OPEN_THRESHOLD;
        bool rightWall = right < OPEN_THRESHOLD;

        if (leftWall && rightWall) {
            float targetOffset = 2.0;
            error = (left - right) - targetOffset;
        }
        else if (leftWall) {
            float targetLeft = 10.0;
            error = (left - targetLeft);
        }
        else if (rightWall) {
            float targetRight = 10.0;
            error = -(right - targetRight);
        }
        else {

            static long lastLeftTicks = 0;
            static long lastRightTicks = 0;

            long leftTicks  = getLeftTicks();
            long rightTicks = getRightTicks();

            long dLeft  = leftTicks  - lastLeftTicks;
            long dRight = rightTicks - lastRightTicks;

            lastLeftTicks  = leftTicks;
            lastRightTicks = rightTicks;

            float errorEnc = (float)(dLeft - dRight);

            float KpEnc = 1.5;
            float correction = KpEnc * errorEnc;

            correction = constrain(correction, -30, 30);

            leftSpeed  = baseLeft  - correction;
            rightSpeed = baseRight + correction;

            setMotorSpeed(leftSpeed, rightSpeed);
            leftMotorForward();
            rightMotorForward();
            return;
        }

        float derivative = error - lastError;
        float correction = Kp * error + Kd * derivative;

        correction = constrain(correction, -50, 50);

        leftSpeed  = baseLeft  - correction;
        rightSpeed = baseRight + correction;

        lastError = error;
    }

    setMotorSpeed(leftSpeed, rightSpeed);
    leftMotorForward();
    rightMotorForward();
}