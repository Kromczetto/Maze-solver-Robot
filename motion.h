#ifndef MOTION_H
#define MOTION_H

bool isRobotIdle();

void moveForward();
void turnLeft90();
void turnRight90();
void turnAround();

void updateMotion();
void stabilizeForward();
void moveForwardShort();
void moveToCenter();

#endif