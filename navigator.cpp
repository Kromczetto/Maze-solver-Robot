#include "navigator.h"
#include "left_hand.h"
#include "maze.h"
#include "robot_config.h"

NavigationAlgorithm currentAlgorithm = GREEDY;
extern SoftwareSerial SUART;
extern bool turnAroundLeftDirection;

RobotState decideTremaux(float left, float front, float right) {

    int x = getRobotX();
    int y = getRobotY();
    Direction dir = getRobotDir();

    int bestDir = -1;
    int bestPriority = 100;

    auto getPriority = [&](int d) {
        int diff = (d - dir + 4) % 4;
        if (diff == 3) return 0;
        if (diff == 0) return 1;
        if (diff == 1) return 2;
        return 3;
    };

    for (int d = 0; d < 4; d++) {
        if (hasWall(x, y, d)) continue;
        if (getVisit(x, y, d) != 0) continue;

        int p = getPriority(d);
        if (p < bestPriority) {
            bestPriority = p;
            bestDir = d;
        }
    }

    if (bestDir == -1) {
        bestPriority = 100;

        for (int d = 0; d < 4; d++) {
            if (hasWall(x, y, d)) continue;
            if (getVisit(x, y, d) != 1) continue;

            int p = getPriority(d);
            if (p < bestPriority) {
                bestPriority = p;
                bestDir = d;
            }
        }
    }

    if (bestDir == -1) {
        bestPriority = 100;

        for (int d = 0; d < 4; d++) {
            if (hasWall(x, y, d)) continue;
            if (getVisit(x, y, d) != 2) continue;

            int p = getPriority(d);
            if (p < bestPriority) {
                bestPriority = p;
                bestDir = d;
            }
        }
    }

    if (bestDir == -1) {
        return TURNING_AROUND;
    }

    int diff = (bestDir - dir + 4) % 4;

    if (diff == 0) return FORWARD;
    if (diff == 1) return TURNING_RIGHT;
    if (diff == 3) return TURNING_LEFT;

    return TURNING_AROUND;
}

RobotState decideFloodFill(float left, float front, float right) {

    int x = getRobotX();
    int y = getRobotY();
    Direction dir = getRobotDir();

    int bestDir = -1;
    int bestVal = 255;
    int bestPriority = 100;

    for (int d = 0; d < 4; d++) {

        if (hasWall(x, y, d)) continue;

        int nx = x, ny = y;

        if (d == NORTH) ny++;
        if (d == EAST)  nx++;
        if (d == SOUTH) ny--;
        if (d == WEST)  nx--;

        if (nx < 0 || ny < 0 || nx >= MAZE_SIZE || ny >= MAZE_SIZE)
            continue;

        int val = getCellValue(nx, ny);

        int diff = (d - dir + 4) % 4;

        int priority;

        if (diff == 3) priority = 0;
        else if (diff == 0) priority = 1;
        else if (diff == 1) priority = 2;
        else priority = 3;

        if (val < bestVal || (val == bestVal && priority < bestPriority)) {
            bestVal = val;
            bestDir = d;
            bestPriority = priority;
        }
    }

    int diff = (bestDir - dir + 4) % 4;

    if (diff == 0) return FORWARD;
    if (diff == 1) return TURNING_RIGHT;
    if (diff == 3) return TURNING_LEFT;

    return TURNING_AROUND;
}

RobotState decideGreedy(float left, float front, float right) {

    int x = getRobotX();
    int y = getRobotY();
    Direction dir = getRobotDir();

    int bestDir = -1;
    int bestCost = 9999;
    int bestPriority = 100;

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

        int dist = abs(nx - GOAL.x) + abs(ny - GOAL.y);

        int visits = getVisit(x, y, d);

        int cost = dist + visits * 5;

        int diff = (d - dir + 4) % 4;

        int priority;
        if (diff == 0) priority = 0;
        else if (diff == 1) priority = 1;
        else if (diff == 3) priority = 1;
        else priority = 2;

        if (cost < bestCost || (cost == bestCost && priority < bestPriority)) {
            bestCost = cost;
            bestDir = d;
            bestPriority = priority;
        }
    }

    if (bestDir == -1) {
        addVisit(x, y, dir);
        return TURNING_AROUND;
    }

    addVisit(x, y, bestDir);

    int diff = (bestDir - dir + 4) % 4;

    if (diff == 0) return FORWARD;
    if (diff == 1) return TURNING_RIGHT;
    if (diff == 3) return TURNING_LEFT;

    return TURNING_AROUND;
}

RobotState getNavigationDecision(float left, float front, float right) {

    static bool turnAroundLeft = true;

    switch (currentAlgorithm) {

        case LEFT_HAND:
            return decideLeftHand(left, front, right, turnAroundLeft);

        case FLOOD_FILL:
            return decideFloodFill(left, front, right);

        case TREMAUX:
            return decideTremaux(left, front, right);

        case GREEDY:
            return decideGreedy(left, front, right);

    }

    return IDLE;
}