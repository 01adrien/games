#if !defined(POOL_H)
#define POOL_H

#include "utils.h"

// Screen
#define WIDTH 1200
#define HEIGHT 600
// Pool related
#define MAX_HOLES 6
// 16 with white ball
#define MAX_BALLS 16
#define TABLE_WIDTH (WIDTH / 3 * 2)
#define TABLE_HEIGHT (HEIGHT / 3 * 2)
#define TABLE_BORDER 25
#define HOLE_RADIUS 15
#define HOLE_BORDER 10
#define BALL_RADIUS 13
#define CUE_LENGTH 350
#define MAX_PULL_BACK 50

typedef enum ballState
{
    BALL_IDLE,
    BALL_MOVING,
    BALL_OUT,
} BallState;

typedef struct ball
{
    Vector2 center;
    Vector2 dir;
    Vector2 velocity;
    float radius;
    Color color;
    uint8_t number;
    BallState state;
} Ball;

typedef struct hole
{
    Vector2 center;
    float radius;
} Hole;

typedef enum cueState
{
    CUE_IDLE,
    CUE_PULL_BACK,
    CUE_RELEASE,
    CUE_ROTATE_CK,
    CUE_ROTATE_ACK,
    CUE_HIT,
    CUE_SHOOT,
} CueState;

typedef struct cue
{
    Vector2 pos;
    Vector2 dir;
    float angle;
    float pullBack;
    float length;
    CueState state;

} Cue;

typedef struct player
{
    Cue cue;
    uint8_t score;
} Player;

typedef struct pool
{
    Rectangle table;
    Hole holes[MAX_HOLES];
    Ball balls[MAX_BALLS];
    Player player[2];
} Pool;

typedef struct gameContext
{
    void (*setupGame)(void);
    Pool pool;
} GameContext;

void setupGame();
void drawPool(Pool pool);
void drawTable(Rectangle table);
void drawBall(Ball ball);
void drawCue(Cue cue);
void moveCue(Cue *cue);
void moveBalls();
void handleInput();
void checkHitBorder(Ball *ball);
bool isShooting(Player player);

inline void printVector2(Vector2 v)
{
    printf("x = %.2f  |  y = %.2f\n", v.x, v.y);
}

bool isNotMoving(Ball ball);

Ball mkBall(float x, float y, Color color);
void resolveBallCollision(Ball *a, Ball *b);

/*

Vx = (1, 0)
Vy = (0, 1)

*/

#endif // POOL_H

/*
on trouve la direction du chaoc
on fait la normal de ce vecteur
overlap
vitesse relative
dot product
injection du dot dans la normale

*/