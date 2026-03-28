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

    analogWrite(ENA, 200);
    analogWrite(ENB, 200);
}

void leftMotorForward() {
    digitalWrite(LEFT_IN1, HIGH);
    digitalWrite(LEFT_IN2, LOW);
}

void leftMotorBackward() {
    digitalWrite(LEFT_IN1, LOW);
    digitalWrite(LEFT_IN2, HIGH);
}

void leftMotorStop() {
    digitalWrite(LEFT_IN1, LOW);
    digitalWrite(LEFT_IN2, LOW);
}

void rightMotorForward() {
    digitalWrite(RIGHT_IN1, HIGH);
    digitalWrite(RIGHT_IN2, LOW);
}

void rightMotorBackward() {
    digitalWrite(RIGHT_IN1, LOW);
    digitalWrite(RIGHT_IN2, HIGH);
}

void rightMotorStop() {
    digitalWrite(RIGHT_IN1, LOW);
    digitalWrite(RIGHT_IN2, LOW);
}

void stopMotors() {
    leftMotorStop();
    rightMotorStop();
}