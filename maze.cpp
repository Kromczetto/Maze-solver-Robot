#include "maze.h"
#include "robot_config.h"

Cell maze[MAZE_SIZE][MAZE_SIZE];

int robotX = MAZE_SIZE / 2;
int robotY = MAZE_SIZE / 2;
Direction robotDir = NORTH;

void initMaze() {
    for (int x = 0; x < MAZE_SIZE; x++) {
        for (int y = 0; y < MAZE_SIZE; y++) {
            for (int d = 0; d < 4; d++)
                maze[x][y].walls[d] = false;

            maze[x][y].value = 255;
            maze[x][y].visited = false;
        }
    }
}

void updateWalls(float left, float front, float right) {

    if (front < WALL_THRESHOLD_CM)
        setWall(robotX, robotY, robotDir);

    if (left < WALL_THRESHOLD_CM)
        setWall(robotX, robotY, (robotDir + 3) % 4);

    if (right < WALL_THRESHOLD_CM)
        setWall(robotX, robotY, (robotDir + 1) % 4);

    maze[robotX][robotY].visited = true;
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
    if (turn == 1) robotDir = (Direction)((robotDir + 1) % 4);
    if (turn == -1) robotDir = (Direction)((robotDir + 3) % 4);
    if (turn == 2) robotDir = (Direction)((robotDir + 2) % 4);
}

void floodFill(int goalX, int goalY) {

    for (int x = 0; x < MAZE_SIZE; x++)
        for (int y = 0; y < MAZE_SIZE; y++)
            maze[x][y].value = 255;

    maze[goalX][goalY].value = 0;

    bool changed = true;

    while (changed) {
        changed = false;

        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) {

                for (int d = 0; d < 4; d++) {

                    if (maze[x][y].walls[d]) continue;

                    int nx = x;
                    int ny = y;

                    if (d == NORTH) ny++;
                    if (d == EAST)  nx++;
                    if (d == SOUTH) ny--;
                    if (d == WEST)  nx--;

                    if (nx < 0 || ny < 0 || nx >= MAZE_SIZE || ny >= MAZE_SIZE)
                        continue;

                    if (maze[nx][ny].value + 1 < maze[x][y].value) {
                        maze[x][y].value = maze[nx][ny].value + 1;
                        changed = true;
                    }
                }
            }
        }
    }
}

bool isGoal() {
    return (robotX == 7 || robotX == 8) &&
           (robotY == 7 || robotY == 8);
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
    return (getRobotX() == GOAL.x && getRobotY() == GOAL.y);
}