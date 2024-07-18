#ifndef WORLD_H
#define WORLD_H

#include <config.h>

class World {
public:
    World();

    int getWorldWidth() const;
    int getWorldHeight() const;
    float getAcceleration() const;
    float getVelocity() const;
    float getVelocityMax() const;
    int getTileMap(int x) const;

    void setAcceleration(float acc);
    void setVelocity(float vel);
    

private:
    const int worldWidth = 900;
    const int worldHeight = 400;
    //const int worldWidth = 6400;
    //const int worldHeight = 1800;
    float acceleration = 0.1f;
    float velocity = 0;
    const float velocityMax = 15.0f;

};

#endif
