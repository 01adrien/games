#include "plugin.h"
#include "raycaster.h"

typedef struct rayInfo
{
    float length;
    int side;
    TileType tile;
    int textureOffset;
    Vector2 hit;
} RayInfo;

RayInfo buffer[PLAYER_SCREEN_WIDTH];

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
        index = MAP_HEIGHT - 1;
    else if (index > MAP_WIDTH)
        index = MAP_WIDTH - 1;
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

bool checkColisionPlayer(GameContext *ctx, Vector2 *vel)
{
    Rectangle tile;
    Player p = ctx->player;
    Vector2 nextMove = Vector2Add(p.pos, Vector2Scale(p.vel, p.radius + 20));
    TileIndex pTile = posToIndex(nextMove, ctx);

    if (getMapTile(pTile, ctx) > TILE_EMPTY)
    {
        // On teste l'axe X et Y pour voir si il y a possibilité de slider contre le mur
        Vector2 checkX = {.x = p.pos.x + p.vel.x * (p.radius + 8), .y = p.pos.y};
        Vector2 checkY = {.x = p.pos.x, .y = p.pos.y + p.vel.y * (p.radius + 8)};
        TileIndex pTileX = posToIndex(checkX, ctx);
        TileIndex pTileY = posToIndex(checkY, ctx);

        if (getMapTile(pTileX, ctx) == TILE_EMPTY)
            *vel = Vector2Normalize(Vector2Subtract(checkX, p.pos));
        else if (getMapTile(pTileY, ctx) == TILE_EMPTY)
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
        p->angle += PLAYER_ROTATION;

    else if (p->move.rot == ROT_LEFT)
        p->angle -= PLAYER_ROTATION;

    p->dir = Vector2Normalize((Vector2){cosf(p->angle), sinf(p->angle)});

    if (p->move.dir == DIR_NONE)
        return;

    Vector2 velSlide = {0, 0};
    if (checkColisionPlayer(ctx, &velSlide))
        p->vel = velSlide;

    p->pos = Vector2Add(p->pos, Vector2Scale(p->vel, PLAYER_SPEED));
}

bool hitWall(Vector2 startRay, Vector2 endRay, GameContext *ctx, TileType *tile)
{
    TileIndex current = posToIndex(endRay, ctx);
    Vector2 tilePos = indexToPos(current, ctx);

    TileIndex frontier = {
        .h = tilePos.y == endRay.y ? current.h - 1 : current.h,
        .w = tilePos.x == endRay.x ? current.w - 1 : current.w,
    };

    TileType t1 = getMapTile(current, ctx);
    TileType t2 = getMapTile(frontier, ctx);

    *tile = MAX(t1, t2);

    return t1 > TILE_EMPTY || t2 > TILE_EMPTY;
}

RayInfo raycast(GameContext *ctx, float angle)
{
    float len;
    int signX, signY;
    TileType tileX, tileY;
    Vector2 adjX, adjY, endX, endY, cornerSquare;
    Player *p = &ctx->player;
    TileIndex pTile = posToIndex(p->pos, ctx);

    // Raycast X
    signX = 1;
    cornerSquare = indexToPos(pTile, ctx);
    endX = p->pos;

    Vector2 dir = {cosf(angle), sinf(angle)};

    if (dir.x >= 0)
    {
        cornerSquare.x += TILE_SIZE;
        signX = -1;
    }

    while (!hitWall(p->pos, endX, ctx, &tileX))
    {
        adjX = (Vector2){.x = cornerSquare.x, .y = p->pos.y};
        len = Vector2Length(Vector2Subtract(p->pos, adjX));
        endX = (Vector2){.x = cornerSquare.x, .y = adjX.y - (tanf(angle * signX) * len)};
        cornerSquare.x += (-signX) * TILE_SIZE;
    }

    // Raycast Y

    signY = 1;
    cornerSquare = indexToPos(pTile, ctx);
    endY = p->pos;
    if (dir.y >= 0) // BOTTOM
    {
        cornerSquare.y += TILE_SIZE;
        signY = -1;
    }
    while (!hitWall(p->pos, endY, ctx, &tileY))
    {
        adjY = (Vector2){.x = p->pos.x, .y = cornerSquare.y};
        len = Vector2Length(Vector2Subtract(p->pos, adjY));
        endY = (Vector2){.x = adjY.x + (tan((angle + PI / 2) * signY) * len), .y = cornerSquare.y};
        cornerSquare.y += (-signY) * TILE_SIZE;
    }

    float distX = Vector2Length(Vector2Subtract(endX, p->pos));
    float distY = Vector2Length(Vector2Subtract(endY, p->pos));
    float dist = MIN(distX, distY);

    return (RayInfo){
        .length = dist,
        .side = distX > distY ? 1 : -1,
        .tile = distX > distY ? tileY : tileX,
        .textureOffset = distX > distY ? -((int)(endY.x) % 64) : (int)(endX.y) % 64,
        .hit = distX > distY ? endY : endX,
    };
}

void draw(GameContext *ctx)
{
    Player p = ctx->player;

    // DRAW 3D
    memset(&buffer, 0, PLAYER_SCREEN_WIDTH);
    float start = p.angle - (PLAYER_FOV * 0.5f);
    Vector2 corner = {.x = p.screen.x + 1, .y = p.screen.y};

    // FLOOR & CEILLING
    DrawRectangle(0, 0, PLAYER_SCREEN_WIDTH, PLAYER_SCREEN_HEIGHT / 2, BROWN);
    DrawRectangle(0, PLAYER_SCREEN_HEIGHT / 2, PLAYER_SCREEN_WIDTH, PLAYER_SCREEN_HEIGHT / 2, GRAY);

    for (int x = 0; x < PLAYER_SCREEN_WIDTH; x++)
    {
        float angle = start + ((PLAYER_FOV * x) / PLAYER_SCREEN_WIDTH);
        RayInfo ray = raycast(ctx, angle);
        float h = MIN(80000 / ray.length, PLAYER_SCREEN_HEIGHT);
        float gap = (PLAYER_SCREEN_HEIGHT - h) / 2;
        Vector2 start = {.x = corner.x + x, .y = corner.y + gap};
        Vector2 end = {.x = start.x, .y = start.y + h};
        Rectangle src = {.x = ray.textureOffset, .y = 0, .width = 1, .height = 64};
        Rectangle dst = {.x = start.x, .y = start.y, .width = 1, .height = h};
        Color c = ray.side == -1 ? WALL_SHADOW : WALL_LIGHT;
        DrawTexturePro(ctx->textures[ray.tile], src, dst, (Vector2){0, 0}, 0, c);
        buffer[x] = ray;
    }

    // DRAW MINI MAP
    int x, y;
    for (size_t i = 0; i < MAP_HEIGHT; i++)
    {
        for (size_t j = 0; j < MAP_WIDTH; j++)
        {
            y = (i * TILE_SIZE + ctx->mapPos.y) * MAP_SCALE;
            x = (j * TILE_SIZE + ctx->mapPos.x) * MAP_SCALE;

            TileType tile = (TileType)ctx->map[i * MAP_WIDTH + j];

            DrawRectangle(x, y, TILE_SIZE * MAP_SCALE, TILE_SIZE * MAP_SCALE, tile > 0 ? BLACK : RAYWHITE);
        }
    }

    // DRAW PLAYER
    for (size_t i = 0; i < PLAYER_SCREEN_WIDTH; i++)
        DrawLineV(Vector2Scale(p.pos, MAP_SCALE), Vector2Scale(buffer[i].hit, MAP_SCALE), FOV_COLOR);

    DrawCircleV(Vector2Scale(p.pos, MAP_SCALE), PLAYER_RADIUS * MAP_SCALE, Fade(RED, .6));
}

void gameLoop(GameContext *ctx)
{

    handleInput(ctx);
    movePlayer(ctx);
    draw(ctx);
}

int Draw(void *ctx)
{
    gameLoop((GameContext *)ctx);
}

int Update(void *ctx, float dt) {}