#include "prelude.hpp"

int main() {
    int screenWidth = 800;
    int screenHeight = 450;

    raylib::Window window(screenWidth, screenHeight, "raylib-cpp - basic window");

    SetTargetFPS(60);

    while (!window.ShouldClose())
    {
        BeginDrawing();

        window.ClearBackground(RAYWHITE);

        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    // UnloadTexture() and CloseWindow() are called automatically.

    return 0;
}
