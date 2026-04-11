#ifndef MOTORS_H
#define MOTORS_H

void initMotors();

void setMotorSpeed(int leftSpeed, int rightSpeed);

void leftMotorForward();
void leftMotorBackward();

void rightMotorForward();
void rightMotorBackward();

void stopMotors();

#endif