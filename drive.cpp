#include <Arduino.h>
#include "drive.h"
#include "motors.h"
#include "tof_sensors.h"
#include "encoders.h"
#include "robot_config.h"

void driveForward(bool stabilize) {

    float left  = getLeftFiltered();
    float right = getRightFiltered();

    int baseLeft  = 95;
    int baseRight = 100;

    int leftSpeed  = baseLeft;
    int rightSpeed = baseRight;

    static float lastError = 0;

    if (stabilize) {

        float Kp = 1.8;
        float Kd = 0.3;

        float error = 0;

        bool leftWall  = left  < OPEN_THRESHOLD;
        bool rightWall = right < OPEN_THRESHOLD;

        // 🔥 ZAWSZE licz jakiś error (brak skoków trybu)

        if (leftWall && rightWall) {
            error = (left - right);
        }
        else if (leftWall) {
            error = (left - 10.0);
        }
        else if (rightWall) {
            error = -(right - 10.0);
        }
        else {
            // 🔥 fallback na encoder (ALE miękki)
            static long lastLeftTicks = 0;
            static long lastRightTicks = 0;

            long leftTicks  = getLeftTicks();
            long rightTicks = getRightTicks();

            long dLeft  = leftTicks  - lastLeftTicks;
            long dRight = rightTicks - lastRightTicks;

            lastLeftTicks  = leftTicks;
            lastRightTicks = rightTicks;

            error = (float)(dLeft - dRight) * 0.5; // 🔥 słabszy wpływ
        }

        // 🔥 derivative z limiterem
        float derivative = error - lastError;
        if (abs(derivative) > 10) derivative = 0;

        float correction = Kp * error + Kd * derivative;

        // 🔥 mniejszy zakres
        correction = constrain(correction, -25, 25);

        leftSpeed  = baseLeft  - correction;
        rightSpeed = baseRight + correction;

        // 🔥 minimalna prędkość
        leftSpeed  = constrain(leftSpeed, 70, 150);
        rightSpeed = constrain(rightSpeed, 70, 150);

        lastError = error;
    }

    setMotorSpeed(leftSpeed, rightSpeed);
    leftMotorForward();
    rightMotorForward();
}