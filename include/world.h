#ifndef WORLD_H
#define WORLD_H

#include <config.h>

class World {
public:

    const float worldWidth = 6400.0f;
    const float worldHeight = 1800.0f;
    float acceleration = 0.1f;
    float velocity = 0.0f;
    float itemAcceleration = 0.5f;
    float itemVelocity = 0.0f;
    const float velocityMax = 12.0f;


    void updateWorld() {
        if (velocity < velocityMax) velocity += acceleration;
        else velocity = velocityMax;
    }
    

private:

};

#endif
