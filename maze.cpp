#include "maze.h"
#include "robot_config.h"

Cell maze[MAZE_SIZE][MAZE_SIZE];

int robotX = START.x;
int robotY = START.y;
Direction robotDir = NORTH;

struct Node { uint8_t x, y; };

static Node queue[MAZE_SIZE * MAZE_SIZE];
static int head = 0;
static int tail = 0;
static bool floodRunning = false;

void initMaze() {
    for (int x = 0; x < MAZE_SIZE; x++) {
        for (int y = 0; y < MAZE_SIZE; y++) {
            for (int d = 0; d < 4; d++) {
                maze[x][y].walls[d] = false;
                maze[x][y].visits[d] = 0;
            }

            maze[x][y].value = 255;
        }
    }

    for (int x = 0; x < MAZE_SIZE; x++) {
        for (int y = 0; y < MAZE_SIZE; y++) {

            if (y == MAZE_SIZE - 1) maze[x][y].walls[NORTH] = true;
            if (x == MAZE_SIZE - 1) maze[x][y].walls[EAST] = true;
            if (y == 0) maze[x][y].walls[SOUTH] = true;
            if (x == 0) maze[x][y].walls[WEST] = true;
        }
    }
}

void floodFillStart() {

    head = 0;
    tail = 0;

    for (int x = 0; x < MAZE_SIZE; x++) {
        for (int y = 0; y < MAZE_SIZE; y++) {
            maze[x][y].value = 255;
        }
    }

    maze[GOAL.x][GOAL.y].value = 0;
    queue[tail++] = { (uint8_t)GOAL.x, (uint8_t)GOAL.y };

    floodRunning = true;
}

void floodFillStep() {

    if (!floodRunning) return;

    while (head < tail) {

        Node n = queue[head++];
        int x = n.x;
        int y = n.y;

        int base = maze[x][y].value;

        for (int d = 0; d < 4; d++) {

            if (hasWall(x, y, d)) continue;

            int nx = x, ny = y;

            if (d == NORTH) ny++;
            if (d == EAST) nx++;
            if (d == SOUTH) ny--;
            if (d == WEST) nx--;

            if (nx < 0 || ny < 0 || nx >= MAZE_SIZE || ny >= MAZE_SIZE)
                continue;

            int newVal = base + 1;

            if (maze[nx][ny].value > newVal) {
                maze[nx][ny].value = newVal;
                queue[tail++] = {nx, ny};
            }
        }
    }

    floodRunning = false;
}

bool isFloodFillDone() { return !floodRunning; }

void updateWalls(float left, float front, float right) {

    bool wallFront = (front < 16);
    bool wallLeft = (left < 16);
    bool wallRight = (right < 16);

    if (wallFront) setWall(robotX, robotY, robotDir);
    if (wallLeft) setWall(robotX, robotY, (robotDir + 3) % 4);
    if (wallRight) setWall(robotX, robotY, (robotDir + 1) % 4);
}

void setWall(int x, int y, int dir) {

    maze[x][y].walls[dir] = true;

    int nx = x, ny = y;
    if (dir == NORTH) ny++;
    if (dir == EAST) nx++;
    if (dir == SOUTH) ny--;
    if (dir == WEST) nx--;

    if (nx < 0 || ny < 0 || nx >= MAZE_SIZE || ny >= MAZE_SIZE)
        return;

    maze[nx][ny].walls[(dir + 2) % 4] = true;
}

void updatePosition() {
    if (robotDir == NORTH) robotY++;
    if (robotDir == EAST) robotX++;
    if (robotDir == SOUTH) robotY--;
    if (robotDir == WEST) robotX--;

    robotX = constrain(robotX, 0, MAZE_SIZE - 1);
    robotY = constrain(robotY, 0, MAZE_SIZE - 1);
}

void updateDirection(int turn) {
    if (turn == 1) robotDir = (Direction)((robotDir + 1) % 4);
    if (turn == -1) robotDir = (Direction)((robotDir + 3) % 4);
    if (turn == 2) robotDir = (Direction)((robotDir + 2) % 4);
}

int getRobotX() { return robotX; }
int getRobotY() { return robotY; }
Direction getRobotDir() { return robotDir; }

uint8_t getCellValue(int x, int y) { return maze[x][y].value; }
bool hasWall(int x, int y, int d) { return maze[x][y].walls[d]; }

bool isAtGoal() {
    return (robotX == GOAL.x && robotY == GOAL.y);
}

bool isCellConsistent(int x, int y) {

    int current = maze[x][y].value;
    int minN = 255;

    for (int d = 0; d < 4; d++) {

        if (hasWall(x, y, d)) continue;

        int nx = x, ny = y;
        if (d == NORTH) ny++;
        if (d == EAST) nx++;
        if (d == SOUTH) ny--;
        if (d == WEST) nx--;

        if (nx < 0 || ny < 0 || nx >= MAZE_SIZE || ny >= MAZE_SIZE)
            continue;

        minN = min(minN, (int)maze[nx][ny].value);
    }

    if (minN == 255) return true;
    return current == minN + 1;
}

void sendMazeDebugBT(Stream& bt) {

    bt.println("MAZE_START");

    for (int y = 0; y < MAZE_SIZE; y++) {
        for (int x = 0; x < MAZE_SIZE; x++) {
            bt.print(maze[x][y].value);
            bt.print(",");
        }
        bt.println();
    }

    bt.println("MAZE_END");
}

uint8_t getVisit(int x, int y, int d) {
    return maze[x][y].visits[d];
}

void addVisit(int x, int y, int d) {

    if (maze[x][y].visits[d] < 2)
        maze[x][y].visits[d]++;

    int nx = x, ny = y;

    if (d == NORTH) ny++;
    if (d == EAST) nx++;
    if (d == SOUTH) ny--;
    if (d == WEST) nx--;

    if (nx < 0 || ny < 0 || nx >= MAZE_SIZE || ny >= MAZE_SIZE)
        return;

    if (maze[nx][ny].visits[(d + 2) % 4] < 2)
        maze[nx][ny].visits[(d + 2) % 4]++;
}

uint8_t getCellVisit(int x, int y) {

    uint8_t maxV = 0;

    for (int d = 0; d < 4; d++) {
        if (maze[x][y].visits[d] > maxV)
            maxV = maze[x][y].visits[d];
    }

    return maxV;
}

void computeGreedyValues() {

    for (int x = 0; x < MAZE_SIZE; x++) {
        for (int y = 0; y < MAZE_SIZE; y++) {

            int dist = abs(x - GOAL.x) + abs(y - GOAL.y);
            int visits = getCellVisit(x, y);

            int val = dist + (visits >= 2 ? 10 : visits);

            if (val > 15) val = 15;

            maze[x][y].value = (uint8_t)val;
        }
    }
}