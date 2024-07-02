#include <config.h>

// some global val we will need

typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY, ENDING} GameScreen;
int main() {
    // initialize win

    const int screenWidth = 800;
    const int screenHeight = 450;
    int fps = 60;

    InitWindow(screenWidth, screenHeight, "Mockarria");

    GameScreen currentScreen = LOGO;
    Player player;

    Camera2D camera = { 0 };
    camera.target = (Vector2){player.x + 20.0f, player.y + 20.0f};
    camera.offset = (Vector2){screenWidth/2.0f, screenHeight/2.0f};
    camera.zoom = 1.0f;

    int frameCounter = 0;
    SetTargetFPS(fps); // set target fps

    // Main game loop
    while (!WindowShouldClose()) // detect window closure
    {
        // Update

        //player movement
        if (IsKeyDown(KEY_A)) player.x -= 2;
        else if (IsKeyDown(KEY_D)) player.x += 2;

        // camera follow the player
        camera.target = (Vector2){player.x + 20, player.y + 20};

        //camera zoom 
        camera.zoom += ((float)GetMouseWheelMove()*0.05f);
        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.1f) camera.zoom = 0.1f;

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
                DrawText("LOGO SCREEN", 20, 20, 40, LIGHTGRAY);
            } break;
            case TITLE:
            {
                DrawText("TITLE SCREEN", 20, 20, 40, LIGHTGRAY);
            }break;
            case GAMEPLAY:
            {
                BeginMode2D(camera);
                    DrawRectangle(-3000, 280, 6000, 1200, BLACK);
                    DrawRectangle(player.x, player.y, player.width, player.height, RED);
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
