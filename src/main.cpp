#include <config.h>

///////////////////////////////////
///////////////////////////////////some global varuables we will need

//game current screen
typedef enum GameScreen { LOGO = 0, TITLE, NEWGAME, LOADGAME, SETTINGS, GAMEPLAY, ENDING} GameScreen;

const int worldSizeW = 6400;
const int worldSizeH = 1800;
const float worldStartX = (float)worldSizeW * 16 / 2 * - 1;
const float worldStartY = (float) worldSizeH * 16 / 2 * -1;
typedef struct Block {
    bool solid = false;
    Vector2 position;
    Rectangle hitbox;
    enum Type { SKY = 0, GRASS, STONE, ICE} type;
    // Constructor with default values
    Block() : solid(false), position({ worldStartX, worldStartY}), hitbox({worldStartX, worldStartY, 16,16}), type(SKY) {}
} Block;

Block block[worldSizeW][worldSizeH];

// screen constants
constexpr int workingWidth = 1280;
constexpr int buttonFontSize = 28;
constexpr int fps = 60;

//audio volumes
float volumeMaster = 0.8f;
float volumeMusic = 0.8f;
float volumeSoundFX = 0.8f;
float volumeAmbient = 0.8f;

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
    const Vector2 loadGameButtonPos { nativeResWidth / 2 - 100, nativeResHeight / 2 - 25 };
    const Vector2 settingsButtonPos { nativeResWidth / 2 - 100, nativeResHeight / 2  + 50 };
    const Vector2 exitButtonPos { nativeResWidth / 2 - 100, nativeResHeight / 2  + 125 };
    const Vector2 backButtonPos { 50 , nativeResHeight - 100};
    const Vector2 createWorldButtonPos = { nativeResWidth / 2 - 300, nativeResHeight - 100 };

    //initializing audio
    InitAudioDevice();
    Music mainMenuMusic1 = LoadMusicStream("../sfx/Menu_soundtrack_1.mp3");
    mainMenuMusic1.looping = true;
    SetMusicVolume(mainMenuMusic1, volumeMusic * volumeMaster);
    PlayMusicStream(mainMenuMusic1);

    //game elements 
    GameScreen currentScreen = LOGO;
    Player player;
    World world;
    Tile tile;


    Image perlin = GenImagePerlinNoise(worldSizeW, worldSizeH, 10, 10, 10);
    ExportImage(perlin, "../save/map.png");
    //create a map
    for (int i = 0; i < worldSizeW - 1; i++){
        for (int j = 0; j < worldSizeH - 1; j++){
            block[i][j].position.x = worldStartX + i * tile.size;
            block[i][j].position.y = worldStartY + j * tile.size;
            block[i][j].hitbox.x = worldStartX + i * tile.size;
            block[i][j].hitbox.y = worldStartY + j * tile.size;

            int randomNum = GetRandomValue(0, 9);
            switch (randomNum){

            case 0:
                block[i][j].type = Block::SKY;
                block[i][j].solid = false;
                break;
            case 1:
                block[i][j].type = Block::GRASS;
                block[i][j].solid = true;
                break;
            case 2:
                block[i][j].type = Block::STONE;
                block[i][j].solid = true;
                break;
            case 3:
                block[i][j].type = Block::ICE;
                block[i][j].solid = false;
                break;
            default:
                block[i][j].type = Block::SKY;
                block[i][j].solid = false;
                break;
            }
        }
    }
    Rectangle floor = {-300, 100, 8000, 500};

    //camera initialization
    Camera2D camera = { 0 };
    camera.target = {player.position.x + 100.0f, player.position.y + 100.0f};
    camera.offset = {nativeResWidth / 2.0f, nativeResHeight / 2.0f};
    camera.zoom = 1.0f;
    Rectangle scissorArea = { player.position.x, player.position.y, nativeResWidth,nativeResHeight};
   


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
    const Rectangle loadGameButtonHitbox { loadGameButtonPos.x , loadGameButtonPos.y , (float)buttonsEmpty.width/2, (float)buttonsEmpty.height / 2 };
    const Rectangle settingsButtonHitbox { settingsButtonPos.x , settingsButtonPos.y , (float)buttonsEmpty.width/2, (float)buttonsEmpty.height / 2 };
    const Rectangle exitButtonHitbox { exitButtonPos.x , exitButtonPos.y , (float)buttonsEmpty.width/2, (float)buttonsEmpty.height / 2 };
    const Rectangle backButtonHitbox { backButtonPos.x , backButtonPos.y , (float)buttonsEmpty.width/2, (float)buttonsEmpty.height / 2 };
    const Rectangle createWorldButtonHitbox { createWorldButtonPos.x , createWorldButtonPos.y, (float)buttonsEmpty.width / 2, (float)buttonsEmpty.height / 2 };

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
        Rectangle mousePosition = { (float)GetMouseX(), (float)GetMouseY(), 5.0f, 5.0f};


        int firstBlockX = (worldSizeW / 2) + (player.position.x - scissorArea.width) / 16;
        int lastBlockX = (worldSizeW / 2) + (player.position.x + scissorArea.width) / 16;
        int firstBlockY = (worldSizeH / 2) + (player.position.y - scissorArea.height) / 16;
        int lastBlockY = (worldSizeH / 2) + (player.position.y + scissorArea.height) / 16;

        
   
        UpdateMusicStream(mainMenuMusic1);

        if (mainMenuCloudPos.x >= 0) {
            mainMenuCloudPos.x += 0.05f;
        }
        else mainMenuCloudPos.x = 0.0f;

        if(currentScreen == GAMEPLAY) 
        {
            StopMusicStream(mainMenuMusic1);

            //update all x coordinates
            player.hitbox.x = player.position.x + 25;
            
            // upadte all y coordinates
            player.position.y += world.getVelocity();
            player.hitbox.y += world.getVelocity();


            if (world.getVelocity() < world.getVelocityMax()) world.setVelocity(world.getVelocity() + world.getAcceleration());
            else world.setVelocity(world.getVelocityMax());

            for (int i = firstBlockX; i < lastBlockX; i++){
                for (int j = firstBlockY; j < lastBlockY; j++){
                    if (CheckCollisionRecs(player.hitbox, block[i][j].hitbox) && block[i][j].solid)
                    {
                        player.jumpCount = player.maxJump;
                        world.setVelocity(0);
                        player.state = GROUND;
                    }
                }
            }

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
        if (IsKeyDown(KEY_LEFT_SHIFT)||IsKeyDown(KEY_RIGHT_SHIFT)) player.movementSpeed = 10.0f;
        if (IsKeyReleased(KEY_LEFT_SHIFT)||IsKeyReleased(KEY_RIGHT_SHIFT)) player.movementSpeed = 3.0f;
        if (IsKeyDown(KEY_A)) player.position.x -= player.movementSpeed;
        else if (IsKeyDown(KEY_D)) player.position.x += player.movementSpeed;

        if (IsKeyPressed(KEY_SPACE) && player.jumpCount > 0) world.setVelocity(player.jumpStrength), player.state = JUMPING, player.jumpCount--;

        //other keys and inputs
        if (IsKeyDown(KEY_ESCAPE)) currentScreen = TITLE;
        // camera follow the player
        camera.target = {player.position.x + 100, player.position.y + 20};
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
                if (!CheckCollisionRecs(mousePosition, playButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonNonPressed, playButtonPos, WHITE);
                    }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mousePosition, playButtonHitbox)){
                        DrawTextureRec(buttonsEmpty, buttonPressed, playButtonPos, WHITE);
                        currentScreen = NEWGAME;
                }
                else DrawTextureRec(buttonsEmpty, buttonHover, playButtonPos, WHITE);

                if (!CheckCollisionRecs(mousePosition, loadGameButtonHitbox)){

                    DrawTextureRec(buttonsEmpty, buttonNonPressed, loadGameButtonPos , WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mousePosition, loadGameButtonHitbox)){
                        DrawTextureRec(buttonsEmpty, buttonPressed, loadGameButtonPos, WHITE);
                        currentScreen = LOADGAME;
                }
                else DrawTextureRec(buttonsEmpty, buttonHover, loadGameButtonPos, WHITE);

                if (!CheckCollisionRecs(mousePosition, settingsButtonHitbox)){

                    DrawTextureRec(buttonsEmpty, buttonNonPressed, settingsButtonPos, WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mousePosition, settingsButtonHitbox)){
                        DrawTextureRec(buttonsEmpty, buttonPressed, settingsButtonPos, WHITE);
                        currentScreen = SETTINGS;
                }
                else DrawTextureRec(buttonsEmpty, buttonHover, settingsButtonPos, WHITE);

                if (!CheckCollisionRecs(mousePosition, exitButtonHitbox)){

                    DrawTextureRec(buttonsEmpty, buttonNonPressed, exitButtonPos, WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mousePosition, exitButtonHitbox)){
                        DrawTextureRec(buttonsEmpty, buttonPressed, exitButtonPos, WHITE);
                        currentScreen = ENDING;
                }
                else DrawTextureRec(buttonsEmpty, buttonHover, exitButtonPos, WHITE);

                //draw button text
                DrawText("PLAY", playButtonPos.x + 20 , playButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);
                DrawText("LOAD GAME", loadGameButtonPos.x + 20 , loadGameButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);
                DrawText("SETTINGS", settingsButtonPos.x + 20 , settingsButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);
                DrawText("EXIT", exitButtonPos.x + 20 , exitButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);


            }break;
            case NEWGAME:
            {
                //draw background
                DrawTextureEx(loadScreenSky, mainMenuCloudPos, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreenSky, (Vector2) { (mainMenuCloudPos.x - loadScreenSky.width) * resolutionScale, mainMenuCloudPos.y}, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreenCloud_1, Vector2{ 0.0f, 0.0f}, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreen, Vector2{ 0.0f, 0.0f}, 0.0f, resolutionScale, WHITE);
                DrawText("MOCKARRIA by Didi", 20, 20, 36, LIGHTGRAY);

                //draw all buttons
                if (!CheckCollisionRecs(mousePosition, backButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonNonPressed, backButtonPos, WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mousePosition, backButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonPressed, backButtonPos, WHITE);
                    currentScreen = TITLE;
                }
                else DrawTextureRec(buttonsEmpty, buttonHover, backButtonPos, WHITE);

                if (!CheckCollisionRecs(mousePosition, createWorldButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonNonPressed, createWorldButtonPos, WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mousePosition, createWorldButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonPressed, createWorldButtonPos, WHITE);
                    currentScreen = GAMEPLAY;
                }
                else DrawTextureRec(buttonsEmpty, buttonHover, createWorldButtonPos, WHITE);


                DrawText("BACK", backButtonPos.x + 20 , backButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);
                DrawText("NEW WORLD", createWorldButtonPos.x + 20 , createWorldButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);

            }break;
            case LOADGAME:
            {
                //draw background
                DrawTextureEx(loadScreenSky, mainMenuCloudPos, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreenSky, (Vector2) { (mainMenuCloudPos.x - loadScreenSky.width) * resolutionScale, mainMenuCloudPos.y}, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreenCloud_1, Vector2{ 0.0f, 0.0f}, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreen, Vector2{ 0.0f, 0.0f}, 0.0f, resolutionScale, WHITE);
                DrawText("MOCKARRIA by Didi", 20, 20, 36, LIGHTGRAY);

                //draw all buttons
                if (!CheckCollisionRecs(mousePosition, backButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonNonPressed, backButtonPos, WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mousePosition, backButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonPressed, backButtonPos, WHITE);
                    currentScreen = TITLE;
                }
                else DrawTextureRec(buttonsEmpty, buttonHover, backButtonPos, WHITE);
                DrawText("BACK", backButtonPos.x + 20 , backButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);

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
                if (!CheckCollisionRecs(mousePosition, backButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonNonPressed, backButtonPos, WHITE);
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mousePosition, backButtonHitbox)){
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
                            case Block::GRASS:
                                DrawTextureRec(tile.tileSet, tile.grass, block[i][j].position, WHITE);
                                break;
                            case Block::STONE:
                                DrawTextureRec(tile.tileSet, tile.stone, block[i][j].position, WHITE);
                                break;
                            case Block::ICE:
                                DrawTextureRec(tile.tileSet, tile.ice, block[i][j].position, WHITE);
                                break;
                            default:
                                break;
                            }
                            /*if (block[i][j].type == Block::GRASS){
                                DrawTextureRec(tile.tileSet, tile.grass, block[i][j].position, WHITE);
                            }
                            if (block[i][j].type == Block::STONE){
                                DrawTextureRec(tile.tileSet, tile.stone, block[i][j].position, WHITE);
                            }*/
                        }
                    }
                    DrawRectangleRec(player.hitbox, GREEN);
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

    CloseAudioDevice();

    CloseWindow();  //Close window and OpenGL context

    return 0;
    
};
