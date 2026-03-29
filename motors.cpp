#include <Arduino.h>
#include "motors.h"

#define LEFT_IN1 3
#define LEFT_IN2 2
#define RIGHT_IN1 5
#define RIGHT_IN2 4

#define ENA 11
#define ENB 6

void initMotors() {
    
    pinMode(LEFT_IN1, OUTPUT);
    pinMode(LEFT_IN2, OUTPUT);
    pinMode(RIGHT_IN1, OUTPUT);
    pinMode(RIGHT_IN2, OUTPUT);

    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
}

void setMotorSpeed(int leftSpeed, int rightSpeed) {

    leftSpeed  = constrain(leftSpeed, 0, 200);
    rightSpeed = constrain(rightSpeed, 0, 200);

    analogWrite(ENA, leftSpeed);
    analogWrite(ENB, rightSpeed);
}

void leftMotorForward() {

    digitalWrite(LEFT_IN1, HIGH);
    digitalWrite(LEFT_IN2, LOW);

}

void leftMotorBackward() {

    digitalWrite(LEFT_IN1, LOW);
    digitalWrite(LEFT_IN2, HIGH);

}

void rightMotorForward() {

    digitalWrite(RIGHT_IN1, HIGH);
    digitalWrite(RIGHT_IN2, LOW);

}

void rightMotorBackward() {

    digitalWrite(RIGHT_IN1, LOW);
    digitalWrite(RIGHT_IN2, HIGH);

}

void stopMotors() {

    digitalWrite(LEFT_IN1, LOW);
    digitalWrite(LEFT_IN2, LOW);
    digitalWrite(RIGHT_IN1, LOW);
    digitalWrite(RIGHT_IN2, LOW);

}