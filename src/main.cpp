#include <config.h>

// some global val we will need


typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY, ENDING} GameScreen;
int main() {
    // initialize win
    const Vector2 screenStartPos{ 0, 0 };
    const int screenWidth = 1280;
    const int screenHeight = 720;
    int fps = 60;

    Rectangle floor = {-2000, (screenHeight/2) + 300, 8000, 30};

    InitWindow(screenWidth, screenHeight, "Mockarria");
    ToggleFullscreen();

    GameScreen currentScreen = LOGO;
    Player player;
    World world;

    Camera2D camera = { 0 };
    camera.target = (Vector2){player.position.x + 20.0f, player.position.y + 20.0f};
    camera.offset = (Vector2){screenWidth/2.0f, screenHeight/2.0f};
    camera.zoom = 1.0f;

    int frameCounter = 0;
    SetTargetFPS(fps); // set target fps

    Image tile = LoadImage("../img/tiles/Platformer/Ground_06.png");
    ImageResize(&tile, 30, 30);
    Texture2D dirtTile = LoadTextureFromImage(tile);
    UnloadImage(tile);
    Texture2D healthUi = LoadTexture("../img/ui/Health_blank_x2.png");
    Texture2D loadScreen = LoadTexture("../img/backgrounds/Initial_load_screen.png");


    int currentFrameMove = 0;
    float currentFrameIdle = 0;

    int framesCounter = 0;
    int framesSpeed = 8;

    // Main game loop
    while (!WindowShouldClose()) // detect window closure
    {
        //Constantly updating stuff
        framesCounter++;
        if(currentScreen == GAMEPLAY) 
        {

            player.position.y += world.velocity;
            player.hitbox.y += world.velocity;
            player.hitbox.x = player.position.x + 22;
            if (world.velocity < world.velocityMax) world.velocity += world.acceleretion;
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

        if (IsKeyPressed(KEY_SPACE) && player.jumpCount > 0 && player.state == GROUND) world.velocity = player.jumpStrength, player.state = JUMPING, player.jumpCount--;

        // camera follow the player
        camera.target = {player.position.x + 20, player.position.y + 20};
        }

        // screen selector
        switch (currentScreen)
        {
        case LOGO:

            frameCounter++;
            if (frameCounter > 240)
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
                DrawTextureEx(loadScreen, screenStartPos, 0.0f, 0.6, WHITE);
                DrawText("LOGO SCREEN", 20, 20, 40, LIGHTGRAY);
            } break;
            case TITLE:
            {
                DrawTextureEx(loadScreen, screenStartPos, 0.0f, 0.6, WHITE);
                DrawText("TITLE SCREEN", 20, 20, 40, LIGHTGRAY);
            }break;
            case GAMEPLAY:
            {
                BeginMode2D(camera);


                    for (int i = 0; i <= world.worldSizeY; i++)
                    {
                        DrawLine(world.worldStartX,world.worldStartY + i * world.cellSize , world.worldEndX, world.worldStartY + i * world.cellSize, LIGHTGRAY);
                    }

                    for (int j = 0; j <= world.worldSizeX; j++)
                    {
                        DrawLine(world.worldStartX + j * world.cellSize,world.worldStartY, world.worldStartX + j * world.cellSize, world.worldEndY, LIGHTGRAY);
                    }


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
                DrawTexture(healthUi,10, 10, WHITE);

            }break;
            case ENDING:
            {
                DrawText("ENDING SCREEN", 20, 20, 40, DARKBLUE);
            }
            
            default:
                break;
            }
        
        EndDrawing();
    }

    // De-initialize

    CloseWindow();  //Close window and OpenGL context

    return 0;
    
}
