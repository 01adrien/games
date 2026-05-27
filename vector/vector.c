#include "utils.h"

#include "utils.h"

#define WIDTH 1200
#define HEIGHT 700

Vector2 normalTop = {.x = 0, .y = -1};
Vector2 normalBottom = {.x = 0, .y = 1};
Vector2 normalLeft = {.x = -1, .y = 0};
Vector2 normalRight = {.x = 1, .y = 0};

// EXO 1 — Velocity Viewer
void f1()
{

    Vector2 velocity = {.x = 1, .y = 1};
    Vector2 center = {.x = 0, .y = 0};
    int radius = 40;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleLinesV(center, radius, BLACK);
        center = Vector2Add(center, velocity);

        DrawLineV(center, Vector2Add(center, velocity), RED);

        if (IsKeyDown(KEY_DOWN))
            velocity.y += 1;
        else if (IsKeyDown(KEY_UP))
            velocity.y -= 1;
        if (IsKeyDown(KEY_LEFT))
            velocity.x -= 1;
        else if (IsKeyDown(KEY_RIGHT))
            velocity.x += 1;
        else if (IsKeyDown(KEY_BACKSPACE))
        {
            center = (Vector2){.x = 0, .y = 0};
            velocity = (Vector2){.x = 1, .y = 1};
        }

        EndDrawing();
    }
}

// EXO 2 — Normalize
void f2()
{

    Vector2 direction = GetMousePosition();
    Vector2 center = {.x = WIDTH / 2, .y = HEIGHT / 2};
    int radius = 40;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleLinesV(center, radius, BLACK);

        direction = GetMousePosition();
        Vector2 normal = Vector2Scale(Vector2Normalize(Vector2Subtract(direction, center)), 50);

        Vector2 end = Vector2Add(center, normal);

        DrawLineV(center, end, RED);

        EndDrawing();
    }
}

// EXO 3 — Dot Product Visualizer
// (1, 0) → droite
// (0, 1) → haut
// (-1, 0) → gauche
// (0, -1) → bas
void f3()
{

    Vector2 normal = {.x = 1, .y = 0};
    Vector2 start = {.x = 0, .y = HEIGHT * .25};
    Vector2 end = Vector2Add(start, Vector2Scale(normal, WIDTH));

    Vector2 velocity = {.x = 0, .y = 0};
    Vector2 arrow = {.x = WIDTH / 2, .y = HEIGHT / 2};
    char dotProductStr[100];
    char dotProductAngleStr[100];
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawLineV(start, end, BLACK);
        arrow = Vector2Add(arrow, velocity);
        DrawLineV(arrow, Vector2Add(arrow, Vector2Scale(Vector2Normalize(velocity), 40)), BLACK);
        // dot = cos(angle entre les vecteurs)
        float dotProduct = Vector2DotProduct(velocity, normal);
        float dotProductAngle = Vector2DotProduct(Vector2Normalize(velocity), normal);
        snprintf(dotProductStr, sizeof(dotProductStr), "%f", dotProduct);
        DrawText(dotProductStr, 10, 10, 20, RED);
        if (IsKeyDown(KEY_DOWN))
            velocity.y += 1;
        else if (IsKeyDown(KEY_UP))
            velocity.y -= 1;
        if (IsKeyDown(KEY_LEFT))
            velocity.x -= 1;
        else if (IsKeyDown(KEY_RIGHT))
            velocity.x += 1;
        else if (IsKeyDown(KEY_BACKSPACE))
        {
            arrow = (Vector2){.x = WIDTH / 2, .y = HEIGHT / 2};
            velocity = (Vector2){.x = 1, .y = 1};
        }
        else
            velocity = Vector2Scale(velocity, 0.95);

        EndDrawing();
    }
}

int main(int argc, char const *argv[])
{
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WIDTH, HEIGHT, "Vector");
    SetTargetFPS(40);
    f3();
    CloseWindow();
    return 0;
}
