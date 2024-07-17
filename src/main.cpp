#include <config.h>

// some global val we will need


typedef enum GameScreen { LOGO = 0, TITLE, NEWGAME, LOADGAME, SETTINGS, GAMEPLAY, ENDING} GameScreen;
int main() {
    // initialize win
    float nativeResWidth, nativeResHeight;
    const int workingWidth = 1280;
    const int workingHeight = 720;
    const Vector2 screenStartPos{ 0, 0 };
    const int buttonFontSize = 28;
    int fps = 60;


    //screen values
    InitWindow(0, 0, "Mockarria");
    nativeResWidth = GetScreenWidth();
    nativeResHeight = GetScreenHeight();
    SetWindowSize(nativeResWidth, nativeResHeight);
    ToggleFullscreen();

    float resolutionScale = nativeResWidth / workingWidth;
    const Vector2 newGameButtonPos { nativeResWidth / 2 - 100, nativeResHeight / 2 - 100 };
    const Vector2 loadGameButtonPos { nativeResWidth / 2 - 100, nativeResHeight / 2 - 25 };
    const Vector2 settingsButtonPos { nativeResWidth / 2 - 100, nativeResHeight / 2  + 50 };
    const Vector2 exitButtonPos { nativeResWidth / 2 - 100, nativeResHeight / 2  + 125 };
    const Vector2 backButtonPos { 50 , nativeResHeight - 100};

    //sound values
    InitAudioDevice();

    Music mainMenuMusic1 = LoadMusicStream("../sfx/Menu_soundtrack_1.mp3");
    mainMenuMusic1.looping = true;
    float volumeMaster = 0.8f;
    float volumeMusic = 0.8f;
    float volumeSoundFX = 0.8f;
    float volumeAmbient = 0.8f;

    SetMusicVolume(mainMenuMusic1, volumeMusic * volumeMaster);
    PlayMusicStream(mainMenuMusic1);

    //classes and general initializations
    GameScreen currentScreen = LOGO;
    Player player;
    World world;
    Tile tile;

    Rectangle floor = {-3000, player.position.y + 50, 8000, 30};

    //camera
    Camera2D camera = { 0 };
    camera.target = (Vector2){player.position.x + 20.0f, player.position.y + 20.0f};
    camera.offset = (Vector2){nativeResWidth/2.0f, nativeResHeight/2.0f};
    camera.zoom = 1.0f;

    //initialize textures
    Texture2D healthUi = LoadTexture("../img/ui/Health_blank_x3.png");
    Texture2D loadScreen = LoadTexture("../img/backgrounds/Initial_load_screen_no_sky.png");
    Texture2D loadScreenSky = LoadTexture("../img/backgrounds/main_menu_sky.png");
    Texture2D loadScreenCloud_1 = LoadTexture("../img/backgrounds/main_menu_cloud_1.png");
    Texture2D buttonsEmpty = LoadTexture("../img/ui/ui_buttons_x2.png");
    Rectangle buttonNonPressed = { 0.0f, 0.0f, (float)buttonsEmpty.width/2, (float)buttonsEmpty.height/2 };
    Rectangle buttonHover = { 0.0f, (float)buttonsEmpty.height/2, (float)buttonsEmpty.width/2, (float)buttonsEmpty.height/2 };
    Rectangle buttonPressed = { (float)buttonsEmpty.width/2, (float)buttonsEmpty.height/2, (float)buttonsEmpty.width/2, (float)buttonsEmpty.height/2 };
    Vector2 mainMenuCloudPos = screenStartPos;

    //button hitboxes
    const Rectangle newGameButtonHitbox { newGameButtonPos.x , newGameButtonPos.y , (float)buttonsEmpty.width/2, (float)buttonsEmpty.height/2 };
    const Rectangle loadGameButtonHitbox { loadGameButtonPos.x , loadGameButtonPos.y , (float)buttonsEmpty.width/2, (float)buttonsEmpty.height/2 };
    const Rectangle settingsButtonHitbox { settingsButtonPos.x , settingsButtonPos.y , (float)buttonsEmpty.width/2, (float)buttonsEmpty.height/2 };
    const Rectangle exitButtonHitbox { exitButtonPos.x , exitButtonPos.y , (float)buttonsEmpty.width/2, (float)buttonsEmpty.height/2 };
    const Rectangle backButtonHitbox { backButtonPos.x , backButtonPos.y , (float)buttonsEmpty.width/2, (float)buttonsEmpty.height/2 };

    //frame related stuff
    int frameCounter = 0;
    SetTargetFPS(fps); // set target fps

    int currentFrameMove = 0;
    float currentFrameIdle = 0;

    int framesCounter = 0;
    int framesSpeed = 8;

    // Main game loop
    while (!WindowShouldClose()) // detect window closure
    {
        //Constantly updating stuff
        framesCounter++;
        Rectangle mousePosition = { (float)GetMouseX(), (float)GetMouseY(), 5.0f, 5.0f};

        UpdateMusicStream(mainMenuMusic1);

        if (mainMenuCloudPos.x >= 0) {
            mainMenuCloudPos.x += 0.05f;
        }
        else mainMenuCloudPos.x = screenStartPos.x;

        if(currentScreen == GAMEPLAY) 
        {
            StopMusicStream(mainMenuMusic1);

            player.position.y += world.velocity;
            player.hitbox.y += world.velocity;
            player.hitbox.x = player.position.x + 22;
            if (world.velocity < world.velocityMax) world.velocity += world.acceleration;
            else world.velocity = world.velocityMax;
            if (CheckCollisionRecs(player.hitbox, floor))
            {
                player.jumpCount = player.maxJump;
                world.velocity = 0;
                player.state = GROUND;
            }

            if (framesCounter >= (60/framesSpeed))
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

        //player movement
        if (IsKeyDown(KEY_LEFT_SHIFT)||IsKeyDown(KEY_RIGHT_SHIFT)) player.movementSpeed = 10.0f;
        if (IsKeyReleased(KEY_LEFT_SHIFT)||IsKeyReleased(KEY_RIGHT_SHIFT)) player.movementSpeed = 3.0f;
        if (IsKeyDown(KEY_A)) player.position.x -= player.movementSpeed;
        else if (IsKeyDown(KEY_D)) player.position.x += player.movementSpeed;

        if (IsKeyPressed(KEY_SPACE) && player.jumpCount > 0 /*&& player.state == GROUND*/) world.velocity = player.jumpStrength, player.state = JUMPING, player.jumpCount--;

        // camera follow the player
        camera.target = {player.position.x + 20, player.position.y + 20};
        }

        // screen selector
        switch (currentScreen)
        {
        case LOGO:

            frameCounter++;
            if (frameCounter > 40)
            {
                currentScreen = TITLE;
            }
            break;
        case TITLE:
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                currentScreen = GAMEPLAY;
            }
        }
        
        default:
            break;
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
                DrawTextureEx(loadScreenCloud_1, screenStartPos, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreen, screenStartPos, 0.0f, resolutionScale, WHITE);
                DrawText("LOGO SCREEN", 20, 20, 40, LIGHTGRAY);
            } break;
            case TITLE:
            {
                //draw title screen elements
                 DrawTextureEx(loadScreenSky, mainMenuCloudPos, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreenSky, (Vector2) { (mainMenuCloudPos.x - loadScreenSky.width) * resolutionScale, mainMenuCloudPos.y}, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreenCloud_1, screenStartPos, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreen, screenStartPos, 0.0f, resolutionScale, WHITE);
                DrawText("MOCKARRIA by Didi", 20, 20, 36, LIGHTGRAY);

                //draw all the buttons
                if (!CheckCollisionRecs(mousePosition, newGameButtonHitbox)){
                    DrawTextureRec(buttonsEmpty, buttonNonPressed, newGameButtonPos, WHITE);
                    }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mousePosition, newGameButtonHitbox)){
                        DrawTextureRec(buttonsEmpty, buttonPressed, newGameButtonPos, WHITE);
                        currentScreen = GAMEPLAY;
                }
                else DrawTextureRec(buttonsEmpty, buttonHover, newGameButtonPos, WHITE);

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
                DrawText("NEW GAME", newGameButtonPos.x + buttonFontSize , newGameButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);
                DrawText("LOAD GAME", loadGameButtonPos.x + buttonFontSize , loadGameButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);
                DrawText("SETTINGS", settingsButtonPos.x + buttonFontSize , settingsButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);
                DrawText("EXIT", exitButtonPos.x + buttonFontSize , exitButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);


            }break;
            case NEWGAME:
            {
                //draw background
                DrawTextureEx(loadScreenSky, (Vector2) { mainMenuCloudPos.x * -1, mainMenuCloudPos.y * -1}, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreenCloud_1, screenStartPos, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreen, screenStartPos, 0.0f, resolutionScale, WHITE);
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
                DrawText("BACK", backButtonPos.x + buttonFontSize , backButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);

            }break;
            case LOADGAME:
            {
                //draw background
                DrawTextureEx(loadScreenSky, (Vector2) { mainMenuCloudPos.x * -1, mainMenuCloudPos.y * -1}, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreenCloud_1, screenStartPos, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreen, screenStartPos, 0.0f, resolutionScale, WHITE);
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
                DrawText("BACK", backButtonPos.x + buttonFontSize , backButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);

            }break;
            case SETTINGS:
            {
                //draw background
                DrawTextureEx(loadScreenSky, (Vector2) { mainMenuCloudPos.x * -1, mainMenuCloudPos.y * -1}, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreenCloud_1, screenStartPos, 0.0f, resolutionScale, WHITE);
                DrawTextureEx(loadScreen, screenStartPos, 0.0f, resolutionScale, WHITE);
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
                DrawText("BACK", backButtonPos.x + buttonFontSize , backButtonPos.y + buttonFontSize / 2 , buttonFontSize, RAYWHITE);

            }break;
            case GAMEPLAY:
            {
                BeginMode2D(camera);
                    //hitbox for easier debugging
                    DrawRectangleLinesEx(player.hitbox,2.0f,RED);

                    //simple floor for testing
                    DrawRectangle(floor.x, floor.y, floor.width, floor.height, BLACK);
                    //player drawing
                    if(IsKeyDown(KEY_A)) DrawTextureRec(player.model_movement, player.frameRecMove, player.position, WHITE);
                    else if(IsKeyDown(KEY_D)) DrawTextureRec(player.model_movement, player.frameRecMove, player.position, WHITE);
                    else if(IsKeyUp(KEY_A && KEY_D)) DrawTextureRec(player.model_movement, player.frameRecIdle, player.position, WHITE);
                EndMode2D();
                //Drawing ui elemnts
                DrawTextureEx(healthUi,screenStartPos, 0.0f, resolutionScale / 2, WHITE);

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
