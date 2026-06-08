#if !defined(RAYCASTER_H)
#define RAYCASTER_H

#include "utils.h"
#include "plugin.h"

#define WIDTH 1200
#define HEIGHT 600
#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define TILE_SIZE 20
#define PLAYER_RADIUS 5
#define PLAYER_SPEED 8

typedef enum tileType
{
    TILE_EMPTY,
    TILE_WALL,
} TileType;

typedef enum playerDirection
{
    DIR_NONE,
    DIR_UP,
    DIR_DOWN,
} PlayerDirection;

typedef enum playerRotation
{
    ROT_NONE,
    ROT_LEFT,
    ROT_RIGHT,
} PlayerRotation;

typedef struct playerMove
{
    PlayerDirection dir;
    PlayerRotation rot;
} PlayerMove;

typedef struct player
{
    PlayerMove move;
    Vector2 pos;
    Vector2 dir;
    Vector2 vel;
    float angle;
    int radius;
} Player;

typedef struct gameContext
{
    int *map;
    Player player;
    Vector2 mapPos;
} GameContext;

void setupContext(GameContext *ctx);

#endif // RAYCASTER_H
