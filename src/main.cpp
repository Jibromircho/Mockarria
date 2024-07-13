#include <config.h>

// some global val we will need


typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY, ENDING} GameScreen;
int main() {
    // initialize win

    const int screenWidth = 800;
    const int screenHeight = 450;
    int fps = 60;

    //some stats
    int velocity = 2;

    InitWindow(screenWidth, screenHeight, "Mockarria");

    GameScreen currentScreen = LOGO;
    Player player;

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


    int currentFrameMove = 0;
    int currentFrameIdle = 0;
    int currentFrameIdleRow = 0;

    int framesCounter = 0;
    int framesSpeed = 8;

    // Main game loop
    while (!WindowShouldClose()) // detect window closure
    {
        // Update
        framesCounter++;
        while(player.position.y < screenHeight/3) player.position.y += velocity;

        if (framesCounter >= (60/framesSpeed))
        {
            framesCounter = 0;
            currentFrameMove++;
            currentFrameIdle++;

            if (currentFrameMove > 7) currentFrameMove = 0;
            if (currentFrameIdle > 11) currentFrameIdle = 0;
    

            player.frameRecMove.x = (float)currentFrameMove*(float)player.model_move.width/8;
            player.frameRecIdle.x = (float)currentFrameIdle*(float)player.model_idle.width/12;
            if(IsKeyDown(KEY_D)) currentFrameIdle = 0, player.frameRecMove.y = player.model_move.height/2;
            if(IsKeyDown(KEY_A)) currentFrameIdle = 0, player.frameRecMove.y = player.model_move.height;
        }

        //player movement
        if (IsKeyDown(KEY_A)) player.position.x -= 2;
        else if (IsKeyDown(KEY_D)) player.position.x += 2;
        if (IsKeyDown(KEY_SPACE)) player.position.y -= velocity;
        else if (IsKeyReleased(KEY_SPACE)) player.position.y += velocity/2;

        // camera follow the player
        camera.target = {player.position.x + 20, player.position.y + 20};

        //camera zoom 
        camera.zoom += ((float)GetMouseWheelMove()*0.05f);
        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.1f) camera.zoom = 0.1f;

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
                DrawText("LOGO SCREEN", 20, 20, 40, LIGHTGRAY);
            } break;
            case TITLE:
            {
                DrawText("TITLE SCREEN", 20, 20, 40, LIGHTGRAY);
            }break;
            case GAMEPLAY:
            {
                BeginMode2D(camera);
                    for (int i = 0; i < 25; i++){
                    DrawTexture(dirtTile, 0 + dirtTile.width * i, screenHeight/2 + dirtTile.height + 70, WHITE);
                    }
                    if(IsKeyDown(KEY_A)) DrawTextureRec(player.model_move, player.frameRecMove, player.position, WHITE);
                    else if(IsKeyDown(KEY_D)) DrawTextureRec(player.model_move, player.frameRecMove, player.position, WHITE);
                    else if(IsKeyUp(KEY_A && KEY_D)) DrawTextureRec(player.model_idle, player.frameRecIdle, player.position, WHITE);
                EndMode2D();

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
