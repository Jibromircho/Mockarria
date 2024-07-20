#ifndef TILE_H
#define TILE_H

#include <config.h>

class Tile
{
public:
    //Tile (int x = 0, int y =0, int textureIndex = 0, const std::string& type = "grass", bool isSolid = true);
    Texture2D tileSet = LoadTexture("../img/tiles/world_tileset.png");
    const float size = 16.0f;
    Rectangle grass = { 0.0f, 0.0f, size, size };
    Rectangle stone = { size * 8, 0.0f, size, size};
    Rectangle ice = { size * 6, size * 2, size, size};


private:
    Vector2 possition;
    int textureIndex; //referance in sprite sheet
    std::string type; // type of tile
    bool isSolid; // collision


};

#endif
