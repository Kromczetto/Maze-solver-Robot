#include <Arduino.h>
#include "motion.h"
#include "motors.h"
#include "tof_sensors.h"

void driveStraightCorridor() {

    float left  = getLeftDistance();
    float right = getRightDistance();

    int baseLeft  = 110;
    int baseRight = 120;

    int correction = 30;
    int threshold  = 3;

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