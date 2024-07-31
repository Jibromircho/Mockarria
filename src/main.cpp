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
    enum Type { SKY = 0, DIRT, STONE, CLAY} type;
    // Constructor with default values
    Block() : solid(false), position({ worldStartX, worldStartY}), hitbox({worldStartX, worldStartY, 16,16}), type(SKY) {}
} Block;

Block block[worldSizeW][worldSizeH];

// screen constants
constexpr int workingWidth = 1280;
constexpr int buttonFontSize = 26;
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
    float nativeResWidth = GetScreenWidth();
    float nativeResHeight = GetScreenHeight();
    SetWindowSize(nativeResWidth, nativeResHeight);
    ToggleFullscreen();
    SetExitKey(0);

    //button positions
    float resolutionScale = nativeResWidth / workingWidth;
    const Vector2 playButtonPos { nativeResWidth / 2 - 100, nativeResHeight / 2 - 100 };
    const Vector2 settingsButtonPos { nativeResWidth / 2 - 100, nativeResHeight / 2  - 25 };
    const Vector2 exitButtonPos { nativeResWidth / 2 - 100, nativeResHeight / 2  + 50 };
    const Vector2 backButtonPos { 50 , nativeResHeight - 100};
    const Vector2 createWorldButtonPos = { nativeResWidth / 2 - 300, nativeResHeight - 100 };
    const Vector2 loadWorldButtonPos = { nativeResWidth / 2 , nativeResHeight - 100 };

    //game elements 
    GameScreen currentScreen = LOGO;
    Player player;
    World world;
    Tile tile;

    //camera initialization
    Camera2D camera = { 0 };
    camera.target = {player.position.x , player.position.y };
    camera.offset = {nativeResWidth / 2.0f, nativeResHeight / 2.0f};
    camera.zoom = 1.8f;
    Rectangle scissorArea = { player.position.x, player.position.y, nativeResWidth,nativeResHeight};
   
    //initialize audio stuff
    InitAudioDevice();
    Music mainMenuMusic1 = LoadMusicStream("../sfx/Menu_soundtrack_1.mp3");
    mainMenuMusic1.looping = true;
    SetMusicVolume(mainMenuMusic1, volumeMusic * volumeMaster);
    PlayMusicStream(mainMenuMusic1);

    //initialize textures
    Texture2D healthUi = LoadTexture("../img/ui/Health_blank_x3.png");
    Texture2D loadScreen = LoadTexture("../img/backgrounds/Initial_load_screen_no_sky.png");
    Texture2D loadScreenSky = LoadTexture("../img/backgrounds/main_menu_sky.png");
    Texture2D loadScreenCloud_1 = LoadTexture("../img/backgrounds/main_menu_cloud_1.png");
    Texture2D buttonsEmpty = LoadTexture("../img/ui/ui_buttons_x2.png");


    Rectangle buttonNonPressed = { 0.0f, 0.0f, (float)buttonsEmpty.width/2, (float)buttonsEmpty.height/2 };
    Rectangle buttonHover = { 0.0f, (float)buttonsEmpty.height/2, (float)buttonsEmpty.width/2, (float)buttonsEmpty.height/2 };
    Rectangle buttonPressed = { (float)buttonsEmpty.width/2, (float)buttonsEmpty.height/2, (float)buttonsEmpty.width/2, (float)buttonsEmpty.height/2 };
    Vector2 mainMenuCloudPos = Vector2{ 0.0f, 0.0f};

    //button hitboxes
    const Rectangle playButtonHitbox { playButtonPos.x , playButtonPos.y , (float)buttonsEmpty.width/2, (float)buttonsEmpty.height / 2 };
    const Rectangle settingsButtonHitbox { settingsButtonPos.x , settingsButtonPos.y , (float)buttonsEmpty.width/2, (float)buttonsEmpty.height / 2 };
    const Rectangle exitButtonHitbox { exitButtonPos.x , exitButtonPos.y , (float)buttonsEmpty.width/2, (float)buttonsEmpty.height / 2 };
    const Rectangle backButtonHitbox { backButtonPos.x , backButtonPos.y , (float)buttonsEmpty.width/2, (float)buttonsEmpty.height / 2 };
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
        Vector2 worldMousePosition = GetScreenToWorld2D(mousePosition, camera);
        Rectangle mouseHitbox = { mousePosition.x, mousePosition.y, 5.0f, 5.0f };
        Rectangle worldMouseHitbox = { worldMousePosition.x, worldMousePosition.y, 0.0f, 0.0f };


        int firstBlockX = (worldSizeW / 2) + (player.position.x - nativeResWidth) / 16;
        int lastBlockX = (worldSizeW / 2) + (player.position.x + nativeResWidth) / 16;
        int firstBlockY = (worldSizeH / 5) + (player.position.y - nativeResHeight) / 16;
        int lastBlockY = (worldSizeH / 5) + (player.position.y + nativeResHeight) / 16;
        
   
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
            if (IsKeyDown(KEY_LEFT_SHIFT)||IsKeyDown(KEY_RIGHT_SHIFT)) player.movementSpeed = 6.0f;
            if (IsKeyReleased(KEY_LEFT_SHIFT)||IsKeyReleased(KEY_RIGHT_SHIFT)) player.movementSpeed = 2.0f;

            if (IsKeyDown(KEY_A)) player.position.x -= player.movementSpeed;
            else if (IsKeyDown(KEY_D)) player.position.x += player.movementSpeed;



            if (world.getVelocity() < world.getVelocityMax()) world.setVelocity(world.getVelocity() + world.getAcceleration());
            else world.setVelocity(world.getVelocityMax());

            player.hitbox.x = player.position.x + player.hitboxOffset.x;

            //tile hitbox checks
            if (IsKeyPressed(KEY_SPACE) && player.jumpCount > 0) world.setVelocity(player.jumpStrength), player.state = JUMPING, player.jumpCount--;
            player.position.y += world.getVelocity();
            player.hitbox.y += world.getVelocity();
            for (int i = firstBlockX; i < lastBlockX; i++){
                for (int j = firstBlockY; j < lastBlockY; j++){
                    if (CheckCollisionRecs(worldMouseHitbox, block[i][j].hitbox)){
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                        block[i][j].type = Block::SKY;
                        block[i][j].solid = false;
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
        if (IsKeyDown(KEY_ESCAPE)) currentScreen = TITLE, player.saveGame(player.position, "../save/playerSave.dat");
        // camera follows the player and zoom handling
        camera.zoom += ((float)GetMouseWheelMove()*0.1f);
        if (camera.zoom > 4.0f) camera.zoom = 4.0f;
        else if (camera.zoom < 1.2f) camera.zoom = 1.2f;
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

                            if (j < 370 + perlin1D(blockHighVal * 10) * 20 || j > 1650) {
                                block[i][j].type = Block::SKY;
                                block[i][j].solid = false;
                            }else if(j < 385 + perlin1D(blockHighVal * 20) * 5){                                           
                                block[i][j].type = Block::DIRT;
                                block[i][j].solid = true;                            
                            }else if(j < 400 + perlin1D(blockHighVal * 30) * 10){
                                block[i][j].type = Block::STONE;
                                block[i][j].solid = true; 
                            }else {
                                double blockVal = perlin2D(ni * 500, nj * 75);
                                if (blockVal <= -0.85f && blockVal > -1.0f) {
                                    block[i][j].type = Block::CLAY;
                                    block[i][j].solid = true;                             
                                } else if (blockVal <= -0.2f && blockVal > -0.85f) {
                                    block[i][j].type = Block::DIRT;
                                    block[i][j].solid = true;
                                } 
                                else if (blockVal <= 1.0f && blockVal > -0.2f) {
                                    block[i][j].type = Block::STONE;
                                    block[i][j].solid = true;
                                }                                                     
                            }
                            double perlinCaves = perlin2D(ni * 10, nj * 35); //perfect for bigger caves
                            if (j > 450 && perlinCaves <= -0.5f && perlinCaves > -1.0f){
                                block[i][j].type = Block::SKY;
                                block[i][j].solid = false;
                            }
                            perlinCaves = perlin2D(ni * 35, nj * 55);//working on tunnels
                            if (perlinCaves <= 0.767f && perlinCaves > 0.483f){
                                block[i][j].type = Block::SKY;
                                block[i][j].solid = false;
                            }
                            perlinCaves = perlin2D(ni * 90, nj * 90); //perfect for small caves
                            if (j > 400 && perlinCaves <= -0.5f && perlinCaves > -1.0f){
                                block[i][j].type = Block::SKY;
                                block[i][j].solid = false;
                            }
                        }
                    }
                    saveMapAsImage(block, "world_map.png");
                }
    
                else DrawTextureRec(buttonsEmpty, buttonHover, createWorldButtonPos, WHITE);
                
                if (!CheckCollisionRecs(mouseHitbox, loadWorldButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonNonPressed, loadWorldButtonPos, WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseHitbox, loadWorldButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonPressed, loadWorldButtonPos, WHITE);
                    player.loadGame(player.position, "../save/playerSave.dat");
                    currentScreen = GAMEPLAY;
                }

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
                if (!CheckCollisionRecs(mouseHitbox, backButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonNonPressed, backButtonPos, WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseHitbox, backButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonPressed, backButtonPos, WHITE);
                    currentScreen = TITLE;
                }
                else DrawTextureRec(buttonsEmpty, buttonHover, backButtonPos, WHITE);
                DrawText("BACK", backButtonPos.x + 20 , backButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);

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
                                DrawTextureRec(tile.tileSet, tile.stoneMid, block[i][j].position, WHITE);
                                break;
                            case Block::CLAY:
                                DrawTextureRec(tile.tileSet, tile.clay, block[i][j].position, WHITE);
                                break;
                            default:
                                break;
                            }
                        }
                    }
                    DrawRectangleLinesEx(player.hitbox, 2, GREEN);
                    //player drawing
                    if(IsKeyDown(KEY_A)) DrawTextureRec(player.model_movement, player.frameRecMove, player.position, WHITE);
                    else if(IsKeyDown(KEY_D)) DrawTextureRec(player.model_movement, player.frameRecMove, player.position, WHITE);
                    else if(IsKeyUp(KEY_A && KEY_D)) DrawTextureRec(player.model_movement, player.frameRecIdle, player.position, WHITE);
                EndMode2D();

                //Drawing ui elemnts
                DrawTextureEx(healthUi,Vector2{ 0.0f, 0.0f}, 0.0f, resolutionScale / 2, WHITE);
                DrawFPS ( 20, 20);

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
