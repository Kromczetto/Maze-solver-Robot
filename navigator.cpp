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
    int bestScore = 10000;

    for (int d = 0; d < 4; d++) {

        if (d == dir && front < 10) continue;
        if ((d - dir + 4) % 4 == 3 && left < 10) continue;
        if ((d - dir + 4) % 4 == 1 && right < 10) continue;

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

        int score = val * 10;

        if (d == dir) score -= 2;             
        if ((d - dir + 4) % 4 == 3) score -= 1;

        if (score < bestScore) {
            bestScore = score;
            bestDir = d;
        }
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
    }

    return IDLE;
}