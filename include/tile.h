#ifndef TILE_H
#define TILE_H

#include <config.h>


enum TileType {
    DIRT,
    STONE,
    CLAY,
    MUD,
    SAND,
};


class Tile
{
public:
    //Tile (int x = 0, int y =0, int textureIndex = 0, const std::string& type = "grass", bool isSolid = true);
    Texture2D tileSet = LoadTexture("../img/tiles/world_tileset.png");
    const float size = 16.0f;
    
    // Set dirt rectangles
    Rectangle dirtTop = { 0.0f, 0.0f, size, size };
    Rectangle dirtCenter = { 0.0f, size, size, size};
    Rectangle dirtRight = { 0.0f, size * 2, size, size};
    Rectangle dirtLeft = { 0.0f, size * 3, size, size};
    Rectangle dirtBott =  { 0.0f, size * 4, size, size};
    Rectangle dirtTopLeft = { 0.0f, size * 5, size, size};
    Rectangle dirtTopRight = { 0.0f, size * 6, size, size};
    Rectangle dirtBottRight = { 0.0f, size * 7, size, size};
    Rectangle dirtBottLeft = { 0.0f, size * 8, size, size};
    Rectangle dirtLeftTopRight = { 0.0f, size * 9, size, size};
    Rectangle dirtTopRightBott = { 0.0f, size * 10, size, size};
    Rectangle dirtRightBottLeft = { 0.0f, size * 11, size, size};
    Rectangle dirtBottLeftTop = { 0.0f, size * 12, size, size};
    Rectangle dirtAllSide = { 0.0f, size * 13, size, size};
    Rectangle dirtLeftRight = { 0.0f, size * 14, size, size};
    Rectangle dirtTopBott = { 0.0f, size * 15, size, size};

    // Set stone rectangles
    Rectangle stoneTop = { size, 0.0f, size, size };
    Rectangle stoneCenter = { size, size, size, size};
    Rectangle stoneRight = { size, size * 2, size, size};
    Rectangle stoneLeft = { size, size * 3, size, size};
    Rectangle stoneBott =  { size, size * 4, size, size};
    Rectangle stoneTopLeft = { size, size * 5, size, size};
    Rectangle stoneTopRight = { size, size * 6, size, size};
    Rectangle stoneBottRight = { size, size * 7, size, size};
    Rectangle stoneBottLeft = { size, size * 8, size, size};
    Rectangle stoneLeftTopRight = { size, size * 9, size, size};
    Rectangle stoneTopRightBott = { size, size * 10, size, size};
    Rectangle stoneRightBottLeft = { size, size * 11, size, size};
    Rectangle stoneBottLeftTop = { size, size * 12, size, size};
    Rectangle stoneAllSide = { size, size * 13, size, size};
    Rectangle stoneLeftRight = { size, size * 14, size, size};
    Rectangle stoneTopBott = { size, size * 15, size, size};


    Rectangle clayTop = { size * 2, 0.0f, size, size };
    Rectangle clayCenter = { size * 2, size, size, size};
    Rectangle clayRight = { size * 2, size * 2, size, size};
    Rectangle clayLeft = { size * 2, size * 3, size, size};
    Rectangle clayBott =  { size * 2, size * 4, size, size};
    Rectangle clayTopLeft = { size * 2, size * 5, size, size};
    Rectangle clayTopRight = { size * 2, size * 6, size, size};
    Rectangle clayBottRight = { size * 2, size * 7, size, size};
    Rectangle clayBottLeft = { size * 2, size * 8, size, size};
    Rectangle clayLeftTopRight = { size * 2, size * 9, size, size};
    Rectangle clayTopRightBott = { size * 2, size * 10, size, size};
    Rectangle clayRightBottLeft = { size * 2, size * 11, size, size};
    Rectangle clayBottLeftTop = { size * 2, size * 12, size, size};
    Rectangle clayAllSide = { size * 2, size * 13, size, size};
    Rectangle clayLeftRight = { size * 2, size * 14, size, size};
    Rectangle clayTopBott = { size * 2, size * 15, size, size};

    Rectangle getIconRecSource(int id) {
        return { ((float)id * size) , size * 13, size, size };
    };

    Rectangle getTileTexture(int blockType) {
        return { blockType * size, size * 13, size, size};
    };

};

#endif
