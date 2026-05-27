#include "utils.h"

#define WIDTH 1200
#define HEIGHT 700

void f1()
{

    int offset = 400;
    int radius = 50;
    float angle = .3;
    Vector2 center = {.x = WIDTH * .5, .y = HEIGHT - radius};

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(center, radius, BLACK);
        center = (Vector2){.x = center.x, .y = ((sin(angle) / 2 + 1) * offset)};
        angle += .1;
        EndDrawing();
    }
}

void f2()
{
    int offset = 100;
    int radius = 50;
    float angle = .3;
    Vector2 center = {.x = WIDTH * .5, .y = HEIGHT * .5};

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(center, radius + ((sin(angle) / 2 + 1) * offset), BLACK);
        angle += .1;
        EndDrawing();
    }
}

#define NUM_BALLS 1

void f3()
{
    int radius = 200;
    float angle = .3;
    float slice = 0;
    float speed = 0.1;
    Vector2 balls[NUM_BALLS];

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        slice = 0;
        for (size_t i = 0; i < NUM_BALLS; i++)
        {
            DrawCircleV(balls[i], 10, BLACK);
            balls[i].x = cos(angle + slice) * radius + WIDTH * .5;
            balls[i].y = sin(angle + slice) * radius + HEIGHT * .5;
            slice += 2 * PI / NUM_BALLS;
        }
        angle += speed;
        EndDrawing();
    }
}

typedef struct Fly
{
    float xAngle;
    float yAngle;
    float xSpeed;
    float ySpeed;
    float radius;
    Vector2 pos;
} Fly;

#define FLY_COUNT 200

void f4()
{
    Fly flies[FLY_COUNT];

    Vector2 center = {WIDTH * .5, HEIGHT * .5};

    for (int i = 0; i < FLY_COUNT; i++)
    {
        flies[i].xAngle = GetRandomValue(0, 360) * DEG2RAD;
        flies[i].yAngle = GetRandomValue(0, 360) * DEG2RAD;

        flies[i].xSpeed = 0.02 + (float)GetRandomValue(0, 10) * 0.005;
        flies[i].ySpeed = 0.02 + (float)GetRandomValue(0, 10) * 0.005;

        flies[i].radius = 100 + GetRandomValue(0, 150);
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        Vector2 target = GetMousePosition();
        Vector2 v = Vector2MoveTowards(center, target, 4);

        for (int i = 0; i < FLY_COUNT; i++)
        {
            flies[i].xAngle += flies[i].xSpeed;
            flies[i].yAngle += flies[i].ySpeed;

            flies[i].pos.x = cosf(flies[i].xAngle) * flies[i].radius + v.x;
            flies[i].pos.y = sinf(flies[i].yAngle) * flies[i].radius + v.y;
        }
        ClearBackground(RAYWHITE);
        center = v;

        for (int i = 0; i < FLY_COUNT; i++)
        {
            DrawCircleV(flies[i].pos, 3, BLACK);
        }

        EndDrawing();
    }
}

int main(int argc, char const *argv[])
{
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WIDTH, HEIGHT, "Trigo");
    SetTargetFPS(40);
    f4();
    CloseWindow();
    return 0;
}
