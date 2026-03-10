#include "direction.h"

static Direction currentDirection = NORTH;

Direction getDirection() {
    return currentDirection;
}

void turnLeftDirection() {
    currentDirection = (Direction)((currentDirection + 3) % 4);
}

void turnRightDirection() {
    currentDirection = (Direction)((currentDirection + 1) % 4);
}

void turnAroundDirection() {
    currentDirection = (Direction)((currentDirection + 2) % 4);
}