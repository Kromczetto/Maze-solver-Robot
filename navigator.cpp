#include "navigator.h"
#include "left_hand.h"
#include "maze.h"
#include "robot_config.h"

// NavigationAlgorithm currentAlgorithm = FLOOD_FILL;
// NavigationAlgorithm currentAlgorithm = LEFT_HAND;
// NavigationAlgorithm currentAlgorithm = TREMAUX;
// NavigationAlgorithm currentAlgorithm = DEAD_END_FILL;
NavigationAlgorithm currentAlgorithm = GREEDY;

static int pledgeTurnSum = 0;
static Direction pledgeHeading = NORTH;
static bool followingWall = false;

void normalizeTurnSum() {
    while (pledgeTurnSum > 180) pledgeTurnSum -= 360;
    while (pledgeTurnSum < -180) pledgeTurnSum += 360;
}

RobotState decideTremaux(float left, float front, float right) {

    int x = getRobotX();
    int y = getRobotY();
    Direction dir = getRobotDir();

    int bestDir = -1;
    int bestVisit = 3;
    int bestPriority = 100;

    for (int d = 0; d < 4; d++) {

        if (hasWall(x, y, d)) continue;

        int visit = getVisit(x, y, d);

        if (visit >= 2) continue;

        int diff = (d - dir + 4) % 4;

        int priority;
        if (diff == 3) priority = 0;
        else if (diff == 0) priority = 1;
        else if (diff == 1) priority = 2;
        else priority = 3;

        if (visit < bestVisit || (visit == bestVisit && priority < bestPriority)) {
            bestVisit = visit;
            bestDir = d;
            bestPriority = priority;
        }
    }

    if (bestDir == -1) {

        for (int d = 0; d < 4; d++) {

            if (hasWall(x, y, d)) continue;

            int diff = (d - dir + 4) % 4;

            if (diff == 3) return TURNING_LEFT;
            if (diff == 0) return FORWARD;
            if (diff == 1) return TURNING_RIGHT;
        }

        return TURNING_AROUND;
    }

    addVisit(x, y, bestDir);

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
    int bestDist = 999;
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

        int dist = abs(nx - GOAL.x) + abs(ny - GOAL.y);

        int diff = (d - dir + 4) % 4;

        int priority;
        if (diff == 3) priority = 0;
        else if (diff == 0) priority = 1;
        else if (diff == 1) priority = 2;
        else priority = 3;

        if (dist < bestDist || (dist == bestDist && priority < bestPriority)) {
            bestDist = dist;
            bestDir = d;
            bestPriority = priority;
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