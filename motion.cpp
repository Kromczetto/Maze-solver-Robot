#include <Arduino.h>
#include "motion.h"
#include "motors.h"
#include "tof_sensors.h"
#include "robot_config.h"

RobotState currentState = FORWARD;

static float saveFrontDistance = 0.0;

RobotState getRobotState() {
    return currentState;
}

void driveStraightCorridor() {

    float left  = getLeftDistance();
    float right = getRightDistance();

    int baseLeft  = 110;
    int baseRight = 120;

    int correction = 30;
    int threshold  = 2;

    int leftSpeed  = baseLeft;
    int rightSpeed = baseRight;

    float diff = left - right;

    if (diff > threshold) {
    
        rightSpeed -= correction;

    }
    else if (diff < -threshold) {
      
        leftSpeed -= correction;
        
    }

    leftSpeed  = constrain(leftSpeed, 80, 200);
    rightSpeed = constrain(rightSpeed, 80, 200);

    setMotorSpeed(leftSpeed, rightSpeed);

    leftMotorForward();
    rightMotorForward();
}

void turnLeft() {
    
    setMotorSpeed(120, 0);

    leftMotorForward();
    rightMotorForward();

}

void updateMotion() {

    float left = getLeftDistance();
    float front = getFrontDistance();

    switch (currentState) {

        case FORWARD: 

            if (front < 7) {

                stopMotors();
                currentState = IDLE;
                return;

            }

            if (left > OPEN_THRESHOLD) {

                saveFrontDistance = front;
                currentState = TURNING_LEFT;
                return;

            }

            driveStraightCorridor();
            break;
        
        case TURNING_LEFT:

            if (left < 10) {

                currentState = FORWARD;
                return;

            }
     
            if (saveFrontDistance - getFrontDistance() < 10) return;

            turnLeft();
            break;
    }

}




