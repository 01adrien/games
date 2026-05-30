#if !defined(PLUGIN_H)
#define PLUGIN_H

#define PLUGIN_API __attribute__((visibility("default")))

#include "hotreload.h"

typedef struct ball
{
    Vector2 pos;
    Vector2 vel;
    int radius;
} Ball;

typedef struct wall
{
    Vector2 start, end, normal;
} Wall;

typedef struct f4Context
{
    int width, height;
    Vector2 normal;
    Vector2 start;
    Vector2 end;
    Vector2 velocity;
    Vector2 startArrow;
    char dotProductStr[100];
} F4Context;

typedef struct f5Context
{
    int width, height;
    Vector2 normal;
    Ball ball;
    Wall walls[4];
} F5Context;

typedef struct f6Context
{
    int width, height, selectedBall;
    Ball balls[2];
    Wall walls[4];
} F6Context;

void drawF4(F4Context *ctx);
void drawF5(F5Context *ctx);
void drawF6(F6Context *ctx);

PLUGIN_API int Draw(void *ctx);

#endif // PLUGIN_H