#ifndef PLAYER_H
#define PLAYER_H

#include <config.h>

typedef enum PlayerState { GROUND = 0, JUMPING, FLYGHT } PlayerState;

struct PlayerStats { 
    int strength = 1;
    int dexterity = 1;
    int intellect = 1;
    int vigor = 1;
    int wisdom = 1;
    int spirit = 1;
    int luck = 1;
    float criticalChance = 1.0f + luck * 0.02f;
    float healthRegeneration = spirit * 0.4f;
    float manaRegeneration = spirit * 0.1f;
} ;


class Player
{
public:
    //player assets and simialr stuff
    Vector2 position = { 0.0f, 0.0f };
    Texture2D model_movement = LoadTexture("../img/player/Unarmed_Movement_full.png");
    Rectangle frameRecMove = { 0.0f, 0.0f, (float)model_movement.width/8, (float)model_movement.height/3 };
    Rectangle frameRecIdle = { 0.0f, 0.0f, (float)model_movement.width/8, (float)model_movement.height/3 };
    PlayerState state = GROUND;
    Vector2 hitboxOffset = { 4 , 4 };
    Rectangle hitbox = { position.x + hitboxOffset.x, position.y + hitboxOffset.y, frameRecIdle.width - 6, frameRecIdle.height - 5 };

    //player stats
    PlayerStats stats;
    const int maxJump = 100;
    int jumpCount = maxJump;
    float jumpStrength = -3.5f;
    int health = 50 + level * 5 + stats.vigor * 10;
    int mana = 10 + level * 2 + stats.wisdom * 5;
    int level = 1;
    float experience = 0.0f;

    float movementSpeed = 2.0f;

    void updatePlayer() //player update functions
    {   
        //input movement
        if (IsKeyDown(KEY_LEFT_SHIFT)||IsKeyDown(KEY_RIGHT_SHIFT)) movementSpeed = 6.0f;
        if (IsKeyReleased(KEY_LEFT_SHIFT)||IsKeyReleased(KEY_RIGHT_SHIFT)) movementSpeed = 2.0f;
        if (IsKeyDown(KEY_A)) position.x -= movementSpeed;
        if (IsKeyDown(KEY_D)) position.x += movementSpeed;

        //always updating 
        hitbox.x = position.x + hitboxOffset.x;
    }

    void drawPlayer() const {
        if(IsKeyDown(KEY_A)) DrawTextureRec(model_movement, frameRecMove, position, WHITE);
        else if(IsKeyDown(KEY_D)) DrawTextureRec(model_movement, frameRecMove, position, WHITE);
        else if(IsKeyUp(KEY_A && KEY_D)) DrawTextureRec(model_movement, frameRecIdle, position, WHITE);
    }

    //player saving function
    void saveGame (Vector2& position, const std::string& filename){
        std::ofstream file(filename, std::ios::binary);
        if(file.is_open()){
            file.write(reinterpret_cast<const char*>(&position), sizeof(Vector2));
            file.close();
            TraceLog(LOG_INFO, "Game saved successfully.");
        } else {
            TraceLog(LOG_ERROR, "Failed to open file for saving.");
        }
    }

    //player load function 
    bool loadGame(Vector2& position, const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (file.is_open()) {
            file.read(reinterpret_cast<char*>(&position), sizeof(Vector2));
            file.close();
            hitbox.x = position.x + hitboxOffset.x;
            hitbox.y = position.y + hitboxOffset.y;
            TraceLog(LOG_INFO, "Game loaded successfully.");
            return true;
        } else {
            TraceLog(LOG_ERROR, "Failed to open file for loading.");
            return false;
        }
    }
    //player position fucntions
    void resetPos(){
        position = { 0.0f, 0.0f };
        hitbox = { position.x + hitboxOffset.x, position.y + hitboxOffset.y, frameRecIdle.width - 6, frameRecIdle.height - 5 };
    }
    void resetJump(){
        jumpCount = maxJump;
    }
    

private:


};

#endif
