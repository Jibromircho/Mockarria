#include <config.h>


int main() {
    // initialize win

    const int screenWidth = 800;
    const int screenHeight = 450;
    const int fps = 60;

    InitWindow(screenWidth, screenHeight, "Mockarria");

    SetTargetFPS(fps); // set target fps

    // Main game loop
    while (!WindowShouldClose()) // detect window closure
    {
        // Update


        // Draw

        BeginDrawing();

            ClearBackground(RAYWHITE);
            DrawText("Game succesfully init", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    // De-initialize

    CloseWindow();  //Close window and OpenGL context

    return 0;
    
}
