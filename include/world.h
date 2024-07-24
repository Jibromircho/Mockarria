#ifndef WORLD_H
#define WORLD_H

#include <config.h>

class World {
public:
    World();

    float getWorldWidth() const;
    float getWorldHeight() const;
    float getAcceleration() const;
    float getVelocity() const;
    float getVelocityMax() const;
    int getTileMap(int x) const;

    void setAcceleration(float acc);
    void setVelocity(float vel);
    

private:
    const float worldWidth = 6400.0f;
    const float worldHeight = 1800.0f;
    float acceleration = 0.1f;
    float velocity = 0.0f;
    const float velocityMax = 15.0f;

};

#endif
