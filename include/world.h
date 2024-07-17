#ifndef WORLD_H
#define WORLD_H

#include <config.h>

class World {
public:
    World();

    int getWorldSizeX() const;
    int getWorldSizeY() const;
    float getAcceleration() const;
    float getVelocity() const;
    float getVelocityMax() const;
    int getTileMap(int x) const;

    void setAcceleration(float acc);
    void setVelocity(float vel);
    

private:
    int worldSizeX = 6400;
    int worldSizeY = 1800;
   // std::map<int, int> tileMap;
    float acceleration = 0.1f;
    float velocity = 0;
    const float velocityMax = 15.0f;

};

#endif
