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
    Vector2 position;
    Rectangle hitbox;
    int health;
    enum Type { SKY = -1, DIRT = 0, STONE = 1, CLAY = 2} type;
    // Constructor with default values
    Block() : solid(false), position({ worldStartX, worldStartY}), hitbox({worldStartX, worldStartY, 16,16}), type(SKY) {}
} Block;

Block block[worldSizeW][worldSizeH];

// screen constants
constexpr float workingHeight = 720.0f;
constexpr float workingWidth = 1280.0f;
Vector2 supportedResolutions[] = {
    {2560, 1440},
    {1920, 1080},
    {1440, 900},
    {1280, 720},
    {640, 480}
};
int currentResolutionIndex = 3;
constexpr int buttonFontSize = 26;
bool inventoryOpened = false;
constexpr int fps = 60;

//audio volumes
float volumeMaster = 0.8f;
float volumeMusic = 0.8f;
float volumeSoundFX = 0.8f;
float volumeAmbient = 0.8f;

//fucntion definitions
void initPermutation(unsigned int seed = std::time(nullptr));
double fade(double t);
double lerp(double t, double a, double b);
double grad1D(int hash, double x);
double grad2D(int hash, double x, double y);
double perlin1D(double x);
double perlin2D(double x, double y);

////debug stuff
void saveMapAsImage(const Block block[worldSizeW][worldSizeH], const char* filename);

void saveWorld(const Block block[worldSizeW][worldSizeH], const std::string& filename);
void loadWorld(Block block[worldSizeW][worldSizeH], const std::string& filename);

Color getColorForBlockType(Block::Type type) {
    switch (type) {
        case Block::SKY:
            return SKYBLUE;   
        case Block::DIRT:
            return BROWN; 
        case Block::STONE:
            return GRAY;      
        case Block::CLAY:
            return RED;
        default:
            return BLACK;
    }
}

int main() {
    // initialize win
    InitWindow(0, 0, "Mockarria");
    float currentResWidth = (float)GetScreenWidth();
    float currentResHeight = (float)GetScreenHeight();
    SetWindowSize(currentResWidth, currentResHeight);
    ToggleFullscreen();
    SetExitKey(0);

    //button positions
    float resolutionScale = currentResHeight / workingHeight;
    const Vector2 playButtonPos { currentResWidth / 2 - 100, currentResHeight / 2 - 100 };
    const Vector2 settingsButtonPos { currentResWidth / 2 - 100, currentResHeight / 2  - 25 };
    const Vector2 exitButtonPos { currentResWidth / 2 - 100, currentResHeight / 2  + 50 };
    const Vector2 backButtonPos { 50 , currentResHeight - 100};

    const Vector2 fullscreenButtonPos { currentResWidth / 2 - 100, currentResHeight / 2 - 100 };
    const Vector2 resolutionButtonPos { currentResWidth / 2 - 100, currentResHeight / 2  - 25 };
    const Vector2 soundButtonPos { currentResWidth / 2 - 100, currentResHeight / 2  + 50 };

    const Vector2 createWorldButtonPos = { currentResWidth / 2 - 300, currentResHeight - 100 };
    const Vector2 loadWorldButtonPos = { currentResWidth / 2 , currentResHeight - 100 };

    //game elements 
    GameScreen currentScreen = LOGO;
    Player player;
    Inventory inventory;
    World world;
    Tile tile;
    //camera initialization
    Camera2D camera = { 0 };
    camera.target = {player.position.x , player.position.y };
    camera.offset = {currentResWidth / 2.0f, currentResHeight / 2.0f};
    camera.zoom = 1.6f;
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
    const Rectangle resolutionButtonHitbox { resolutionButtonPos.x, resolutionButtonPos.y, (float)buttonsEmpty.width / 2, (float)buttonsEmpty.height / 2 };
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


        int firstBlockX = (worldSizeW / 2) + (player.position.x - currentResWidth) / 16;
        int lastBlockX = (worldSizeW / 2) + (player.position.x + currentResWidth) / 16;
        int firstBlockY = (worldSizeH / 5) + (player.position.y - currentResHeight) / 16;
        int lastBlockY = (worldSizeH / 5) + (player.position.y + currentResHeight) / 16;
        
   
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
            player.updatePlayer();

            world.updateWorld();

            if (IsKeyPressed(KEY_SPACE) && player.jumpCount > 0) world.setVelocity(player.jumpStrength), player.state = JUMPING, player.jumpCount--;
            player.position.y += world.getVelocity();
            player.hitbox.y += world.getVelocity();
            //tile hitbox checks
            for (int i = firstBlockX; i < lastBlockX; i++){
                for (int j = firstBlockY; j < lastBlockY; j++){
                    if (CheckCollisionRecs(worldMouseHitbox, block[i][j].hitbox) && mouse_playerDistance <= 100){
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                            if (block[i][j].health > 0) {
                                block[i][j].health -= 100;
                            } else if (block[i][j].health == 0 && block[i][j].type != Block::SKY) {
                                inventory.addItem(block[i][j].type, 1);
                                block[i][j].type = Block::SKY;
                                block[i][j].solid = false;
                            }
                        }
                    }
                    if (CheckCollisionRecs(worldMouseHitbox, block[i][j].hitbox)) {
                        if (!CheckCollisionRecs(block[i][j].hitbox,player.hitbox)){
                            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                                if (block[i][j].type == -1 && mouse_playerDistance <= 100) {
                                    int itemID = inventory.useItem(block[i][j].type);
                                    if (itemID != -1) {
                                        block[i][j].type = static_cast<Block::Type>(itemID);
                                        block[i][j].solid = true;
                                    }                        
                                }
                            }
                        }
                    }

                    if(block[i][j].solid){
                        Rectangle collisionArea = GetCollisionRec(player.hitbox, block[i][j].hitbox);
                        if (collisionArea.width > collisionArea.height){
                            if (collisionArea.y == player.hitbox.y){
                                player.position.y += collisionArea.height;
                                player.hitbox.y += collisionArea.height;
                                world.setVelocity(world.getVelocity() + 1);
                            }else player.position.y -= collisionArea.height, player.hitbox.y -= collisionArea.height, world.setVelocity(0), player.state = GROUND, player.resetJump();
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

        //other keys and inputs
        if (IsKeyDown(KEY_ESCAPE)) {
            currentScreen = TITLE;
            player.saveGame("../save/playerSave.dat", inventory);
            saveWorld(block, "../save/worldSave.dat");
        }
        //hotbar slot select
        for (int i = 0; i < 10; i++) {
            if (IsKeyPressed('0' + ((i + 1) % 10))) inventory.selectHotbarSlot(i);
        }
        //open invetory
        if (IsKeyPressed(KEY_B)) {
            if(inventoryOpened) inventoryOpened = false;
            else inventoryOpened = true;
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
            {
                DrawTextureEx(loadScreenSky, mainMenuCloudPos, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreenSky, (Vector2) { (mainMenuCloudPos.x - loadScreenSky.width) * resolutionScale, mainMenuCloudPos.y}, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreenCloud_1, Vector2{ 0.0f, 0.0f}, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreen, Vector2{ 0.0f, 0.0f}, 0.0f, resolutionScale, WHITE);
                DrawText("LOGO SCREEN", 20, 20, 40, LIGHTGRAY);
            } break;
            case TITLE:
            {
                //draw title screen elements
                DrawTextureEx(loadScreenSky, mainMenuCloudPos, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreenSky, (Vector2) { (mainMenuCloudPos.x - loadScreenSky.width) * resolutionScale, mainMenuCloudPos.y}, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreenCloud_1, Vector2{ 0.0f, 0.0f}, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreen, Vector2{ 0.0f, 0.0f}, 0.0f, resolutionScale, WHITE);
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
                DrawTextureEx(loadScreenSky, mainMenuCloudPos, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreenSky, (Vector2) { (mainMenuCloudPos.x - loadScreenSky.width) * resolutionScale, mainMenuCloudPos.y}, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreenCloud_1, Vector2{ 0.0f, 0.0f}, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreen, Vector2{ 0.0f, 0.0f}, 0.0f, resolutionScale, WHITE);
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
                    unsigned int seed = 1234;
                    int* terrainHeight = new int(0);
                    initPermutation();
                    for (int i = 0; i < worldSizeW - 1; i++){

                        for (int j = 0; j < worldSizeH - 1; j++){
                            block[i][j].position.x = worldStartX + i * tile.size;
                            block[i][j].position.y = worldStartY + j * tile.size;
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
                            *terrainHeight = static_cast<int>(height * (worldSizeH / 3));

                            if (j < *terrainHeight) { //shapes bigger hills on surfice
                                block[i][j].type = Block::SKY;
                                block[i][j].health = 0;
                                block[i][j].solid = false;
                            }
                            else if(j < *terrainHeight + 50 + (perlin1D(ni * 20) + 0.3) * 150 + GetRandomValue(-3, 3)){ //makes dirt layeer near surfice                                 
                                block[i][j].type = Block::DIRT;
                                block[i][j].health = 300;
                                block[i][j].solid = true;                            
                            }else{                                      // makes stone layer bellow sirfice
                                block[i][j].type = Block::STONE;
                                block[i][j].health = 600;
                                block[i][j].solid = true; 
                            }
                            double perlinCaves = perlin2D(ni * 10, nj * 35); //perfect for bigger caves
                            if (j > 450 && perlinCaves <= -0.5f && perlinCaves > -1.0f){
                                block[i][j].type = Block::SKY;
                                block[i][j].health = 0;
                                block[i][j].solid = false;
                            }
                            perlinCaves = perlin2D(ni * 95, nj * 30);//working on tunnels
                            if (perlinCaves <= 0.767f && perlinCaves > 0.483f){
                                block[i][j].type = Block::SKY;
                                block[i][j].health = 0;
                                block[i][j].solid = false;
                            } else if (perlinCaves <= 1.0f && perlinCaves > 0.767f) {
                                block[i][j].type = Block::CLAY;
                                block[i][j].health = 300;
                                block[i][j].solid = true;
                            }
                            perlinCaves = perlin2D(ni * 90, nj * 90); //perfect for small caves
                            if (perlinCaves <= -0.5f && perlinCaves > -1.0f){
                                block[i][j].type = Block::SKY;
                                block[i][j].health = 0;
                                block[i][j].solid = false;
                            }
                            
                        }
                    }
                    player.position.y = worldStartY + (*terrainHeight - 4) * tile.size;
                    player.hitbox.y = player.position.y + player.hitboxOffset.y;
                    inventory.resetInventory();
                    delete terrainHeight;

                saveMapAsImage(block, "world_map.png");
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
                DrawTextureEx(loadScreenSky, mainMenuCloudPos, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreenSky, (Vector2) { (mainMenuCloudPos.x - loadScreenSky.width) * resolutionScale, mainMenuCloudPos.y}, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreenCloud_1, Vector2{ 0.0f, 0.0f}, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreen, Vector2{ 0.0f, 0.0f}, 0.0f, resolutionScale, WHITE);
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
                //resolution button
                if (!CheckCollisionRecs(mouseHitbox, resolutionButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonNonPressed, resolutionButtonPos, WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseHitbox, resolutionButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonPressed, resolutionButtonPos, WHITE);
                    currentResolutionIndex = (currentResolutionIndex + 1) % (sizeof(supportedResolutions) / sizeof(supportedResolutions[0]));
                    SetWindowSize(supportedResolutions[currentResolutionIndex].x, supportedResolutions[currentResolutionIndex].y);

                }
                else DrawTextureRec(buttonsEmpty, buttonHover, resolutionButtonPos, WHITE);

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
                            switch (block[i][j].type)
                            {
                            case Block::DIRT:
                                if (!block[i][j + 1].solid && !block[i][j - 1].solid && !block[i - 1][j].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.dirtAllSide, block[i][j].position, WHITE);
                                else if (!block[i][j - 1].solid && !block[i - 1][j].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.dirtLeftTopRight, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i - 1][j].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.dirtRightBottLeft, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i][j - 1].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.dirtTopRightBott, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i][j - 1].solid && !block[i - 1][j].solid) DrawTextureRec(tile.tileSet, tile.dirtBottLeftTop, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i - 1][j].solid) DrawTextureRec(tile.tileSet, tile.dirtBottLeft, block[i][j].position, WHITE);
                                else if (!block[i][j - 1].solid && !block[i - 1][j].solid) DrawTextureRec(tile.tileSet, tile.dirtTopLeft, block[i][j].position, WHITE);
                                else if (!block[i + 1][j].solid && !block[i][j - 1].solid) DrawTextureRec(tile.tileSet, tile.dirtTopRight, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.dirtBottRight, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i][j - 1].solid) DrawTextureRec(tile.tileSet, tile.dirtLeftRight, block[i][j].position, WHITE);
                                else if (!block[i - 1][j].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.dirtTopBott, block[i][j].position, WHITE);                    
                                else if (!block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.dirtRight, block[i][j].position, WHITE);
                                else if (!block[i - 1][j].solid) DrawTextureRec(tile.tileSet, tile.dirtLeft, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid) DrawTextureRec(tile.tileSet, tile.dirtBott, block[i][j].position, WHITE);
                                else if (!block[i][j - 1].solid) DrawTextureRec(tile.tileSet, tile.dirtTop, block[i][j].position, WHITE);     
                                else DrawTextureRec(tile.tileSet, tile.dirtCenter, block[i][j].position, WHITE);
                                break;
                            case Block::STONE:
                                if (!block[i][j + 1].solid && !block[i][j - 1].solid && !block[i - 1][j].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.stoneAllSide, block[i][j].position, WHITE);
                                else if (!block[i][j - 1].solid && !block[i - 1][j].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.stoneLeftTopRight, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i - 1][j].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.stoneRightBottLeft, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i][j - 1].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.stoneTopRightBott, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i][j - 1].solid && !block[i - 1][j].solid) DrawTextureRec(tile.tileSet, tile.stoneBottLeftTop, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i - 1][j].solid) DrawTextureRec(tile.tileSet, tile.stoneBottLeft, block[i][j].position, WHITE);
                                else if (!block[i][j - 1].solid && !block[i - 1][j].solid) DrawTextureRec(tile.tileSet, tile.stoneTopLeft, block[i][j].position, WHITE);
                                else if (!block[i + 1][j].solid && !block[i][j - 1].solid) DrawTextureRec(tile.tileSet, tile.stoneTopRight, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.stoneBottRight, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i][j - 1].solid) DrawTextureRec(tile.tileSet, tile.stoneLeftRight, block[i][j].position, WHITE);
                                else if (!block[i - 1][j].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.stoneTopBott, block[i][j].position, WHITE);                    
                                else if (!block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.stoneRight, block[i][j].position, WHITE);
                                else if (!block[i - 1][j].solid) DrawTextureRec(tile.tileSet, tile.stoneLeft, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid) DrawTextureRec(tile.tileSet, tile.stoneBott, block[i][j].position, WHITE);
                                else if (!block[i][j - 1].solid) DrawTextureRec(tile.tileSet, tile.stoneTop, block[i][j].position, WHITE);     
                                else DrawTextureRec(tile.tileSet, tile.stoneCenter, block[i][j].position, WHITE);
                                break;
                            case Block::CLAY:
                                if (!block[i][j + 1].solid && !block[i][j - 1].solid && !block[i - 1][j].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.clayAllSide, block[i][j].position, WHITE);
                                else if (!block[i][j - 1].solid && !block[i - 1][j].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.clayLeftTopRight, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i - 1][j].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.clayRightBottLeft, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i][j - 1].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.clayTopRightBott, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i][j - 1].solid && !block[i - 1][j].solid) DrawTextureRec(tile.tileSet, tile.clayBottLeftTop, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i - 1][j].solid) DrawTextureRec(tile.tileSet, tile.clayBottLeft, block[i][j].position, WHITE);
                                else if (!block[i][j - 1].solid && !block[i - 1][j].solid) DrawTextureRec(tile.tileSet, tile.clayTopLeft, block[i][j].position, WHITE);
                                else if (!block[i + 1][j].solid && !block[i][j - 1].solid) DrawTextureRec(tile.tileSet, tile.clayTopRight, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.clayBottRight, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid && !block[i][j - 1].solid) DrawTextureRec(tile.tileSet, tile.clayLeftRight, block[i][j].position, WHITE);
                                else if (!block[i - 1][j].solid && !block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.clayTopBott, block[i][j].position, WHITE);                    
                                else if (!block[i + 1][j].solid) DrawTextureRec(tile.tileSet, tile.clayRight, block[i][j].position, WHITE);
                                else if (!block[i - 1][j].solid) DrawTextureRec(tile.tileSet, tile.clayLeft, block[i][j].position, WHITE);
                                else if (!block[i][j + 1].solid) DrawTextureRec(tile.tileSet, tile.clayBott, block[i][j].position, WHITE);
                                else if (!block[i][j - 1].solid) DrawTextureRec(tile.tileSet, tile.clayTop, block[i][j].position, WHITE);     
                                else DrawTextureRec(tile.tileSet, tile.clayCenter, block[i][j].position, WHITE);
                                break;
                            default:
                                break;
                            }
                        }
                    }
                    DrawRectangleLinesEx(player.hitbox, 2, GREEN);
                    //player drawing
                    player.drawPlayer();
                EndMode2D();

                //Drawing ui elemnts
                player.drawPlayerHealth(resolutionScale);
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
                    DrawTextureEx(inventorySlot, position, 0.0f, resolutionScale + selectedSlotScale, tint);

                    inventory.drawHotbarItems(tile,{ (currentResWidth / 3) + 48.0f, 16.0f });
                }
                if (inventoryOpened) {
                    for (int i = 0; i < inventory.hotbarSize; i++) {
                        for (int j = 1; j < inventory.inventoryRows; j++) {
                            Vector2 position = { (currentResWidth / 3) + (i * 48.0f) + 48.0f, (j * 48.0f) + 16.0f };
                            Color tint = { 255, 255, 255, 220 };
                            DrawTextureEx(inventorySlot, position, 0.0f, resolutionScale + 0.25f , tint);
                        }
                    }
                }


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
double lerp(double t, double a, double b) {
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

    return lerp(u, grad1D(a, x), grad1D(b, x - 1));
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

    double res = lerp(v, lerp(u, grad2D(aa, x, y), grad2D(ba, x - 1, y)),
                         lerp(u, grad2D(ab, x, y - 1), grad2D(bb, x - 1, y - 1)));
    return res;
}


/// debug stuff
void saveMapAsImage(const Block block[worldSizeW][worldSizeH], const char* filename) {
    int width = worldSizeW;
    int height = worldSizeH;

    Image image = GenImageColor(width, height, BLACK);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Color color = getColorForBlockType(block[x][y].type);
            ImageDrawPixel(&image, x, y, color);
        }
    }
    ExportImage(image, filename);

    UnloadImage(image);
}

void saveWorld(const Block block[6400][1800], const std::string& filename) {
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


