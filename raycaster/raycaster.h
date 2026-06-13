#if !defined(RAYCASTER_H)
#define RAYCASTER_H

#include "utils.h"
#include "plugin.h"

#define WIDTH 1200
#define HEIGHT 600
#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define TILE_SIZE 10
#define PLAYER_RADIUS 3
#define PLAYER_SPEED 8
#define PLAYER_FOV (PI / 3)
#define PLAYER_HIGH 32
#define WALL_HIGH (PLAYER_HIGH * 2)
#define PLAYER_SCREEN_WIDTH 520
#define PLAYER_SCREEN_HEIGHT 420

// #define DEBUG

#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

typedef enum tileType
{
    TILE_EMPTY,
    TILE_WALL,
    TILE_WALL2,
    TILE_WALL3,
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
    Rectangle screen;
} Player;

typedef struct mapIndex
{
    int h, w;
} TileIndex;

typedef struct gameContext
{
    int *map;
    Player player;
    Vector2 mapPos;
} GameContext;

void setupContext(GameContext *ctx);

#endif // RAYCASTER_H
