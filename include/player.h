#ifndef PLAYER_H
#define PLAYER_H

#include <config.h>

typedef enum PlayerState { GROUND = 0, JUMPING, FLYGHT } PlayerState;
class Player
{
public:
    Vector2 position = { 400.0f, 280.0f };
    Texture2D model_movement = LoadTexture("../img/player/Unarmed_Movement_full.png");
    Rectangle frameRecMove = { 0.0f, 0.0f, (float)model_movement.width/8, (float)model_movement.height/3 };
    Rectangle frameRecIdle = { 0.0f, 0.0f, (float)model_movement.width/8, (float)model_movement.height/3 };
    const int maxJump = 1;
    int jumpCount = maxJump;
    float movementSpeed = 3.0f;
    PlayerState state = GROUND;
    Rectangle hitbox = { position.x + 20, position.y + 10, frameRecIdle.width - 40, frameRecIdle.height - 25};


    

private:


};

#endif
