#include "world.h"


World::World() {
    
}

// Getter methods
float World::getWorldWidth() const {
    return worldWidth;
}

float World::getWorldHeight() const {
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

void World::updateWorld() {
    if (velocity < velocityMax) velocity += acceleration;
    else velocity = velocityMax;
}

