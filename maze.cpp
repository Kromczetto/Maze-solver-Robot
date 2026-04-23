#include "maze.h"
#include "robot_config.h"

Cell maze[MAZE_SIZE][MAZE_SIZE];

int robotX = START.x;
int robotY = START.y;
Direction robotDir = NORTH;

struct Node {
    uint8_t x;
    uint8_t y;
};

static Node queue[MAZE_SIZE * MAZE_SIZE];
static int head = 0;
static int tail = 0;
static bool floodRunning = false;

void initMaze() {
    for (int x = 0; x < MAZE_SIZE; x++) {
        for (int y = 0; y < MAZE_SIZE; y++) {
            for (int d = 0; d < 4; d++)
                maze[x][y].walls[d] = false;

            maze[x][y].value = 255;
        }
    }
}

void floodFillStart() {

    for (int x = 0; x < MAZE_SIZE; x++) {
        for (int y = 0; y < MAZE_SIZE; y++) {
            maze[x][y].value = 255;
        }
    }

    head = 0;
    tail = 0;

    maze[GOAL.x][GOAL.y].value = 0;
    queue[tail++] = {GOAL.x, GOAL.y};

    floodRunning = true;
}

void floodFillStep() {

    if (!floodRunning) return;

    int steps = 4;

    while (steps-- && head < tail) {

        Node current = queue[head++];

        int x = current.x;
        int y = current.y;

        int baseVal = maze[x][y].value;

        for (int d = 0; d < 4; d++) {

            if (hasWall(x, y, d)) continue;

            int nx = x;
            int ny = y;

            if (d == NORTH) ny++;
            if (d == EAST)  nx++;
            if (d == SOUTH) ny--;
            if (d == WEST)  nx--;

            if (nx < 0 || ny < 0 || nx >= MAZE_SIZE || ny >= MAZE_SIZE)
                continue;

            if (maze[nx][ny].value > baseVal + 1) {

                maze[nx][ny].value = baseVal + 1;

                if (tail < MAZE_SIZE * MAZE_SIZE) {
                    queue[tail++] = {nx, ny};
                }
            }
        }

        if (tail >= MAZE_SIZE * MAZE_SIZE) {
            floodRunning = false;
            return;
        }
    }

    if (head >= tail) {
        floodRunning = false;
    }
}

bool isFloodFillDone() {
    return !floodRunning;
}

void updateWalls(float left, float front, float right) {

    bool wallFront = (front < 16);
    bool wallLeft  = (left  < 16);
    bool wallRight = (right < 16);

    if (wallFront) setWall(robotX, robotY, robotDir);
    if (wallLeft)  setWall(robotX, robotY, (robotDir + 3) % 4);
    if (wallRight) setWall(robotX, robotY, (robotDir + 1) % 4);
}

void setWall(int x, int y, int dir) {

    maze[x][y].walls[dir] = true;

    int nx = x;
    int ny = y;

    if (dir == NORTH) ny++;
    if (dir == EAST)  nx++;
    if (dir == SOUTH) ny--;
    if (dir == WEST)  nx--;

    if (nx < 0 || ny < 0 || nx >= MAZE_SIZE || ny >= MAZE_SIZE)
        return;

    maze[nx][ny].walls[(dir + 2) % 4] = true;
}

void updatePosition() {
    if (robotDir == NORTH) robotY++;
    if (robotDir == EAST)  robotX++;
    if (robotDir == SOUTH) robotY--;
    if (robotDir == WEST)  robotX--;

    robotX = constrain(robotX, 0, MAZE_SIZE - 1);
    robotY = constrain(robotY, 0, MAZE_SIZE - 1);
}

void updateDirection(int turn) {
    if (turn == 1)  robotDir = (Direction)((robotDir + 1) % 4);
    if (turn == -1) robotDir = (Direction)((robotDir + 3) % 4);
    if (turn == 2)  robotDir = (Direction)((robotDir + 2) % 4);
}

int getRobotX() { return robotX; }
int getRobotY() { return robotY; }
Direction getRobotDir() { return robotDir; }

uint8_t getCellValue(int x, int y) {
    return maze[x][y].value;
}

bool hasWall(int x, int y, int d) {
    return maze[x][y].walls[d];
}

bool isAtGoal() {
    return (robotX == GOAL.x && robotY == GOAL.y);
}

bool isCellConsistent(int x, int y) {

    int current = maze[x][y].value;
    int minNeighbor = 255;

    for (int d = 0; d < 4; d++) {

        if (hasWall(x, y, d)) continue;

        int nx = x;
        int ny = y;

        if (d == NORTH) ny++;
        if (d == EAST)  nx++;
        if (d == SOUTH) ny--;
        if (d == WEST)  nx--;

        if (nx < 0 || ny < 0 || nx >= MAZE_SIZE || ny >= MAZE_SIZE)
            continue;

        int val = maze[nx][ny].value;

        if (val < minNeighbor) {
            minNeighbor = val;
        }
    }

    return current == minNeighbor + 1;
}