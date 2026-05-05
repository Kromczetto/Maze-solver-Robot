#ifndef MAZE_H
#define MAZE_H

#include <Arduino.h>

enum Direction { NORTH = 0, EAST, SOUTH, WEST };

struct Point { int x; int y; };

struct Cell {
    bool walls[4];
    uint8_t value;
    uint8_t visits[4]; 
    uint8_t greedy;
};

static const Point START = {0, 0};
static const Point GOAL = {5, 5};

void initMaze();
void resetVisits();
void updateWalls(float left, float front, float right);
void setWall(int x, int y, int dir);
void updatePosition();
void updateDirection(int turn);
bool isCellConsistent(int x, int y);
void computeGreedyValues();

int getRobotX();
int getRobotY();
bool isAtGoal();
Direction getRobotDir();

uint8_t getCellValue(int x, int y);
uint8_t getGreedyValue(int x, int y);
bool hasWall(int x, int y, int d);

void floodFillStart();
void floodFillStep();
bool isFloodFillDone();

void sendMazeDebugBT(Stream& bt);

uint8_t getVisit(int x, int y, int d);
void addVisit(int x, int y, int d);
uint8_t getCellVisit(int x, int y);

void resetRobotState();
void computeFullFloodFill();

#endif