#ifndef DIRECTION_H
#define DIRECTION_H

enum Direction {
    NORTH,
    EAST,
    SOUTH,
    WEST
};

Direction getDirection();

void turnLeftDirection();
void turnRightDirection();
void turnAroundDirection();

#endif