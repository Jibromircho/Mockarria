#include <config.h>

// some global val we will need

typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY, ENDING} GameScreen;
int main() {
    // initialize win

    const int screenWidth = 800;
    const int screenHeight = 450;
    const int fps = 60;

    InitWindow(screenWidth, screenHeight, "Mockarria");

    GameScreen currentScreen = LOGO;

    int frameCounter = 0;
    SetTargetFPS(fps); // set target fps

    Player playerChar;
    // Main game loop
    while (!WindowShouldClose()) // detect window closure
    {
        // Update
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
                DrawRectangle(screenWidth/2, screenHeight/2, playerChar.width, playerChar.height, BLACK);
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
