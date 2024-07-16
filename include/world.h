#ifndef WORLD_H
#define WORLD_H

#include <config.h>

class World
{

public:

    //worls size and generation
    const int cellSize = 20;
    const int worldSizeX = 6400;
    const int worldSizeY = 1800;
    const int worldStartX = (worldSizeX * cellSize / 2) * -1;
    const int worldStartY = (worldSizeY * cellSize / 2) * -1;
    const int worldEndX = worldSizeX * cellSize / 2;
    const int worldEndY = worldSizeY * cellSize / 2;

    //world settings
    float acceleretion = 0.1f;
    float velocity = 0;
    const float velocityMax = 15.0f;

private:

};


#endif
