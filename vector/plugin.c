#include "plugin.h"

int Draw(void *ctx)
{
    drawF6((F6Context *)ctx);
}

void drawF4(F4Context *ctx)
{
    Vector2 endArrow = Vector2Add(ctx->startArrow, ctx->velocity);
    float dotProduct = Vector2DotProduct(ctx->velocity, ctx->normal);
    Vector2 endProjection = Vector2Add(ctx->startArrow, Vector2Scale(ctx->normal, dotProduct));
    Vector2 projectionV = Vector2Subtract(endProjection, ctx->startArrow);
    Vector2 rest = Vector2Subtract(ctx->velocity, projectionV);
    Vector2 endRest = Vector2Add(ctx->startArrow, rest);
    snprintf(ctx->dotProductStr, sizeof(ctx->dotProductStr), "%f", dotProduct);
    // Normal ----->
    DrawLineV(ctx->start, ctx->end, BLACK);
    DrawLineV(ctx->startArrow, endArrow, BLACK);
    // Projection sur la normal
    DrawLineV(ctx->startArrow, endProjection, RED);
    DrawText(ctx->dotProductStr, 10, 10, 20, RED);
    // rest
    DrawLineV(ctx->startArrow, endRest, GREEN);

    // INPUT HANDLER

    if (IsKeyDown(KEY_DOWN))
        ctx->velocity.y += 1;
    else if (IsKeyDown(KEY_UP))
        ctx->velocity.y -= 1;
    if (IsKeyDown(KEY_LEFT))
        ctx->velocity.x -= 1;
    else if (IsKeyDown(KEY_RIGHT))
        ctx->velocity.x += 1;
    else if (IsKeyDown(KEY_BACKSPACE))
    {
        ctx->startArrow = (Vector2){.x = ctx->width / 2, .y = ctx->width / 2};
        ctx->velocity = (Vector2){.x = 1, .y = 1};
    }
    else
        ctx->velocity = Vector2Scale(ctx->velocity, 0.95);

    ctx->startArrow = Vector2Add(ctx->startArrow, ctx->velocity);
}

void drawF5(F5Context *ctx)
{
    // DRAW
    DrawCircleV(ctx->ball.pos, ctx->ball.radius, BLACK);
    for (size_t i = 0; i < 4; i++)
        DrawLineV(ctx->walls[i].start, ctx->walls[i].end, RED);

    // INPUT HANDLER

    if (IsKeyDown(KEY_DOWN))
        ctx->ball.vel.y += 1;
    else if (IsKeyDown(KEY_UP))
        ctx->ball.vel.y -= 1;
    if (IsKeyDown(KEY_LEFT))
        ctx->ball.vel.x -= 1;
    else if (IsKeyDown(KEY_RIGHT))
        ctx->ball.vel.x += 1;
    else
        ctx->ball.vel = Vector2Scale(ctx->ball.vel, 0.95);

    // COLISSION HANDLER
    for (size_t i = 0; i < 4; i++)
    {
        if (CheckCollisionCircleLine(
                ctx->ball.pos, ctx->ball.radius, ctx->walls[i].start, ctx->walls[i].end))
        {
            ctx->ball.vel = Vector2Reflect(ctx->ball.vel, ctx->walls[i].normal);
        }
    }

    ctx->ball.pos = Vector2Add(ctx->ball.pos, ctx->ball.vel);
}

void drawF6(F6Context *ctx)
{
    DrawCircleV(ctx->balls[0].pos, ctx->balls[0].radius, GREEN);
    DrawCircleV(ctx->balls[1].pos, ctx->balls[1].radius, BLUE);
    DrawLineV(ctx->balls[0].pos, Vector2Add(ctx->balls[0].pos, ctx->balls[0].vel), BLACK);
    DrawLineV(ctx->balls[1].pos, Vector2Add(ctx->balls[1].pos, ctx->balls[1].vel), BLACK);

    DrawCircleLinesV(ctx->balls[ctx->selectedBall].pos, ctx->balls[ctx->selectedBall].radius, RED);

    for (size_t i = 0; i < 4; i++)
        DrawLineV(ctx->walls[i].start, ctx->walls[i].end, RED);

    if (IsKeyDown(KEY_DOWN))
        ctx->balls[ctx->selectedBall].vel.y += 1;
    else if (IsKeyDown(KEY_UP))
        ctx->balls[ctx->selectedBall].vel.y -= 1;
    if (IsKeyDown(KEY_LEFT))
        ctx->balls[ctx->selectedBall].vel.x -= 1;
    else if (IsKeyDown(KEY_RIGHT))
        ctx->balls[ctx->selectedBall].vel.x += 1;
    else if (IsKeyDown(KEY_BACKSPACE))
        ctx->selectedBall = ctx->selectedBall == 1 ? 0 : 1;
    else if (IsKeyDown(KEY_SPACE))
        ctx->balls[ctx->selectedBall].vel = (Vector2){0, 0};
    else if (IsKeyDown(KEY_R))
    {
        ctx->balls[1].pos = (Vector2){ctx->width / 2 + 50, ctx->height / 2 + 50};
        ctx->balls[0].pos = (Vector2){ctx->width / 2 - 50, ctx->height / 2 - 50};
    }

    // COLISSION HANDLER
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 2; j++)
        {
            if (CheckCollisionCircleLine(
                    ctx->balls[j].pos, ctx->balls[j].radius, ctx->walls[i].start, ctx->walls[i].end))
            {
                ctx->balls[j].vel = Vector2Reflect(ctx->balls[j].vel, ctx->walls[i].normal);
            }
        }
    }

    if (CheckCollisionCircles(
            ctx->balls[0].pos, ctx->balls[0].radius, ctx->balls[1].pos, ctx->balls[1].radius))
    {
        Vector2 impact = Vector2Subtract(ctx->balls[1].pos, ctx->balls[0].pos);
        DrawLineV(ctx->balls[0].pos, Vector2Add(ctx->balls[0].pos, impact), RED);
        int distance = Vector2Length(impact);
        Vector2 normalImpact = Vector2Normalize(impact);
        int overlap = (ctx->balls[0].radius + ctx->balls[1].radius) - distance;
        if (overlap)
        {
            ctx->balls[0].pos = Vector2Subtract(ctx->balls[0].pos, Vector2Scale(normalImpact, overlap * 0.5f));
            ctx->balls[1].pos = Vector2Add(ctx->balls[1].pos, Vector2Scale(normalImpact, overlap * 0.5f));
        }

        Vector2 relativeVelocity = Vector2Subtract(ctx->balls[0].vel, ctx->balls[1].vel);

        float impulse = Vector2DotProduct(relativeVelocity, normalImpact);

        if (impulse <= 0)
            return;

        ctx->balls[0].vel = Vector2Subtract(ctx->balls[0].vel, Vector2Scale(normalImpact, impulse));
        ctx->balls[1].vel = Vector2Add(ctx->balls[1].vel, Vector2Scale(normalImpact, impulse));
    }

    ctx->balls[0].pos = Vector2Add(ctx->balls[0].pos, ctx->balls[0].vel);
    ctx->balls[1].pos = Vector2Add(ctx->balls[1].pos, ctx->balls[1].vel);
}