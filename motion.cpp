#include <Arduino.h>
#include "motion.h"
#include "motors.h"
#include "tof_sensors.h"
#include "robot_config.h"
#include "mpu.h"

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

void turnRight() {

    setMotorSpeed(0, 120);

    leftMotorForward();
    rightMotorForward();

}

void updateMotion() {

    float left = getLeftDistance();
    float right = getRightDistance();
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
                resetAngle();
                currentState = TURNING_LEFT;
                return;

            }

            if (right > OPEN_THRESHOLD) {

                saveFrontDistance = front;
                resetAngle();
                currentState = TURNING_RIGHT;
                return;

            }

            driveStraightCorridor();
            break;
        
        case TURNING_LEFT:

            float angle = getAngle();

            if (angle >= TURN_ANGLE) {

                currentState = FORWARD;
                return;

            }
     
            if (saveFrontDistance - getFrontDistance() < 10) return;

            turnLeft();
            break;

        case TURNING_RIGHT:

            angle = getAngle();

            if (angle <= -TURN_ANGLE) {
            
                currentState = FORWARD;
                return;
            }

            if (saveFrontDistance - getFrontDistance() < 10) return;

            turnRight();
            break;

        case IDLE:
            stopMotors();
            break;
    }

}




