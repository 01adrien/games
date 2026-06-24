#if !defined(RAYCASTER_H)
#define RAYCASTER_H

#include "utils.h"
#include "plugin.h"

#define WIDTH 1000
#define HEIGHT 600
#define MAP_SIZE 24
#define MAP_WIDTH MAP_SIZE
#define MAP_HEIGHT MAP_SIZE
#define TILE_SIZE 64
#define MAP_SCALE 0.0625f
#define PLAYER_RADIUS 50
#define PLAYER_SPEED 300  // pixels / seconde
#define PLAYER_ROTATION 2 // radians / seconde
#define PLAYER_FOV (PI / 3)
#define PLAYER_HIGH 32
#define WALL_HIGH (PLAYER_HIGH * 2)
#define PLAYER_SCREEN_WIDTH WIDTH
#define PLAYER_SCREEN_HEIGHT HEIGHT
#define ZOOM 35000

// #define DEBUG

#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

typedef enum tileType
{
    TILE_EMPTY,
    TILE_BLUESTONE,
    TILE_WOOD,
    TILE_COLORSTONE,
    TILE_GREYSTONE,
    TILE_REDBRICK,
    TILE_EAGLE,
    TILE_COUNT,
} TileType;

Color WALL_LIGHT = {245, 242, 235, 255};
Color WALL_SHADOW = {155, 80, 80, 255};
Color FOV_COLOR = {255, 215, 0, 5};

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
    Vector2 gridPos;
    Vector2 dir;
    Vector2 vel;
    Vector2 plane;
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
    Texture2D textures[TILE_COUNT];
    float dt;
} GameContext;

void setupContext(GameContext *ctx);

#endif // RAYCASTER_H
