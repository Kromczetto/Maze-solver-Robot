#ifndef CELL_H
#define CELL_H

struct Cell {
    bool wallNorth;
    bool wallSouth;
    bool wallEast;
    bool wallWest;

    bool visited;
};

#endif