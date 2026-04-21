#ifndef MAZE_H
#define MAZE_H

#include <Arduino.h>

enum Direction {
    NORTH = 0,
    EAST,
    SOUTH,
    WEST
};

struct Point {
    int x;
    int y;
};

struct Cell {
    bool walls[4];
    uint8_t value;
};

static const Point START = {5, 5};
static const Point GOAL = {6, 6};

void initMaze();
void updateWalls(float left, float front, float right);
void setWall(int x, int y, int dir);
void updatePosition();
void updateDirection(int turn);

int getRobotX();
int getRobotY();
bool isAtGoal();
Direction getRobotDir();

uint8_t getCellValue(int x, int y);
bool hasWall(int x, int y, int d);

void floodFillStart();
void floodFillStep();
bool isFloodFillDone();

#endif