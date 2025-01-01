#include <config.h>

///////////////////////////////////
///////////////////////////////////some global varuables we will need

std::vector<int> p;

//game current screen
typedef enum GameScreen { LOGO = 0, TITLE, PLAY, SETTINGS, GAMEPLAY, ENDING} GameScreen;


const int worldSizeW = 6400;
const int worldSizeH = 1800;
const float worldStartX = (float)worldSizeW * 16 / 2 * - 1;
const float worldStartY = (float) worldSizeH * 16 / 5 * -1;
typedef struct Block {
    bool solid = false;
    bool directional = false;
    Vector2 position;
    Rectangle hitbox;
    int health;
    enum Type { SKY = -1, DIRT = 0, STONE = 1, CLAY = 2, MUD = 3, SNOW = 4, SAND = 5, } type;
    // Constructor with default values
    Block() : solid(false), directional(false), position({ worldStartX, worldStartY}), hitbox({worldStartX, worldStartY, 16,16}), health(300), type(SKY){}
} Block;

Block block[worldSizeW][worldSizeH];

// screen constants
const float workingWidth = 1980.0f;
const float workingHeight = 1080.0f;
constexpr int buttonFontSize = 26;
bool mapOpened = false;
constexpr int fps = 60;

//audio volumes
float volumeMaster = 0.8f;
float volumeMusic = 0.8f;
float volumeSoundFX = 0.8f;
float volumeAmbient = 0.8f;

//fucntion definitions
void initPermutation(unsigned int seed = std::time(nullptr));
double fade(double t);
double lerpgpt(double t, double a, double b);
double grad1D(int hash, double x);
double grad2D(int hash, double x, double y);
double perlin1D(double x);
double perlin2D(double x, double y);
void generateNewWorldMap(Tile* tile,Player* player,Inventory* inventory);
void drawMenuBackground(Texture2D* loadScreenSky, Vector2* mainMenuCloudPos, Texture2D* loadScreenCloud_1, Texture2D* loadScreen, float resolutionScale);

////debug stuff
void saveMapAsImage(const Block block[worldSizeW][worldSizeH], const char* filename);

void saveWorld(const Block block[worldSizeW][worldSizeH], const std::string& filename);
void loadWorld(Block block[worldSizeW][worldSizeH], const std::string& filename);

Color getColorForBlockType(Block::Type type) {
    switch (type) {
        case Block::SKY: return SKYBLUE;
        case Block::DIRT: return BROWN;
        case Block::STONE: return GRAY;
        case Block::CLAY: return RED;
        case Block::MUD: return LIGHTGRAY;
        case Block::SAND: return YELLOW;
        case Block::SNOW: return { 224, 247, 250, 255 };
        default: return BLACK;
    }
}

int main() {
    // initialize win
    InitWindow(0, 0, "Mockarria");
    float currentResWidth = (float)GetScreenWidth();
    float currentResHeight = (float)GetScreenHeight();
    float resolutionScale = 1.0f;
    
    SetWindowSize(currentResWidth, currentResHeight);
    ToggleFullscreen();
    
    SetExitKey(0);

    //button positions
    const Vector2 playButtonPos { currentResWidth / 2 - 100, currentResHeight / 2 - 100 };
    const Vector2 settingsButtonPos { currentResWidth / 2 - 100, currentResHeight / 2  - 25 };
    const Vector2 exitButtonPos { currentResWidth / 2 - 100, currentResHeight / 2  + 50 };
    const Vector2 backButtonPos { 50 , currentResHeight - 100};

    const Vector2 fullscreenButtonPos { currentResWidth / 2 - 100, currentResHeight / 2 - 100 };
    const Vector2 soundButtonPos { currentResWidth / 2 - 100, currentResHeight / 2  - 25 };

    const Vector2 createWorldButtonPos = { currentResWidth / 2 - 300, currentResHeight - 100 };
    const Vector2 loadWorldButtonPos = { currentResWidth / 2 , currentResHeight - 100 };

    //game elements 
    GameScreen currentScreen = LOGO;
    Player player;
    Inventory inventory;
    World world;
    Tile tile;
    std::vector<Item> existingItems;
    //camera initialization
    Camera2D camera = { 0 };
    camera.target = {player.position.x , player.position.y };
    camera.offset = {currentResWidth / 2.0f, currentResHeight / 2.0f};
    camera.zoom = 1.4f;
    Rectangle scissorArea = { player.position.x, player.position.y, currentResWidth,currentResHeight};
   
    //initialize audio stuff
    InitAudioDevice();
    Music mainMenuMusic1 = LoadMusicStream("../sfx/Menu_soundtrack_1.mp3");
    mainMenuMusic1.looping = true;
    SetMasterVolume(volumeMaster);
    SetMusicVolume(mainMenuMusic1, volumeMusic);
    PlayMusicStream(mainMenuMusic1);

    //initialize textures
    Texture2D inventorySlot = LoadTexture("../img/ui/Inventory_Slot.png");
    Texture2D loadScreen = LoadTexture("../img/backgrounds/Initial_load_screen_no_sky.png");
    Texture2D loadScreenSky = LoadTexture("../img/backgrounds/main_menu_sky.png");
    Texture2D loadScreenCloud_1 = LoadTexture("../img/backgrounds/main_menu_cloud_1.png");
    Texture2D buttonsEmpty = LoadTexture("../img/ui/ui_buttons_x2.png");


    Rectangle buttonNonPressed = { 0.0f, 0.0f, (float)buttonsEmpty.width / 2, (float)buttonsEmpty.height / 2 };
    Rectangle buttonHover = { 0.0f, (float)buttonsEmpty.height / 2, (float)buttonsEmpty.width / 2, (float)buttonsEmpty.height / 2 };
    Rectangle buttonPressed = { (float)buttonsEmpty.width / 2, (float)buttonsEmpty.height / 2, (float)buttonsEmpty.width / 2, (float)buttonsEmpty.height / 2 };
    Vector2 mainMenuCloudPos = Vector2{ 0.0f, 0.0f};

    //button hitboxes
    const Rectangle playButtonHitbox { playButtonPos.x , playButtonPos.y , (float)buttonsEmpty.width / 2, (float)buttonsEmpty.height / 2 };
    const Rectangle settingsButtonHitbox { settingsButtonPos.x , settingsButtonPos.y , (float)buttonsEmpty.width / 2, (float)buttonsEmpty.height / 2 };
    const Rectangle exitButtonHitbox { exitButtonPos.x , exitButtonPos.y , (float)buttonsEmpty.width / 2, (float)buttonsEmpty.height / 2 };
    const Rectangle backButtonHitbox { backButtonPos.x , backButtonPos.y , (float)buttonsEmpty.width / 2, (float)buttonsEmpty.height / 2 };

    const Rectangle fullscreenButtonHitbox { fullscreenButtonPos.x, fullscreenButtonPos.y, (float)buttonsEmpty.width / 2, (float)buttonsEmpty.height / 2 };
    const Rectangle soundButtonHitbox { soundButtonPos.x, soundButtonPos.y, (float)buttonsEmpty.width / 2, (float)buttonsEmpty.height / 2 };

    const Rectangle createWorldButtonHitbox { createWorldButtonPos.x , createWorldButtonPos.y, (float)buttonsEmpty.width / 2, (float)buttonsEmpty.height / 2 };
    const Rectangle loadWorldButtonHitbox { loadWorldButtonPos.x , loadWorldButtonPos.y, (float)buttonsEmpty.width / 2, (float)buttonsEmpty.height / 2 };

    //frame related stuff
    int frameCounter = 0;
    SetTargetFPS(fps); // set target fps

    int currentFrameMove = 0;
    int currentFrameIdle = 0;

    int framesCounter = 0;
    int framesSpeed = 8;


    // Main game loop
    while (!WindowShouldClose()) // detect window closure
    {
        //Constantly updating stuff
        framesCounter++;
        Vector2 mousePosition = GetMousePosition();
        Vector2 playerCenterPos = player.center;
        Vector2 worldMousePosition = GetScreenToWorld2D(mousePosition, camera);
        float mouse_playerDistance = Vector2Distance(playerCenterPos, worldMousePosition);
        Rectangle mouseHitbox = { mousePosition.x, mousePosition.y, 5.0f, 5.0f };
        Rectangle worldMouseHitbox = { worldMousePosition.x, worldMousePosition.y, 0.0f, 0.0f };


        int firstBlockX = (worldSizeW / 2) + (player.position.x - currentResWidth / 2) / 16;
        int lastBlockX = (worldSizeW / 2) + (player.position.x + currentResWidth / 2) / 16;
        int firstBlockY = (worldSizeH / 5) + (player.position.y - currentResHeight / 2) / 16;
        int lastBlockY = (worldSizeH / 5) + (player.position.y + currentResHeight / 2) / 16;
        
   
        UpdateMusicStream(mainMenuMusic1);

        if (mainMenuCloudPos.x >= 0) {
            mainMenuCloudPos.x += 0.05f;
        }
        else mainMenuCloudPos.x = 0.0f;

        if(currentScreen == GAMEPLAY) 
        {

            if (framesCounter >= (fps/framesSpeed))
            {
                framesCounter = 0;
            
                currentFrameMove++;
                currentFrameIdle++;

                if(player.state == JUMPING)
                {
                    currentFrameMove = 2;
                    currentFrameIdle = 0;
                }
            
                if (currentFrameMove > 7) currentFrameMove = 0;
                if (currentFrameIdle > 7) currentFrameIdle = 0;
    

                player.frameRecMove.x = (float)currentFrameMove*(float)player.model_movement.width/8;
                player.frameRecIdle.x = (float)currentFrameIdle*(float)player.model_movement.width/8;
                player.frameRecIdle.y = (player.model_movement.height/3)*2; 
                if(IsKeyDown(KEY_A)) currentFrameIdle = 0, player.frameRecMove.y = (player.model_movement.height/3)*3;
                if(IsKeyDown(KEY_D)) currentFrameIdle = 0, player.frameRecMove.y = player.model_movement.height/3;
            }
            //player actions inputs
            player.updatePlayer(&world);

            world.updateWorld();

            //tile hitbox checks
            for (int i = firstBlockX; i < lastBlockX; i++){
                for (int j = firstBlockY; j < lastBlockY; j++){
                    bool isMouseOnBlock = CheckCollisionRecs(worldMouseHitbox, block[i][j].hitbox);
                    if (isMouseOnBlock && mouse_playerDistance <= 100){
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                            if (block[i][j].health > 0) {
                                block[i][j].health -= 100;
                            } else if (block[i][j].health == 0 && block[i][j].type != Block::SKY) { //////// for loop to be removed once done testing
                            //for(int g = 0; g < 11;g++){
                                Item item(block[i][j].type , block[i][j].position,1);
                                existingItems.push_back(item);
                            //}                 
                                block[i][j].type = Block::SKY;
                                block[i][j].solid = false;
                                block[i][j].directional = false;
                            }
                        }
                    }
                    if (isMouseOnBlock && !CheckCollisionRecs(block[i][j].hitbox,player.hitbox)) {
                        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                            if (block[i][j].type == -1 && mouse_playerDistance <= 100) {           
                                block[i][j].type = static_cast<Block::Type>(inventory.slots[inventory.hotbarIndex][0].item.id);
                                block[i][j].solid = true;
                                block[i][j].directional = true;
                                inventory.removeItem(inventory.slots[inventory.hotbarIndex][0].item.id, 1);                                             
                            }
                        }
                    }
                    

                    if(block[i][j].solid){
                        Rectangle collisionArea = GetCollisionRec(player.hitbox, block[i][j].hitbox);
                        if (collisionArea.width > collisionArea.height){
                            if (collisionArea.y == player.hitbox.y){
                                player.position.y += collisionArea.height;
                                player.hitbox.y += collisionArea.height;
                                world.velocity += 1;
                            }else player.position.y -= collisionArea.height, player.hitbox.y -= collisionArea.height, world.velocity = 0, player.state = GROUND, player.resetJump();
                        }
                        if (collisionArea.height > collisionArea.width){
                            if (collisionArea.x == player.hitbox.x) {
                                player.position.x += collisionArea.width;
                                player.hitbox.x += collisionArea.width;
                            }else player.position.x -= collisionArea.width, player.hitbox.x -= collisionArea.width;
                        }
                    }
                }
            }
            
            std::vector<Item> updatedItems;

            std::vector<bool> checked (existingItems.size(), false);

            for (size_t i = 0; i < existingItems.size(); ++i) {
                for (size_t j = i + 1; j < existingItems.size(); ++j) {
                    if (existingItems[i].id == existingItems[j].id) {
                        if (CheckCollisionRecs(existingItems[i].hitbox, existingItems[j].hitbox)) {
                            if (existingItems[i].stackSize + existingItems[j].stackSize <= existingItems[i].maxStackSize) {
                                existingItems[i].stackSize += existingItems[j].stackSize;
                                existingItems.erase(existingItems.begin() + j);
                                --j;
                            }
                        }
                    }
                }
            }

            for (size_t i = 0; i < existingItems.size(); i++) {
                if (CheckCollisionRecs(player.pickupHitbox, existingItems[i].hitbox)) {
                    Vector2 itemMagnetism = { existingItems[i].position.x - player.center.x, existingItems[i].position.y - player.center.y };
                    existingItems[i].position.x -= itemMagnetism.x * 0.025;
                    existingItems[i].position.y -= itemMagnetism.y * 0.025;
                    existingItems[i].hitbox.x = existingItems[i].position.x;
                    existingItems[i].hitbox.y = existingItems[i].position.y;
                    if (CheckCollisionRecs(player.hitbox, existingItems[i].hitbox)) {
                        if (inventory.pickUpItem(existingItems[i].id, &existingItems[i].stackSize)) {
                            existingItems[i].place = ItemPlace::INVENTORY;
                            continue;
                        }
                    }
                }
                updatedItems.push_back(existingItems[i]);
            }
            existingItems = std::move(updatedItems);

        //other keys and inputs
        if (IsKeyDown(KEY_ESCAPE)) {
            currentScreen = TITLE;
            player.saveGame("../save/playerSave.dat", inventory);
            saveWorld(block, "../save/worldSave.dat");
            saveMapAsImage(block, "../save/world_map_1.png");
        }
        //hotbar slot select
        for (int i = 0; i < 10; i++) {
            if (IsKeyPressed('0' + ((i + 1) % 10))) inventory.hotbarIndex = i;
        }
        //open invetory
        if (IsKeyPressed(KEY_B)) {
            if(inventory.inventoryOpened) inventory.inventoryOpened = false;
            else inventory.inventoryOpened = true;
        }
        //open world map
        if (IsKeyPressed(KEY_M)) {
            if (mapOpened) mapOpened = false;
            else mapOpened = true;
        }
        // camera follows the player
        camera.target = {player.position.x, player.position.y };
        }

        // screen selector
        if (currentScreen == LOGO)
        {
            frameCounter++;
            if (frameCounter > 40)
            {
                currentScreen = TITLE;
            }
        }

        // Draw

        BeginDrawing();

            ClearBackground(RAYWHITE);
            switch (currentScreen)
            {
            case LOGO:
                drawMenuBackground(&loadScreenSky, &mainMenuCloudPos, &loadScreenCloud_1, &loadScreen,resolutionScale);
                break;
            case TITLE:
            {
                //draw title screen elements
                drawMenuBackground(&loadScreenSky, &mainMenuCloudPos, &loadScreenCloud_1, &loadScreen,resolutionScale);
                DrawText("MOCKARRIA by Didi", 20, 20, 36, LIGHTGRAY);

                //draw all the buttons
                if (!CheckCollisionRecs(mouseHitbox, playButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonNonPressed, playButtonPos, WHITE);
                    }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseHitbox, playButtonHitbox)){
                        DrawTextureRec(buttonsEmpty, buttonPressed, playButtonPos, WHITE);
                        currentScreen = PLAY;
                }
                else DrawTextureRec(buttonsEmpty, buttonHover, playButtonPos, WHITE);

                if (!CheckCollisionRecs(mouseHitbox, settingsButtonHitbox)){

                    DrawTextureRec(buttonsEmpty, buttonNonPressed, settingsButtonPos, WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseHitbox, settingsButtonHitbox)){
                        DrawTextureRec(buttonsEmpty, buttonPressed, settingsButtonPos, WHITE);
                        currentScreen = SETTINGS;
                }
                else DrawTextureRec(buttonsEmpty, buttonHover, settingsButtonPos, WHITE);

                if (!CheckCollisionRecs(mouseHitbox, exitButtonHitbox)){

                    DrawTextureRec(buttonsEmpty, buttonNonPressed, exitButtonPos, WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseHitbox, exitButtonHitbox)){
                        DrawTextureRec(buttonsEmpty, buttonPressed, exitButtonPos, WHITE);
                        currentScreen = ENDING;
                }
                else DrawTextureRec(buttonsEmpty, buttonHover, exitButtonPos, WHITE);

                //draw button text
                DrawText("PLAY", playButtonPos.x + 20 , playButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);
                DrawText("SETTINGS", settingsButtonPos.x + 20 , settingsButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);
                DrawText("EXIT", exitButtonPos.x + 20 , exitButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);


            }break;
            case PLAY:
            {
                //draw background
                drawMenuBackground(&loadScreenSky, &mainMenuCloudPos, &loadScreenCloud_1, &loadScreen, resolutionScale);
                DrawText("MOCKARRIA by Didi", 20, 20, 36, LIGHTGRAY);

                //draw all buttons
                if (!CheckCollisionRecs(mouseHitbox, backButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonNonPressed, backButtonPos, WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseHitbox, backButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonPressed, backButtonPos, WHITE);
                    currentScreen = TITLE;
                }
                else DrawTextureRec(buttonsEmpty, buttonHover, backButtonPos, WHITE);

                if (!CheckCollisionRecs(mouseHitbox, createWorldButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonNonPressed, createWorldButtonPos, WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseHitbox, createWorldButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonPressed, createWorldButtonPos, WHITE);
                    player.resetPos();
                    currentScreen = GAMEPLAY;
                    //create a map
                    generateNewWorldMap(&tile, &player, &inventory);
                    
                saveMapAsImage(block, "../save/world_map_1.png");
                }
    
                else DrawTextureRec(buttonsEmpty, buttonHover, createWorldButtonPos, WHITE);
                
                if (!CheckCollisionRecs(mouseHitbox, loadWorldButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonNonPressed, loadWorldButtonPos, WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseHitbox, loadWorldButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonPressed, loadWorldButtonPos, WHITE);
                    player.loadGame("../save/playerSave.dat", inventory);
                    loadWorld(block, "../save/worldSave.dat");
                    currentScreen = GAMEPLAY;
                }
                else DrawTextureRec(buttonsEmpty, buttonHover, loadWorldButtonPos, WHITE);

                DrawText("BACK", backButtonPos.x + 20 , backButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);
                DrawText("NEW WORLD", createWorldButtonPos.x + 20 , createWorldButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);
                DrawText("LOAD WORLD", loadWorldButtonPos.x + 20, loadWorldButtonPos.y + buttonFontSize / 2, buttonFontSize, RAYWHITE);
            }break;
            case SETTINGS:
            {
                //draw background
                drawMenuBackground(&loadScreenSky, &mainMenuCloudPos, &loadScreenCloud_1, &loadScreen, resolutionScale);
                DrawText("MOCKARRIA by Didi", 20, 20, 36, LIGHTGRAY);

                //draw all buttons

                //back button
                if (!CheckCollisionRecs(mouseHitbox, backButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonNonPressed, backButtonPos, WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseHitbox, backButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonPressed, backButtonPos, WHITE);
                    currentScreen = TITLE;
                }
                else DrawTextureRec(buttonsEmpty, buttonHover, backButtonPos, WHITE);
                DrawText("BACK", backButtonPos.x + 20 , backButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);

                //fulscreen button
                if (!CheckCollisionRecs(mouseHitbox, fullscreenButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonNonPressed, fullscreenButtonPos, WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseHitbox, fullscreenButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonPressed, fullscreenButtonPos, WHITE);
                    ToggleFullscreen();

                }
                else DrawTextureRec(buttonsEmpty, buttonHover, fullscreenButtonPos, WHITE);
                if (IsWindowFullscreen()) DrawText("FULLSCREEN : OFF", fullscreenButtonPos.x + 20 , fullscreenButtonPos.y + buttonFontSize / 1.4 , buttonFontSize / 1.4, RAYWHITE);
                if (!IsWindowFullscreen()) DrawText("FULLSCREEN : ON", fullscreenButtonPos.x + 20 , fullscreenButtonPos.y + buttonFontSize / 1.4 , buttonFontSize / 1.4, RAYWHITE);

                //sound on/off button
                if (!CheckCollisionRecs(mouseHitbox, soundButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonNonPressed, soundButtonPos, WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseHitbox, soundButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonPressed, soundButtonPos, WHITE);
                    if (GetMasterVolume() == 0.0f) {
                        SetMasterVolume(volumeMaster);
                    }
                    else {
                        SetMasterVolume(0.0f);
                    }

                }
                else DrawTextureRec(buttonsEmpty, buttonHover, soundButtonPos, WHITE);
                if (GetMasterVolume() > 0) DrawText("SOUND : ON", soundButtonPos.x + 20, soundButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);
                if (GetMasterVolume() == 0) DrawText("SOUND : OFF", soundButtonPos.x + 20, soundButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);



            }break;
            case GAMEPLAY:
            {
                BeginMode2D(camera);
                    for (int i = firstBlockX; i < lastBlockX; i++){
                        for (int j = firstBlockY; j < lastBlockY; j++){
                            std::string blockWrap;
                            if (!block[i][j + 1].solid && !block[i][j - 1].solid && !block[i - 1][j].solid && !block[i + 1][j].solid) blockWrap = "a" ;
                            else if (!block[i][j - 1].solid && !block[i - 1][j].solid && !block[i + 1][j].solid) blockWrap = "ltr" ;
                            else if (!block[i][j + 1].solid && !block[i - 1][j].solid && !block[i + 1][j].solid) blockWrap = "rbl" ;
                            else if (!block[i][j + 1].solid && !block[i][j - 1].solid && !block[i + 1][j].solid) blockWrap = "trb" ;
                            else if (!block[i][j + 1].solid && !block[i][j - 1].solid && !block[i - 1][j].solid) blockWrap = "blt" ;
                            else if (!block[i][j + 1].solid && !block[i - 1][j].solid) blockWrap = "bl" ;
                            else if (!block[i][j - 1].solid && !block[i - 1][j].solid) blockWrap = "tl" ;
                            else if (!block[i + 1][j].solid && !block[i][j - 1].solid) blockWrap = "tr" ;
                            else if (!block[i][j + 1].solid && !block[i + 1][j].solid) blockWrap = "br" ;
                            else if (!block[i][j + 1].solid && !block[i][j - 1].solid) blockWrap = "lr" ;
                            else if (!block[i - 1][j].solid && !block[i + 1][j].solid) blockWrap = "tb" ;              
                            else if (!block[i + 1][j].solid) blockWrap = "r" ;
                            else if (!block[i - 1][j].solid) blockWrap = "l" ;
                            else if (!block[i][j + 1].solid) blockWrap = "b" ;
                            else if (!block[i][j - 1].solid) blockWrap = "t" ;
                            else blockWrap = "c";
                            Rectangle sourceRec = tile.getTileTexture(block[i][j].type, blockWrap);

                            DrawTextureRec(tile.tileSet, sourceRec, block[i][j].position, WHITE);

                        }
                    }
                    DrawRectangleLinesEx(player.pickupHitbox, 2, GREEN);
                    //player drawing
                    player.drawPlayer();
                    
                        int itemsOnGround = 0;
                    for (Item& item : existingItems) {
                        itemsOnGround++;
                        if (item.place == ItemPlace::GROUND) {
                            Rectangle itemRecSource = tile.getIconRecSource(item.id);

                            Rectangle itemDest = {
                                item.position.x,
                                item.position.y,
                                itemRecSource.height * 0.8f,
                                itemRecSource.width * 0.8f,
                            };

                            DrawTexturePro(tile.tileSet,itemRecSource,itemDest,{ 0 ,0 }, 0.0f, WHITE);
                        }
                    }
                    

                EndMode2D();

                //Drawing ui elemnts
                //player.drawPlayerHealth(resolutionScale);
                float selectedSlotScale;
                for (int i = 0; i < 10; i++) {
                    Vector2 position = { (currentResWidth / 3) + (i * 48.0f) + 48.0f, 16.0f };
                    Color tint;

                    if (i == inventory.hotbarIndex) {
                        tint = { 211, 211, 150, 220 };
                        selectedSlotScale = 0.25f;
                    } else {
                        tint = { 255, 255, 255, 220 };
                        selectedSlotScale = 0.0f;
                    }
                    DrawTextureEx(inventorySlot, position, 0.0f, 2.0f + selectedSlotScale, tint);

                    inventory.drawHotbarItems(&tile,{ (currentResWidth / 3) + 48.0f, 16.0f });
                }
                if (inventory.inventoryOpened) {
                    for (int i = 0; i < inventory.hotbarSize; i++) {
                        for (int j = 1; j < inventory.inventoryRows; j++) {
                            Vector2 position = { (currentResWidth / 3) + (i * 48.0f) + 48.0f, (j * 48.0f) + 16.0f };
                            Color tint = { 255, 255, 255, 220 };
                            DrawTextureEx(inventorySlot, position, 0.0f, 2.25f , tint);
                            inventory.drawInventoryItems(&tile, { (currentResWidth / 3) + 48.0f, 16.0f });
                        }
                    }
                }

                            //////// for stress testing ho many items i can have on the ground /// tested for up to 1000 stable
                            Font font = GetFontDefault();
                            DrawTextEx( font, (std::to_string(itemsOnGround).c_str()), {350, 200},22, 2,BLACK);
                            ///////
                DrawFPS ( 200, 200 );

            }break;
            case ENDING:
            {
                CloseAudioDevice();
                CloseWindow();
                return 0;
            }
            
            default:
                break;
            }
        
        EndDrawing();
    }

    // De-initialize
    UnloadMusicStream(mainMenuMusic1);
    CloseAudioDevice();

    CloseWindow();  //Close window and OpenGL context

    return 0;
    
};

//////////////////
void initPermutation(unsigned int seed){

    p.resize(256);
    std::iota(p.begin(), p.end(), 0);

    std::default_random_engine engine(seed);

    std::shuffle(p.begin(), p.end(), engine);

    p.insert(p.end(), p.begin(), p.end());
}
//////////////////
double fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}
///////////////
double lerpgpt(double t, double a, double b) {
    return a + t * (b - a);
}
//////////////////
double grad1D(int hash, double x) {
    int h = hash & 15;
    double grad = 1 + (h & 7);
    grad = (h & 8) ? -grad : grad;
    grad = grad / 8.0;
    return grad * x;
}
/////////////////
double grad2D(int hash, double x, double y) {
    int h = hash & 7; 
    double u = h < 4 ? x : y; 
    double v = h < 4 ? y : x; 
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}
////////////////
double perlin1D(double x) {
    int X = (int)floor(x) & 255; 
    x -= floor(x);      
    double u = fade(x);  

    int a = p[X];
    int b = p[X + 1];

    return lerpgpt(u, grad1D(a, x), grad1D(b, x - 1));
}
////////////////
double perlin2D(double x, double y) {
    int X = (int)floor(x) & 255; 
    int Y = (int)floor(y) & 255;
    x -= floor(x);              
    y -= floor(y);
    double u = fade(x);  
    double v = fade(y);

    int aa = p[p[X] + Y];
    int ab = p[p[X] + Y + 1];
    int ba = p[p[X + 1] + Y];
    int bb = p[p[X + 1] + Y + 1];

    double res = lerpgpt(v, lerpgpt(u, grad2D(aa, x, y), grad2D(ba, x - 1, y)),
                         lerpgpt(u, grad2D(ab, x, y - 1), grad2D(bb, x - 1, y - 1)));
    return res;
}


/// debug stuff
void saveMapAsImage(const Block block[worldSizeW][worldSizeH], const char* filename) {

    Image image = GenImageColor(worldSizeW, worldSizeH, BLACK);

    for (int y = 0; y < worldSizeH; y++) {
        for (int x = 0; x < worldSizeW; x++) {
            Color color = getColorForBlockType(block[x][y].type);
            ImageDrawPixel(&image, x, y, color);
        }
    }
    ExportImage(image, filename);

    UnloadImage(image);
}

void saveWorld(const Block block[worldSizeW][worldSizeH], const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Could not open file for saving.\n";
        return;
    }

    // Write the entire block array to the file
    outFile.write(reinterpret_cast<const char*>(&block[0][0]), sizeof(Block) * 6400 * 1800);

    outFile.close();
    std::cout << "World saved successfully.\n";
}

void loadWorld(Block block[6400][1800], const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error: Could not open file for loading.\n";
        return;
    }

    // Read the entire block array from the file
    inFile.read(reinterpret_cast<char*>(&block[0][0]), sizeof(Block) * 6400 * 1800);

    inFile.close();
    std::cout << "World loaded successfully.\n";
}

void generateNewWorldMap(Tile* tile, Player* player, Inventory* inventory) {
    unsigned int seed = 1234;
    int terrainHeight;
    int terrainEdge = 200;
    initPermutation();
            
    for (int i = 0; i < worldSizeW - 1; i++){
        for (int j = 0; j < worldSizeH - 1; j++){
            block[i][j].position.x = worldStartX + i * tile->size;
            block[i][j].position.y = worldStartY + j * tile->size;
            block[i][j].hitbox.x = block[i][j].position.x;
            block[i][j].hitbox.y = block[i][j].position.y;

            double ni = (double)i / worldSizeW;
            double nj = (float)j / worldSizeH;
            double blockHighVal = perlin1D(ni * 20);

            double frequency = 0.5;
            double amplitude = 1.0;
            double persistence = 0.5;
            double totalAmplitude = 0.0;
            double height = 0.0;

            for (int octave = 0; octave < 12; octave++) {
                height += perlin1D(ni * frequency * 20) * amplitude;
                totalAmplitude += amplitude;
                amplitude *= persistence;
                frequency *= 2.0;
            }
            height /= totalAmplitude;
            height = (height + 1) / 2;
            terrainHeight = static_cast<int>(height * (worldSizeH / 3));

            if (j < terrainHeight) { //shapes bigger hills on surfice
                block[i][j].type = Block::SKY;
                block[i][j].health = 0;
                block[i][j].solid = false;
            }
            else if(j < terrainHeight + 50 + (perlin1D(ni * 20) + 0.3) * 150 + GetRandomValue(-3, 3)) { //makes dirt layeer near surfice                                 
                block[i][j].type = Block::DIRT;
                block[i][j].health = 300;
                block[i][j].solid = true;
                if (i < terrainEdge + (perlin1D(nj * 20) + 0.3) * 100 + GetRandomValue(-5, 5)|| i > worldSizeW - terrainEdge + (perlin1D(nj * 20) + 0.3) * 100 + GetRandomValue(-5, 5)) {
                    block[i][j].type = Block::SAND;
                    block[i][j].health = 300;
                    block[i][j].solid = true;
                }                           
            }else{           // makes stone layer bellow sirfice
                block[i][j].type = Block::STONE;
                block[i][j].health = 600;
                block[i][j].solid = true; 
            }
            double perlinCaves = perlin2D(ni * 10, nj * 35); //perfect for bigger caves
            if (j > terrainHeight && perlinCaves <= -0.5f && perlinCaves > -1.0f){
                block[i][j].type = Block::SKY;
                block[i][j].health = 0;
                block[i][j].solid = false;
            }
            perlinCaves = perlin2D(ni * 95, nj * 30);//working on tunnels
            if (j > terrainHeight && perlinCaves <= 0.600f && perlinCaves > 0.483f){
                block[i][j].type = Block::SKY;
                block[i][j].health = 0;
                block[i][j].solid = false;
            } else if (j > terrainHeight && perlinCaves <= 0.790f && perlinCaves > 0.600f) {
                block[i][j].type = Block::CLAY;
                block[i][j].health = 300;
                block[i][j].solid = true;
            } else if (j > terrainHeight && perlinCaves <= 0.81f && perlinCaves > 0.790f) {
                block[i][j].type = Block::MUD;
                block[i][j].health = 300;
                block[i][j].solid = true;
            } else if (j > terrainHeight && perlinCaves <= 1.0f && perlinCaves > 0.81f) {
                block[i][j].type = Block::SNOW;
                block[i][j].health = 300;
                block[i][j].solid = true;
            }
            perlinCaves = perlin2D(ni * 90, nj * 90); //perfect for small caves
            if (j > 450 && perlinCaves <= -0.5f && perlinCaves > -1.0f){
                block[i][j].type = Block::SKY;
                block[i][j].health = 0;
                block[i][j].solid = false;
            }             
        }
    }
    player->position.y = worldStartY + (terrainHeight - 4) * tile->size;
    player->hitbox.y = player->position.y + player->hitboxOffset.y;
    inventory->resetInventory();
}

void drawMenuBackground(Texture2D* loadScreenSky, Vector2* mainMenuCloudPos, Texture2D* loadScreenCloud_1, Texture2D* loadScreen, float resolutionScale) {
    DrawTextureEx(*loadScreenSky, *mainMenuCloudPos, 0.0f, resolutionScale * 1.5f, WHITE);
    DrawTextureEx(*loadScreenSky, (Vector2) { (mainMenuCloudPos->x - loadScreenSky->width), mainMenuCloudPos->y}, 0.0f, resolutionScale * 1.5f, WHITE);
    DrawTextureEx(*loadScreenCloud_1, Vector2{ 0.0f, 0.0f}, 0.0f, resolutionScale * 1.5f, WHITE);
    DrawTextureEx(*loadScreen, Vector2{ 0.0f, 0.0f}, 0.0f, resolutionScale * 1.5f, WHITE);
}
 