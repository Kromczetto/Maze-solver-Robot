#ifndef MAZE_H
#define MAZE_H

#include <Arduino.h>

#define MAZE_SIZE 12

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
    bool visited;
};

static const Point START = {6, 6};
static const Point GOAL  = {7, 7};

void initMaze();
void updateWalls(float left, float front, float right);
void setWall(int x, int y, int dir);
void updatePosition();
void updateDirection(int turn);

void floodFill(int goalX, int goalY);
bool isGoal();

int getRobotX();
int getRobotY();
bool isAtGoal();
Direction getRobotDir();

uint8_t getCellValue(int x, int y);
bool hasWall(int x, int y, int d);

#endif