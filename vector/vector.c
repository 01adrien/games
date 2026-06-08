#include "plugin.h"
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
    Vector2 startArrow = {.x = WIDTH / 2, .y = HEIGHT / 2};
    char dotProductStr[100];
    char dotProductAngleStr[100];
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawLineV(start, end, BLACK);
        startArrow = Vector2Add(startArrow, velocity);
        DrawLineV(startArrow, Vector2Add(startArrow, Vector2Scale(Vector2Normalize(velocity), 40)), BLACK);
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
            startArrow = (Vector2){.x = WIDTH / 2, .y = HEIGHT / 2};
            velocity = (Vector2){.x = 1, .y = 1};
        }
        else
            velocity = Vector2Scale(velocity, 0.95);

        EndDrawing();
    }
}

// EXO 4 — PROJECTION (ombre d’un vecteur)
// velocity = projection + reste
// reste = velocity - projection
// dot = cos(angle entre les vecteurs)
//

F4Context initF4()
{

    Vector2 normal = {.x = 1, .y = 0};
    Vector2 start = {.x = 0, .y = HEIGHT * .25};
    Vector2 end = Vector2Add(start, Vector2Scale(normal, WIDTH));
    Vector2 velocity = {.x = 0, .y = 0};
    Vector2 startArrow = {.x = WIDTH / 2, .y = HEIGHT / 2};

    return (F4Context){
        .normal = normal,
        .start = start,
        .end = end,
        .velocity = velocity,
        .startArrow = startArrow,
        .width = WIDTH,
        .height = HEIGHT,
    };
}

F5Context initF5()
{
    Vector2 normal = {.x = 1, .y = 0};
    Ball ball = {
        .pos = {.x = WIDTH / 2, .y = HEIGHT / 2},
        .radius = 20,
        .vel = {.x = 0, .y = 0},
    };

    int p = 50;

    return (F5Context){
        .normal = normal,
        .ball = ball,
        .walls = {
            {.start = {.x = p, .y = p}, .end = {.x = p, .y = HEIGHT - p}, .normal = {.x = 1, .y = 0}},
            {.start = {.x = p, .y = p}, .end = {.x = WIDTH - p, .y = p}, .normal = {.x = 0, .y = 1}},
            {.start = {.x = WIDTH - p, .y = p}, .end = {.x = WIDTH - p, .y = HEIGHT - p}, .normal = {.x = -1, .y = 0}},
            {.start = {.x = p, .y = HEIGHT - p}, .end = {.x = WIDTH - p, .y = HEIGHT - p}, .normal = {.x = 0, .y = -1}},
        },
        .width = WIDTH,
        .height = HEIGHT,
    };
}

F6Context initF6()
{

    int p = 50;
    return (F6Context){
        .width = WIDTH,
        .height = HEIGHT,
        .selectedBall = 0,
        .balls = {
            {
                .pos = (Vector2){.x = WIDTH / 2 - 50, .y = HEIGHT / 2 - 50},
                .radius = 40,
                .vel = {.x = 0, .y = 0},
            },
            {
                .pos = (Vector2){.x = WIDTH / 2 + 50, .y = HEIGHT / 2 + 50},
                .radius = 20,
                .vel = {.x = 0, .y = 0},
            },
        },
        .walls = {
            {.start = {.x = p, .y = p}, .end = {.x = p, .y = HEIGHT - p}, .normal = {.x = 1, .y = 0}},
            {.start = {.x = p, .y = p}, .end = {.x = WIDTH - p, .y = p}, .normal = {.x = 0, .y = 1}},
            {.start = {.x = WIDTH - p, .y = p}, .end = {.x = WIDTH - p, .y = HEIGHT - p}, .normal = {.x = -1, .y = 0}},
            {.start = {.x = p, .y = HEIGHT - p}, .end = {.x = WIDTH - p, .y = HEIGHT - p}, .normal = {.x = 0, .y = -1}},
        },
    };
}

int main(int argc, char const *argv[])
{
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WIDTH, HEIGHT, "Vector");
    SetTargetFPS(40);

    void *plugin_handle = NULL;
    DrawFuncPtr Draw = NULL;
    const char *file = "../build/vector_plugin.so";
    long lastPluginTime = 0;

    if (reload_plugin(file, &lastPluginTime, &Draw, &plugin_handle))
        return 1;

    F4Context ctxF4 = initF4();
    F5Context ctxF5 = initF5();
    F6Context ctxF6 = initF6();

    while (!WindowShouldClose())
    {
        if (should_reload(file, &lastPluginTime))
        {
            if (reload_plugin(file, &lastPluginTime, &Draw, &plugin_handle))
            {
                break;
            }
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        Draw(&ctxF6);
        EndDrawing();
    }

    if (plugin_handle)
        dlclose(plugin_handle);

    return 0;
}
