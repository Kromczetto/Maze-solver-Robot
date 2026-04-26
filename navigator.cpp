#include "navigator.h"
#include "left_hand.h"
#include "maze.h"
#include "robot_config.h"

// NavigationAlgorithm currentAlgorithm = FLOOD_FILL;
// NavigationAlgorithm currentAlgorithm = LEFT_HAND;
// NavigationAlgorithm currentAlgorithm = TREMAUX;
NavigationAlgorithm currentAlgorithm = PLEDGE;

static int pledgeTurnSum = 0;
static Direction pledgeHeading = NORTH;

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

RobotState decidePledge(float left, float front, float right) {

    int x = getRobotX();
    int y = getRobotY();
    Direction dir = getRobotDir();

    if (pledgeTurnSum == 0) {

        int d = pledgeHeading;

        if (!hasWall(x, y, d)) {

            int diff = (d - dir + 4) % 4;

            if (diff == 0) return FORWARD;
            if (diff == 1) return TURNING_RIGHT;
            if (diff == 3) return TURNING_LEFT;

            return TURNING_AROUND;
        }
    }

    if (!hasWall(x, y, (dir + 1) % 4)) {
        pledgeTurnSum += 90;
        return TURNING_RIGHT;
    }

    if (!hasWall(x, y, dir)) {
        return FORWARD;
    }

    if (!hasWall(x, y, (dir + 3) % 4)) {
        pledgeTurnSum -= 90;
        return TURNING_LEFT;
    }

    pledgeTurnSum += 180;
    return TURNING_AROUND;
}

void setPledgeHeading(Direction dir) {
    pledgeHeading = dir;
    pledgeTurnSum = 0; /
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

        case PLEDGE:
            return decidePledge(left, front, right);
    }

    return IDLE;
}