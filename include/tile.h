#ifndef TILE_H
#define TILE_H

#include <config.h>

class Tile
{
public:
    //Tile (int x = 0, int y =0, int textureIndex = 0, const std::string& type = "grass", bool isSolid = true);
    Texture2D tileSet = LoadTexture("../img/tiles/world_tileset_d.png");
    const float size = 16.0f;
    Rectangle dirtTop = { 0.0f, 0.0f, size, size };
    Rectangle dirtCenter = { 0.0f, size, size, size};
    Rectangle dirtRight = { 0.0f, size * 2, size, size};
    Rectangle dirtLeft = { 0.0f, size * 3, size, size};
    Rectangle dirtBott =  { 0.0f, size * 4, size, size};
    Rectangle dirtTopLeft = { 0.0f, size * 5, size, size};
    Rectangle dirtTopRight = { 0.0f, size * 6, size, size};
    Rectangle dirtBottRight = { 0.0f, size * 7, size, size};
    Rectangle dirtBottLeft = { 0.0f, size * 8, size, size};

    //////
    Rectangle stoneTop = { size, 0.0f, size, size};
    Rectangle stoneMid = { size, size, size, size};
    Rectangle clay = { size * 2, 0.0f, size, size};


private:
    Vector2 possition;
    int textureIndex; //referance in sprite sheet
    std::string type; // type of tile
    bool isSolid; // collision


};

#endif
