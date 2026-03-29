#ifndef MOTORS_H
#define MOTORS_H

void initMotors();

void setMotorSpeed(int leftSpeed, int rightSpeed);

void leftMotorForward();
void leftMotorBackward();
void leftMotorStop();

void rightMotorForward();
void rightMotorBackward();
void rightMotorStop();

void stopMotors();

#endif