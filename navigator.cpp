#include "navigator.h"
#include "left_hand.h"
#include "maze.h"
#include "robot_config.h"

NavigationAlgorithm currentAlgorithm = FLOOD_FILL;
// NavigationAlgorithm currentAlgorithm = LEFT_HAND;

RobotState decideFloodFill(float left, float front, float right) {

    int x = getRobotX();
    int y = getRobotY();
    Direction dir = getRobotDir();

    int bestDir = dir;
    int bestVal = 255;

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

        int val = getCellValue(nx, ny);

        if (val < bestVal) {
            bestVal = val;
            bestDir = d;
        }
    }

    int diff = (bestDir - dir + 4) % 4;

    if (diff == 0 && front > OPEN_THRESHOLD_STRONG) return FORWARD;
    if (diff == 1 && right >OPEN_THRESHOLD_STRONG) return TURNING_RIGHT;
    if (diff == 3 && left > OPEN_THRESHOLD_STRONG) return TURNING_LEFT;

    return TURNING_AROUND;
}

RobotState getNavigationDecision(float left, float front, float right) {

    static bool turnAroundLeft = true;

    switch (currentAlgorithm) {

        case LEFT_HAND:
            return decideLeftHand(left, front, right, turnAroundLeft);

        case FLOOD_FILL:
            return decideFloodFill(left, front, right);
    }

    return IDLE;
}