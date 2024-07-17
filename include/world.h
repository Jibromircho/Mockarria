#ifndef WORLD_H
#define WORLD_H

#include <vector> // for std::vector

class World {
public:
    // Constructor and Destructor
    World(int sizeX = 6400, int sizeY = 1800);
    ~World();

    // World settings
    float acceleration = 0.1f;
    float velocity = 0;
    const float velocityMax = 15.0f;

    // Methods to get and set tiles
    int getTile(int x, int y) const;
    void setTile(int x, int y, int tileType);

    // World dimensions
    int getSizeX() const { return worldSizeX; }
    int getSizeY() const { return worldSizeY; }

private:
    // World size and generation
    const int worldSizeX;
    const int worldSizeY;
    std::vector<std::vector<int>> tileMap; // Dynamic 2D array for the tile map
};

// Inline implementations

// Constructor: initialize the tile map with default values
inline World::World(int sizeX, int sizeY) 
    : worldSizeX(sizeX), worldSizeY(sizeY), tileMap(sizeX, std::vector<int>(sizeY, 0)) {}

// Destructor: no manual memory management needed due to std::vector
inline World::~World() {}

// Get the tile type at a specific position
inline int World::getTile(int x, int y) const {
    if (x >= 0 && x < worldSizeX && y >= 0 && y < worldSizeY) {
        return tileMap[x][y];
    }
    return -1; // Return an invalid tile type for out-of-bounds access
}

// Set the tile type at a specific position
inline void World::setTile(int x, int y, int tileType) {
    if (x >= 0 && x < worldSizeX && y >= 0 && y < worldSizeY) {
        tileMap[x][y] = tileType;
    }
}

#endif // WORLD_H
