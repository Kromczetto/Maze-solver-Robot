#include "navigator.h"
#include "left_hand.h"
#include "maze.h"
#include "robot_config.h"

NavigationAlgorithm currentAlgorithm = GREEDY;
extern SoftwareSerial SUART;

RobotState decideTremaux(float left, float front, float right) {

    int x = getRobotX();
    int y = getRobotY();
    Direction dir = getRobotDir();

    int bestDir = -1;
    int bestPriority = 100;

    auto isOpen = [&](int d) {

        // sprawdzamy sensory względem kierunku robota
        if (d == dir) return front > 16;
        if (d == (dir + 3) % 4) return left > 16;
        if (d == (dir + 1) % 4) return right > 16;

        return true; // tył – zakładamy że można zawrócić
    };

    auto getPriority = [&](int d) {

        int diff = (d - dir + 4) % 4;

        // 🔥 NOWY PRIORYTET: FORWARD > LEFT > RIGHT > BACK
        if (diff == 0) return 0;      // FORWARD
        if (diff == 3) return 1;      // LEFT
        if (diff == 1) return 2;      // RIGHT
        return 3;                     // BACK
    };

    // =========================================
    // 🔴 1. NAJPIERW visit == 0
    // =========================================
    for (int d = 0; d < 4; d++) {

        if (hasWall(x, y, d)) continue;
        if (!isOpen(d)) continue;

        if (getVisit(x, y, d) != 0) continue;

        int priority = getPriority(d);

        if (priority < bestPriority) {
            bestPriority = priority;
            bestDir = d;
        }
    }

    // =========================================
    // 🔴 2. POTEM visit == 1
    // =========================================
    if (bestDir == -1) {

        bestPriority = 100;

        for (int d = 0; d < 4; d++) {

            if (hasWall(x, y, d)) continue;
            if (!isOpen(d)) continue;

            if (getVisit(x, y, d) != 1) continue;

            int priority = getPriority(d);

            if (priority < bestPriority) {
                bestPriority = priority;
                bestDir = d;
            }
        }
    }

    // =========================================
    // 🔴 3. JEŚLI NIC → ZAWRÓT
    // =========================================
    if (bestDir == -1) {
        return TURNING_AROUND;
    }

    // =========================================
    // 🔴 ZAPIS VISIT
    // =========================================
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