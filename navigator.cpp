#include "navigator.h"
#include "left_hand.h"

NavigationAlgorithm currentAlgorithm = LEFT_HAND;

RobotState getNavigationDecision(float left, float front, float right) {

    static bool turnAroundLeft = true;

    switch (currentAlgorithm) {

        case LEFT_HAND:
            return decideLeftHand(left, front, right, turnAroundLeft);

        case FLOOD_FILL:
            // przyszłość
            return FORWARD;
    }

    return FORWARD;
}