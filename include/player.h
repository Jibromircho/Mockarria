#ifndef PLAYER_H
#define PLAYER_H

#include <config.h>

class Player
{
public:
    
    Texture2D model_move = LoadTexture("../img/player/Unarmed_Run_full.png");
    Texture2D model_idle = LoadTexture("../img/player/Unarmed_Idle_full.png");
    Vector2 position = { 400.0f, 280.0f };
    Rectangle frameRecMove = { 0.0f, 0.0f, (float)model_move.width/8, (float)model_move.height/2 };
    Rectangle frameRecIdle = { 0.0f, 0.0f, (float)model_idle.width/12, (float)model_idle.height/3 };

private:

};

#endif
