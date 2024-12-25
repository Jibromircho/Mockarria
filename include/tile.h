#ifndef TILE_H
#define TILE_H

#include <config.h>


class Tile
{
public:
    //Tile (int x = 0, int y =0, int textureIndex = 0, const std::string& type = "grass", bool isSolid = true);
    Texture2D tileSet = LoadTexture("../img/tiles/world_tileset.png");
    const float size = 16.0f;
    
    Rectangle getIconRecSource(int id) {
        return { ((float)id * size) , size * 13, size, size };
    };

    Rectangle getTileTexture(int blockType, std::string emptySides) {
        int blockWrap;
        if (emptySides == "t") blockWrap = 0;
        else if (emptySides == "c") blockWrap = 1;
        else if (emptySides == "r") blockWrap = 2;
        else if (emptySides == "l") blockWrap = 3;
        else if (emptySides == "b") blockWrap = 4;
        else if (emptySides == "tl") blockWrap = 5;
        else if (emptySides == "tr") blockWrap = 6;
        else if (emptySides == "br") blockWrap = 7;
        else if (emptySides == "bl") blockWrap = 8;
        else if (emptySides == "ltr") blockWrap = 9;
        else if (emptySides == "trb") blockWrap = 10;
        else if (emptySides == "rbl") blockWrap = 11;
        else if (emptySides == "blt") blockWrap = 12;
        else if (emptySides == "a") blockWrap = 13;
        else if (emptySides == "lr") blockWrap = 14;
        else if (emptySides == "tb") blockWrap = 15;

        return { blockType * size, blockWrap * size, size, size};
    };

};

#endif
