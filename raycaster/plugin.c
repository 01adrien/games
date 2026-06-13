#include "plugin.h"
#include "raycaster.h"

typedef struct rayInfo
{
    float length;
    int side;
} RayInfo;

RayInfo buffer[PLAYER_SCREEN_WIDTH] = {0};

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
    int w, h, w1, h1, w2, h2, wp, hp;
    Rectangle tile;
    Player p = ctx->player;
    Vector2 nextMove = Vector2Add(p.pos, Vector2Scale(p.vel, p.radius + 8));
    TileIndex pTile = posToIndex(nextMove, ctx);

    if (getMapTile(pTile, ctx) > TILE_EMPTY)
    {
        // On teste l'axe X et Y pour voir si il y a possibilité de slider contre le mur
        Vector2 checkX = {.x = p.pos.x + p.vel.x * (p.radius + 8), .y = p.pos.y};
        Vector2 checkY = {.x = p.pos.x, .y = p.pos.y + p.vel.y * (p.radius + 8)};
        TileIndex pTileX = posToIndex(checkX, ctx);
        TileIndex pTileY = posToIndex(checkY, ctx);

#ifdef DEBUG
        Vector2 tileSize = {TILE_SIZE, TILE_SIZE};
        DrawRectangleV(indexToPos(pTile, ctx), tileSize, RED);
        DrawRectangleV(indexToPos(pTileX, ctx), tileSize, GREEN);
        DrawRectangleV(indexToPos(pTileY, ctx), tileSize, YELLOW);
#endif

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
        p->angle += 0.15;

    else if (p->move.rot == ROT_LEFT)
        p->angle -= 0.15;

    p->dir = Vector2Normalize((Vector2){cosf(p->angle), sinf(p->angle)});

    if (p->move.dir == DIR_NONE)
        return;

    Vector2 velSlide = {0, 0};
    if (checkColisionPlayer(ctx, &velSlide))
        p->vel = velSlide;

    p->pos = Vector2Add(p->pos, Vector2Scale(p->vel, PLAYER_SPEED));
}

void drawTile(TileType type, int x, int y)
{
    switch (type)
    {
    case TILE_EMPTY:
        DrawRectangleLines(x, y, TILE_SIZE, TILE_SIZE, RED);
        break;
    default:
        DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, BLACK);
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
    DrawCircleV(p.pos, radius, BLACK);
}

void draw3D(GameContext *ctx)
{
    Rectangle screen = ctx->player.screen;
    Vector2 corner = {.x = screen.x + 1, .y = screen.y};
    // DrawRectangleRec(screen, Fade(BLUE, .4));
    // DrawRectangle(screen.x, screen.y + screen.height / 2, screen.width, screen.height / 2, Fade(WHITE, 1));
    for (size_t i = 0; i < PLAYER_SCREEN_WIDTH; i++)
    {
        RayInfo ray = buffer[i];
        float h = MIN(8000 / ray.length, PLAYER_SCREEN_HEIGHT);
        float gap = (PLAYER_SCREEN_HEIGHT - h) / 2;
        Vector2 start = {.x = corner.x + i, .y = corner.y + gap};
        Vector2 end = {.x = start.x, .y = start.y + h};
        DrawLineV(start, end, Fade(BROWN, ray.side == -1 ? 1 : .8));
    }
    DrawRectangleLines(screen.x, screen.y, screen.width, screen.height, BLACK);
}

bool hitWall(Vector2 startRay, Vector2 endRay, GameContext *ctx)
{
    TileIndex current = posToIndex(endRay, ctx);
    Vector2 tilePos = indexToPos(current, ctx);

    TileIndex frontier = {
        .h = tilePos.y == endRay.y ? current.h - 1 : current.h,
        .w = tilePos.x == endRay.x ? current.w - 1 : current.w,
    };

    return getMapTile(current, ctx) > TILE_EMPTY || getMapTile(frontier, ctx) > TILE_EMPTY;
}

RayInfo raycast(GameContext *ctx, float angle)
{
    float len;
    int signX, signY;
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

    while (!hitWall(p->pos, endX, ctx))
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
    while (!hitWall(p->pos, endY, ctx))
    {
        adjY = (Vector2){.x = p->pos.x, .y = cornerSquare.y};
        len = Vector2Length(Vector2Subtract(p->pos, adjY));
        endY = (Vector2){.x = adjY.x + (tan((angle + PI / 2) * signY) * len), .y = cornerSquare.y};
        cornerSquare.y += (-signY) * TILE_SIZE;
    }

    float distX = Vector2Length(Vector2Subtract(endX, p->pos));
    float distY = Vector2Length(Vector2Subtract(endY, p->pos));
    DrawLineV(p->pos, distX > distY ? endY : endX, BLUE);
    float dist = MIN(distX, distY);
    return (RayInfo){.length = dist, .side = distX > distY ? signY : signX};
}

void fov(GameContext *ctx)
{

    Player p = ctx->player;
    memset(buffer, 0, PLAYER_SCREEN_WIDTH);
    // largeur du champ de vision (contrôle le "zoom" de la caméra)
    float planeSize = tanf(PLAYER_FOV * 0.5f);

    // vecteur perpendiculaire à la direction du joueur
    // représente la "largeur de l'écran" dans le monde
    Vector2 plane = {
        .x = -p.dir.y * planeSize,
        .y = p.dir.x * planeSize};

    // angle du rayon le plus à gauche du champ de vision
    float start = p.angle - (PLAYER_FOV * 0.5f);

    for (int x = 0; x < PLAYER_SCREEN_WIDTH; x++)
    {
        // position du pixel sur l'écran (-1 gauche → +1 droite)
        float cameraX = ((2.0f * x) / PLAYER_SCREEN_WIDTH) - 1.0f;

        // direction du rayon basée sur l'écran virtuel (camera plane)
        Vector2 rayDir = Vector2Add(p.dir, Vector2Scale(plane, cameraX));
        // float angle = atan2f(rayDir.y, rayDir.x);
        // version alternative : calcul direct en angles (plus simple mais moins "engine-like")
        float angle = start + ((PLAYER_FOV * x) / PLAYER_SCREEN_WIDTH);
        buffer[x] = raycast(ctx, angle);
    }
}

void gameLoop(GameContext *ctx)
{
    handleInput(ctx);
    drawMap(ctx);
    movePlayer(ctx);
    fov(ctx);
    drawPlayer(ctx);
    draw3D(ctx);
}

int Draw(void *ctx)
{
    gameLoop((GameContext *)ctx);
}

int Update(void *ctx, float dt) {}