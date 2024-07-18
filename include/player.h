#ifndef PLAYER_H
#define PLAYER_H

#include <config.h>

typedef enum PlayerState { GROUND = 0, JUMPING, FLYGHT } PlayerState;
class Player
{
public:
    //player assets and simialr stuff
    Vector2 position = { 0.0f, 0.0f };
    Texture2D model_movement = LoadTexture("../img/player/Unarmed_Movement_full.png");
    Rectangle frameRecMove = { 0.0f, 0.0f, (float)model_movement.width/8, (float)model_movement.height/3 };
    Rectangle frameRecIdle = { 0.0f, 0.0f, (float)model_movement.width/8, (float)model_movement.height/3 };
    PlayerState state = GROUND;
    Rectangle hitbox = { position.x + 22, position.y + 10, frameRecIdle.width - 43, frameRecIdle.height - 25};

    //player stats
    const int maxJump = 100;
    int jumpCount = maxJump;
    float jumpStrength = -3.5f;
    int health = 100;
    float movementSpeed = 1.0f;


    

private:


};

#endif
