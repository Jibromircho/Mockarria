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
    Rectangle hitbox = { position.x, position.y, frameRecIdle.width, frameRecIdle.height };

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
            hitbox.x = position.x;
            hitbox.y = position.y;
            TraceLog(LOG_INFO, "Game loaded successfully.");
            return true;
        } else {
            TraceLog(LOG_ERROR, "Failed to open file for loading.");
            return false;
        }
    }


    

private:


};

#endif
