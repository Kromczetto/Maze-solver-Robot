#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "motion.h"
#include "maze.h"

void setPledgeHeading(Direction dir);
RobotState getNavigationDecision(float left, float front, float right);

#endif