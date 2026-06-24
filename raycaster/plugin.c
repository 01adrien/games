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

Vector2 worldToMini(Vector2 p, GameContext *ctx)
{
    return (Vector2){
        ctx->mapPos.x + p.x * MAP_SCALE,
        ctx->mapPos.y + p.y * MAP_SCALE,
    };
}

int _posToIndex(float p)
{
    int index = (int)(p / TILE_SIZE);
    if (index < 0)
        index = 0;
    else if (index >= MAP_SIZE)
        index = MAP_SIZE - 1;
    return index;
}

TileIndex posToIndex(Vector2 pos)
{
    return (TileIndex){
        .w = _posToIndex(pos.x),
        .h = _posToIndex(pos.y),
    };
}

float _indexToPos(int n)
{
    return n * TILE_SIZE;
}

Vector2 indexToPos(TileIndex index)
{
    return (Vector2){
        .x = _indexToPos(index.w),
        .y = _indexToPos(index.h),
    };
}

TileType getMapTile(TileIndex index, GameContext *ctx)
{
    return (TileType)ctx->map[index.h * MAP_WIDTH + index.w];
}

bool isWall(TileType tile)
{
    return tile > TILE_EMPTY && tile <= TILE_EAGLE;
}

bool checkColisionPlayer(GameContext *ctx, Vector2 *vel)
{
    Player p = ctx->player;
    Vector2 nextMove = Vector2Add(p.pos, Vector2Scale(p.vel, p.radius + 20));
    TileIndex pTile = posToIndex(nextMove);
    TileType tile = getMapTile(pTile, ctx);
    if (isWall(tile))
    {
        // On teste l'axe X et Y pour voir si il y a possibilité de slider contre le mur
        Vector2 checkX = {.x = p.pos.x + p.vel.x * (p.radius + 8), .y = p.pos.y};
        Vector2 checkY = {.x = p.pos.x, .y = p.pos.y + p.vel.y * (p.radius + 8)};
        TileIndex pTileX = posToIndex(checkX);
        TileIndex pTileY = posToIndex(checkY);

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

    if (p->move.rot == ROT_RIGHT)
        p->angle += PLAYER_ROTATION * ctx->dt;

    else if (p->move.rot == ROT_LEFT)
        p->angle -= PLAYER_ROTATION * ctx->dt;

    p->dir = Vector2Normalize((Vector2){cosf(p->angle), sinf(p->angle)});

    Vector2 perp = {-p->dir.y, p->dir.x};
    p->plane = Vector2Scale(perp, 0.66f);

    if (p->move.dir == DIR_UP)
        p->vel = p->dir;

    else if (p->move.dir == DIR_DOWN)
        p->vel = Vector2Negate(p->dir);

    else if (p->move.dir == DIR_NONE)
        return;

    Vector2 velSlide = {0, 0};
    if (checkColisionPlayer(ctx, &velSlide))
        p->vel = velSlide;

    p->pos = Vector2Add(p->pos, Vector2Scale(p->vel, PLAYER_SPEED * ctx->dt));
}

bool hitWall(Vector2 startRay, Vector2 endRay, GameContext *ctx, TileType *tile)
{
    TileIndex current = posToIndex(endRay);
    Vector2 tilePos = indexToPos(current);

    TileIndex frontier = {
        .h = tilePos.y == endRay.y ? current.h - 1 : current.h,
        .w = tilePos.x == endRay.x ? current.w - 1 : current.w,
    };

    TileType t1 = getMapTile(current, ctx);
    TileType t2 = getMapTile(frontier, ctx);

    *tile = MAX(t1, t2);

    return isWall(*tile);
}

RayInfo raycast(GameContext *ctx, Vector2 rayDir)
{
    float len;
    int signX, signY;
    TileType tileX, tileY;
    Vector2 adjX, adjY, endX, endY, cornerSquare;
    Player *p = &ctx->player;
    TileIndex pTile = posToIndex(p->pos);
    float angle = atan2f(rayDir.y, rayDir.x);
    // X intersections
    cornerSquare = indexToPos(pTile);
    endX = p->pos;

    // gauche ou droite
    if (rayDir.x >= 0)
        cornerSquare.x += TILE_SIZE;

    while (!hitWall(p->pos, endX, ctx, &tileX))
    {
        // Distance horizontale entre le joueur et la ligne verticale actuellement testée.
        float dx = cornerSquare.x - p->pos.x;
        // coef directeur / pente de la droite
        float coef = (rayDir.y / rayDir.x);
        endX = (Vector2){
            .x = cornerSquare.x,
            // Application de la pente du rayon pour retrouver Y.
            .y = p->pos.y + dx * coef,

        };

        cornerSquare.x += rayDir.x >= 0 ? TILE_SIZE : -TILE_SIZE;
    }

    // Y intersections
    cornerSquare = indexToPos(pTile);
    endY = p->pos;

    // Haut ou bas
    if (rayDir.y >= 0)
        cornerSquare.y += TILE_SIZE;

    while (!hitWall(p->pos, endY, ctx, &tileY))
    {
        float dy = cornerSquare.y - p->pos.y;

        endY = (Vector2){
            .x = p->pos.x + dy * (rayDir.x / rayDir.y),
            .y = cornerSquare.y};

        cornerSquare.y += rayDir.y >= 0 ? TILE_SIZE : -TILE_SIZE;
    }

    float distX = Vector2Length(Vector2Subtract(endX, p->pos));
    float distY = Vector2Length(Vector2Subtract(endY, p->pos));
    float dist = MIN(distX, distY);

    // Avoid fish eye
    float perp = dist * cos(angle - p->angle);

    return (RayInfo){
        .length = perp,
        .side = distX > distY ? 1 : -1,
        .tile = distX > distY ? tileY : tileX,
        .textureOffset = distX > distY ? -((int)(endY.x) % 64) : (int)(endX.y) % 64,
        .hit = distX > distY ? endY : endX,
    };
}

void draw(GameContext *ctx)
{
    // DRAW PLAYER 3D VIEW
    Player p = ctx->player;
    Vector2 corner = {.x = p.screen.x, .y = p.screen.y};

    for (int x = 0; x < PLAYER_SCREEN_WIDTH; x++)
    {
        // camera x = -1 -> 1 for screenWidth
        float cameraX = 2.0f * x / (float)PLAYER_SCREEN_WIDTH - 1.0f;
        Vector2 rayDir = Vector2Add(p.dir, Vector2Scale(p.plane, cameraX));
        RayInfo ray = raycast(ctx, rayDir);
        float h = ZOOM / ray.length;
        buffer[x] = ray;
        float gap = (PLAYER_SCREEN_HEIGHT - h) / 2;
        Vector2 start = {.x = corner.x + x, .y = corner.y + gap};
        Vector2 end = {.x = start.x, .y = start.y + h};
        Rectangle src = {.x = ray.textureOffset, .y = 0, .width = 1, .height = 64};
        Rectangle dst = {.x = start.x, .y = start.y, .width = 1, .height = h};

        float shade = MAX(1.0f, 1.0f - (ray.length / (10 * TILE_SIZE)));
        Color c = ray.side == -1 ? WALL_LIGHT : WALL_LIGHT;
        Color wallColor = {c.r * shade, c.g * shade, c.b * shade, c.r};
        DrawTexturePro(ctx->textures[ray.tile], src, dst, (Vector2){0, 0}, 0, wallColor);
    }

    // DRAW MINI MAP
    for (size_t i = 0; i < MAP_HEIGHT; i++)
    {
        for (size_t j = 0; j < MAP_WIDTH; j++)
        {
            TileType tile = (TileType)ctx->map[i * MAP_WIDTH + j];
            Vector2 v = {.y = i * TILE_SIZE, .x = j * TILE_SIZE};
            Vector2 p = worldToMini(v, ctx);
            DrawRectangle(
                p.x, p.y,
                TILE_SIZE * MAP_SCALE,
                TILE_SIZE * MAP_SCALE,
                tile > 0 ? BLACK : RAYWHITE);
        }
    }

    // DRAW PLAYER

    for (size_t i = 0; i < PLAYER_SCREEN_WIDTH; i++)
        DrawLineV(
            worldToMini(p.pos, ctx),
            worldToMini(buffer[i].hit, ctx),
            FOV_COLOR);

    DrawCircleV(worldToMini(p.pos, ctx), PLAYER_RADIUS * MAP_SCALE, Fade(BLUE, .6));
}

void gameLoop(GameContext *ctx)
{
    ctx->dt = GetFrameTime();
    handleInput(ctx);
    movePlayer(ctx);
    draw(ctx);
}

int Draw(void *ctx)
{
    gameLoop((GameContext *)ctx);
}

int Update(void *ctx, float dt) {}