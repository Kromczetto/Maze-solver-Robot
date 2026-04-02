#include <Arduino.h>
#include "motion.h"
#include "motors.h"
#include "tof_sensors.h"
#include "robot_config.h"
#include "mpu.h"

RobotState currentState = FORWARD;

static float saveFrontDistance = 0.0;
unsigned long prepareStartTime = 0;

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
    
    setMotorSpeed(120, 120);

    leftMotorForward();
    rightMotorBackward();

}

void turnRight() {

    setMotorSpeed(120, 120);
    
    leftMotorBackward();
    rightMotorForward();

}

void turnAround() {
        
    setMotorSpeed(120, 120);
    
    if (getLeftDistance() > getRightDistance()) {

        leftMotorBackward();
        rightMotorForward();

    } else {

        leftMotorForward();
        rightMotorForward();
    }

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
                currentState = PREPARE_TURN_LEFT;
                return;

            }

            if (right > OPEN_THRESHOLD) {

                saveFrontDistance = front;
                resetAngle();
                currentState = PREPARE_TURN_RIGHT;
                return;

            }

            if (front < 10 && right < 12 && left < 12) {
                currentState = TURNING_AROUND;
            }

            driveStraightCorridor();
            break;
        
        case TURNING_LEFT: {

            float angle = getAngle();

            turnLeft(); 

            if (angle >= TURN_ANGLE) {

                stopMotors(); 
                currentState = FORWARD;
                return;

            }

            break;
        }

       case TURNING_RIGHT: {

            float angle = getAngle();

            turnRight(); 

            if (angle <= -TURN_ANGLE) {

                stopMotors();
                currentState = FORWARD;
                return;
            }

            break;
        }

        case IDLE:

            stopMotors();
            break;

        case PREPARE_TURN_LEFT: 

            if (prepareStartTime == 0) {
                prepareStartTime = millis();
            } 

            setMotorSpeed(120, 120);
            leftMotorForward();
            rightMotorForward();

            if (millis() - prepareStartTime > 300) {

                prepareStartTime = 0;
                resetAngle();
                currentState = TURNING_LEFT;
            }

            break;
        
        case PREPARE_TURN_RIGHT: 

            if (prepareStartTime == 0) {
                prepareStartTime = millis();
            }
            
            setMotorSpeed(120, 120);
            leftMotorForward();
            rightMotorForward();

            if (millis() - prepareStartTime > 300) {

                prepareStartTime = 0;
                resetAngle();
                currentState = TURNING_RIGHT;

            }

            break;

        case TURNING_AROUND:

            if (front > 20) {
                currentState = FORWARD;
            }

            turnAround();

            break;
    }

}




