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
    Vector2 hitboxOffset = { 3 , 4 };
    Vector2 centerOffset = { 12.0f , 21.0f };
    Rectangle hitbox = { position.x + hitboxOffset.x, position.y + hitboxOffset.y, frameRecIdle.width - 5, frameRecIdle.height - 5 };
    Vector2 center = { position.x + centerOffset.x, position.y + centerOffset.y };

    Texture2D healthUi = LoadTexture("../img/ui/Hearts.png");
    Rectangle healthUi20 = { 0.0f, 0.0f, (float)healthUi.width/5, 0.0f };
    Rectangle healthUi15 = { 0.0f, 0.0f, (float)healthUi.width/5, 0.0f };
    Rectangle healthUi10 = { 0.0f, 0.0f, (float)healthUi.width/5, 0.0f };
    Rectangle healthUi5 = { 0.0f, 0.0f, (float)healthUi.width/5, 0.0f };
    Rectangle healthUi0 = { 0.0f, 0.0f, (float)healthUi.width/5, 0.0f };

    //player stats
    PlayerStats stats;
    int maxJump = 100;
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
        center = { position.x + centerOffset.x, position.y + centerOffset.y };
    }

    void drawPlayer() const {
        if(IsKeyDown(KEY_A)) DrawTextureRec(model_movement, frameRecMove, position, WHITE);
        else if(IsKeyDown(KEY_D)) DrawTextureRec(model_movement, frameRecMove, position, WHITE);
        else if(IsKeyUp(KEY_A && KEY_D)) DrawTextureRec(model_movement, frameRecIdle, position, WHITE);
    }
    
    void drawPlayerHealth(float resolutionScale) const {
        Vector2 position = { 16.0f, 8.0f };
        float scaleFactor = resolutionScale / 1.6;
        Rectangle destRec = { position.x, position.y, healthUi20.width * scaleFactor, healthUi20.height * scaleFactor };
        if ((health % 20) > 0){
            DrawTexturePro(healthUi,healthUi20, (Rectangle{ 16.0f, 8.0f , 0.0f, 0.0f }),Vector2{ 0.0f, 0.0f }, resolutionScale / 1.6, WHITE);
        }
    }

    //player saving function
    void saveGame(const std::string& filename, const Inventory& inventory) const {
        std::ofstream file(filename, std::ios::binary);
        if (file.is_open()) {
            // Save Player data
            file.write(reinterpret_cast<const char*>(&position), sizeof(Vector2));
            file.write(reinterpret_cast<const char*>(&health), sizeof(health));
            file.write(reinterpret_cast<const char*>(&mana), sizeof(mana));
            file.write(reinterpret_cast<const char*>(&level), sizeof(level));
            file.write(reinterpret_cast<const char*>(&experience), sizeof(experience));
            file.write(reinterpret_cast<const char*>(&jumpCount), sizeof(jumpCount));
            file.write(reinterpret_cast<const char*>(&stats), sizeof(PlayerStats));

            // Save Inventory data
            file.write(reinterpret_cast<const char*>(&inventory.hotbarIndex), sizeof(inventory.hotbarIndex));
            for (int i = 0; i < inventory.hotbarSize; ++i) {
                for (int j = 0; j < inventory.inventoryRows; ++j) {
                    file.write(reinterpret_cast<const char*>(&inventory.slots[i][j]), sizeof(Slot));
                }
            }

            file.close();
            TraceLog(LOG_INFO, "Game saved successfully.");
        } else {
            TraceLog(LOG_ERROR, "Failed to open file for saving.");
        }
    }

    //player load function 
    bool loadGame(const std::string& filename, Inventory& inventory) {
        std::ifstream file(filename, std::ios::binary);
        if (file.is_open()) {
            // Load Player data
            file.read(reinterpret_cast<char*>(&position), sizeof(Vector2));
            file.read(reinterpret_cast<char*>(&health), sizeof(health));
            file.read(reinterpret_cast<char*>(&mana), sizeof(mana));
            file.read(reinterpret_cast<char*>(&level), sizeof(level));
            file.read(reinterpret_cast<char*>(&experience), sizeof(experience));
            file.read(reinterpret_cast<char*>(&jumpCount), sizeof(jumpCount));
            file.read(reinterpret_cast<char*>(&stats), sizeof(PlayerStats));

            hitbox.x = position.x + hitboxOffset.x;
            hitbox.y = position.y + hitboxOffset.y;
            center = { position.x + centerOffset.x, position.y + centerOffset.y };

            // Load Inventory data
            file.read(reinterpret_cast<char*>(&inventory.hotbarIndex), sizeof(inventory.hotbarIndex));
            for (int i = 0; i < inventory.hotbarSize; ++i) {
                for (int j = 0; j < inventory.inventoryRows; ++j) {
                    file.read(reinterpret_cast<char*>(&inventory.slots[i][j]), sizeof(Slot));
                }
            }

            file.close();
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
