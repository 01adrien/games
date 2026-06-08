#include "plugin.h"
#include "raycaster.h"

// #define DEBUG

void handleInput(GameContext *ctx)
{
    Player *p = &ctx->player;
    p->move.dir = DIR_NONE;
    p->move.rot = ROT_NONE;

    if (IsKeyDown(KEY_UP))
        p->move.dir = DIR_UP;
    if (IsKeyDown(KEY_DOWN))
        p->move.dir = DIR_DOWN;
    if (IsKeyDown(KEY_LEFT))
        p->move.rot = ROT_LEFT;
    if (IsKeyDown(KEY_RIGHT))
        p->move.rot = ROT_RIGHT;
}

int posToIndex(float p, float mapCorner)
{
    return (int)((p - mapCorner) / TILE_SIZE);
}

float indexToPos(int n, float mapCorner)
{
    return n * TILE_SIZE + mapCorner;
}

TileType getMapTile(int h, int w, GameContext *ctx)
{
    return (TileType)ctx->map[h * MAP_WIDTH + w];
}

bool checkColisionPlayer(GameContext *ctx, Vector2 *vel)
{
    int w, h, w1, h1, w2, h2;
    Rectangle tile;
    Player p = ctx->player;
    float newX = p.pos.x + p.vel.x * (p.radius + 10);
    float newY = p.pos.y + p.vel.y * (p.radius + 10);
    Vector2 checkX = {.x = newX, .y = p.pos.y};
    Vector2 checkY = {.x = p.pos.x, .y = newY};
    Vector2 center = Vector2Add(p.pos, Vector2Scale(p.vel, p.radius + 10));

    w = posToIndex(center.x, ctx->mapPos.x);
    h = posToIndex(center.y, ctx->mapPos.y);

    w1 = posToIndex(checkX.x, ctx->mapPos.x);
    h1 = posToIndex(checkX.y, ctx->mapPos.y);

    w2 = posToIndex(checkY.x, ctx->mapPos.x);
    h2 = posToIndex(checkY.y, ctx->mapPos.y);

    TileType playerTile = getMapTile(h, w, ctx);

    if (playerTile == TILE_WALL)
    {
#ifdef DEBUG
        DrawRectangle(indexToPos(w, ctx->mapPos.x), indexToPos(h, ctx->mapPos.y), TILE_SIZE, TILE_SIZE, RED);
        DrawRectangle(indexToPos(w1, ctx->mapPos.x), indexToPos(h1, ctx->mapPos.y), TILE_SIZE, TILE_SIZE, GREEN);
        DrawRectangle(indexToPos(w2, ctx->mapPos.x), indexToPos(h2, ctx->mapPos.y), TILE_SIZE, TILE_SIZE, YELLOW);
#endif
        if (getMapTile(h1, w1, ctx) != TILE_WALL)
            *vel = Vector2Normalize(Vector2Subtract(checkX, p.pos));
        else if (getMapTile(h2, w2, ctx) != TILE_WALL)
            *vel = Vector2Normalize(Vector2Subtract(checkY, p.pos));
        return true;
    }

    return false;
}

void movePlayer(GameContext *ctx)
{
    Player *p = &ctx->player;

    p->vel = (Vector2){0, 0};

    if (p->move.dir == DIR_UP)
        p->vel = p->dir;

    else if (p->move.dir == DIR_DOWN)
        p->vel = Vector2Negate(p->dir);

    if (p->move.rot == ROT_RIGHT)
        p->angle += 0.1;

    else if (p->move.rot == ROT_LEFT)
        p->angle -= 0.1;

    p->dir = Vector2Normalize((Vector2){cosf(p->angle), sinf(p->angle)});

    if (p->move.dir == DIR_NONE)
        return;

    Vector2 velOneWay = {0, 0};
    if (checkColisionPlayer(ctx, &velOneWay))
        p->vel = velOneWay;

    p->pos = Vector2Add(p->pos, Vector2Scale(p->vel, PLAYER_SPEED));
}

// DRAW FUNCTION

void drawTile(TileType type, int x, int y)
{
    switch (type)
    {
    case TILE_EMPTY:
        DrawRectangleLines(x, y, TILE_SIZE, TILE_SIZE, RED);
        break;
    case TILE_WALL:
        DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, BLACK);
    default:
        break;
    }
}

void drawMap(GameContext *ctx)
{
    int x, y;

    for (size_t i = 0; i < MAP_HEIGHT; i++)
    {
        for (size_t j = 0; j < MAP_WIDTH; j++)
        {
            y = i * TILE_SIZE + ctx->mapPos.y;
            x = j * TILE_SIZE + ctx->mapPos.x;
            drawTile((TileType)ctx->map[i * MAP_WIDTH + j], x, y);
        }
    }
}

void drawPlayer(GameContext *ctx)
{
    Player p = ctx->player;
    int radius = PLAYER_RADIUS;
    float angle = 2.4;

    Vector2 sides[3] = {
        {.x = cosf(p.angle) * radius + p.pos.x,
         .y = sinf(p.angle) * radius + p.pos.y},
        {.x = cosf(p.angle - angle) * radius + p.pos.x,
         .y = sinf(p.angle - angle) * radius + p.pos.y},
        {.x = cosf(p.angle + angle) * radius + p.pos.x,
         .y = sinf(p.angle + angle) * radius + p.pos.y},
    };

    // DrawTriangle(sides[0], sides[1], sides[2], BLUE);
    DrawLineEx(p.pos, Vector2Add(p.pos, Vector2Scale(p.dir, p.radius * 1.5)), 5, BLACK);
    DrawCircleV(p.pos, radius, BLUE);
}

void gameLoop(GameContext *ctx)
{
    handleInput(ctx);
    drawMap(ctx);
    drawPlayer(ctx);
    movePlayer(ctx);
}

int Draw(void *ctx)
{
    gameLoop((GameContext *)ctx);
}
