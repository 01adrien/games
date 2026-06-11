#include "plugin.h"
#include "raycaster.h"

#define DEBUG

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

int _posToIndex(float p, float mapCorner)
{
    int index = (int)((p - mapCorner) / TILE_SIZE);
    if (index < 0)
        index = 0;
    else if (index > MAP_HEIGHT)
    {
        index = MAP_HEIGHT;
    }
    else if (index > MAP_WIDTH)
    {
        index = MAP_WIDTH;
    }

    return index;
}

float _indexToPos(int n, float mapCorner)
{
    return n * TILE_SIZE + mapCorner;
}

Vector2 indexToPos(TileIndex index, GameContext *ctx)
{
    return (Vector2){.x = _indexToPos(index.w, ctx->mapPos.x), .y = _indexToPos(index.h, ctx->mapPos.y)};
}

TileIndex posToIndex(Vector2 pos, GameContext *ctx)
{
    return (TileIndex){.w = _posToIndex(pos.x, ctx->mapPos.x), .h = _posToIndex(pos.y, ctx->mapPos.y)};
}

TileType getMapTile(TileIndex index, GameContext *ctx)
{
    return (TileType)ctx->map[index.h * MAP_WIDTH + index.w];
}

VectorDirection vectorDir(Vector2 vector)
{
    VectorDirection direction;
    if (vector.x)
    {
        /* code */
    }

    return direction;
}

bool checkColisionPlayer(GameContext *ctx, Vector2 *vel)
{
    int w, h, w1, h1, w2, h2, wp, hp;
    Rectangle tile;
    Player p = ctx->player;
    Vector2 nextMove = Vector2Add(p.pos, Vector2Scale(p.vel, p.radius + 10));
    TileIndex pTile = posToIndex(nextMove, ctx);

    if (getMapTile(pTile, ctx) == TILE_WALL)
    {
        // On teste l'axe X et Y pour voir si il y a possibilité de slider contre le mur
        Vector2 checkX = {.x = p.pos.x + p.vel.x * (p.radius + 10), .y = p.pos.y};
        Vector2 checkY = {.x = p.pos.x, .y = p.pos.y + p.vel.y * (p.radius + 10)};
        TileIndex pTileX = posToIndex(checkX, ctx);
        TileIndex pTileY = posToIndex(checkY, ctx);

#ifdef DEBUG
        Vector2 tileSize = {TILE_SIZE, TILE_SIZE};
        DrawRectangleV(indexToPos(pTile, ctx), tileSize, RED);
        DrawRectangleV(indexToPos(pTileX, ctx), tileSize, GREEN);
        DrawRectangleV(indexToPos(pTileY, ctx), tileSize, YELLOW);
#endif

        if (getMapTile(pTileX, ctx) != TILE_WALL)
            *vel = Vector2Normalize(Vector2Subtract(checkX, p.pos));
        else if (getMapTile(pTileY, ctx) != TILE_WALL)
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

    Vector2 velSlide = {0, 0};
    if (checkColisionPlayer(ctx, &velSlide))
        p->vel = velSlide;

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
    DrawLineEx(p.pos, Vector2Add(p.pos, Vector2Scale(p.dir, p.radius * 1.5)), 5, BLACK);
    DrawCircleV(p.pos, radius, BLUE);
}

void rayCasting(GameContext *ctx)
{
    float len;
    int sign;
    Vector2 adjX, adjY, endX, endY, cornerSquare;
    Player *p = &ctx->player;
    TileIndex pTile = posToIndex(p->pos, ctx);

    // Raycast X
    sign = 1;
    cornerSquare = indexToPos(pTile, ctx);
    endX = p->pos;
    if (p->dir.x >= 0)
    {
        cornerSquare.x += TILE_SIZE;
        sign = -1;
    }

    while (getMapTile(posToIndex(endX, ctx), ctx) != TILE_WALL)
    {
        adjX = (Vector2){.x = cornerSquare.x, .y = p->pos.y};
        len = Vector2Length(Vector2Subtract(p->pos, adjX));
        endX = (Vector2){.x = cornerSquare.x, .y = adjX.y - (tanf(p->angle * sign) * len)};
        cornerSquare.x += (-sign) * TILE_SIZE;
    }

    // Raycast Y
    sign = 1;
    cornerSquare = indexToPos(pTile, ctx);
    endY = p->pos;
    if (p->dir.y >= 0) // BOTTOM
    {
        cornerSquare.y += TILE_SIZE;
        sign = -1;
    }
    while (getMapTile(posToIndex(endY, ctx), ctx) != TILE_WALL)
    {
        adjY = (Vector2){.x = p->pos.x, .y = cornerSquare.y};
        len = Vector2Length(Vector2Subtract(p->pos, adjY));
        endY = (Vector2){.x = adjY.x + (tan((p->angle + PI / 2) * sign) * len), .y = cornerSquare.y};
        cornerSquare.y += (-sign) * TILE_SIZE;
    }

    float distY = Vector2Length(Vector2Subtract(endY, p->pos));
    float distX = Vector2Length(Vector2Subtract(endX, p->pos));

    DrawLineV(p->pos, distX > distY ? endY : endX, BLUE);
}

void gameLoop(GameContext *ctx)
{
    handleInput(ctx);
    drawMap(ctx);
    movePlayer(ctx);
    rayCasting(ctx);
    drawPlayer(ctx);
}

int Draw(void *ctx)
{
    gameLoop((GameContext *)ctx);
}

int Update(void *ctx, float dt) {}