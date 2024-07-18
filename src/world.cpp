#include "world.h"


World::World() {
    
}

// Getter methods
int World::getWorldWidth() const {
    return worldWidth;
}

int World::getWorldHeight() const {
    return worldHeight;
}

float World::getAcceleration() const {
    return acceleration;
}

float World::getVelocity() const {
    return velocity;
}

float World::getVelocityMax() const {
    return velocityMax;
}

int World::getTileMap(int x) const {
    //return tileMap[x];
}

// Setter methods
void World::setAcceleration(float acc) {
    acceleration = acc;
}

void World::setVelocity(float vel) {
    if (vel <= velocityMax) {
        velocity = vel;
    } else {
        velocity = velocityMax; // Ensure velocity does not exceed maximum
    }
}

