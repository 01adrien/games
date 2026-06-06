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
#define TABLE_BORDER 30
#define JAW_SIZE 15
#define HOLE_RADIUS 20
#define BALL_RADIUS 13
#define CUE_LENGTH 350
#define MAX_PULL_BACK 50
#define BANDS_COUNT 6
#define CORNER_JAW_ANGLE 50
#define SIDE_JAW_ANGLE 15
#define SCORE_BALL_RADIUS 35
#define MAX_PARTICULES 50

#define NORMAL_LEFT (Vector2){.x = -1, .y = 0}
#define NORMAL_RIGHT (Vector2){.x = 1, .y = 0}
#define NORMAL_TOP (Vector2){.x = 0, .y = -1}
#define NORMAL_BOTTOM (Vector2){.x = 0, .y = 1}

typedef struct particule
{
    Vector2 pos, vel, acc;
} Particule;

typedef enum ballState
{
    BALL_IDLE,
    BALL_MOVING,
    BALL_OUT,
} BallState;

typedef enum ballKind
{
    BALL_RED,
    BALL_YELLOW,
    BALL_WHITE,
    BALL_BLACK,
} BallKind;

typedef struct ball
{
    Vector2 center;
    Vector2 dir;
    Vector2 velocity;
    float radius;
    Color color;
    uint8_t number;
    BallState state;
    BallKind kind;
    float angle;
} Ball;

typedef enum jawPos
{
    JAW_LEFT,
    JAW_CENTER,
    JAW_RIGHT,
} JawPos;

typedef struct band
{
    Vector2 start, end, normal;
} Band;

typedef struct jaw
{
    Vector2 start, end, normal;
} Jaw;

typedef struct hole
{
    Vector2 center;
    float radius;
    Jaw jaws[2];
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
    float rayLen;

} Cue;

typedef struct player
{
    Cue cue;
    uint8_t score;
    BallKind kind;
} Player;

typedef struct pool
{
    Rectangle table;
    Particule particules[MAX_PARTICULES];
    Hole holes[MAX_HOLES];
    Ball balls[MAX_BALLS];
    Band bands[BANDS_COUNT];
    Player player[2];
    uint8_t redCount, yellowCount, currentPlayer;
} Pool;

typedef struct gameContext GameContext;

struct gameContext
{
    void (*setupGame)(GameContext *);
    Pool pool;
    Ball *whiteBall;
    Player *currentPlayer;
};

void setupGame(GameContext *ctx);
void gameLoop(GameContext *ctx);

void drawFloor();
void drawPool(GameContext *ctx);
void drawTable(Rectangle table);
void drawHoles(Hole *holes);
void drawBands(Band *bands);
void drawBall(Ball ball);
void drawCue(Cue cue);
void drawBand(Vector2 start, Vector2 end);
void moveCue(GameContext *ctx);
void moveBalls(GameContext *ctx);
void handleInput(GameContext *ctx);
void drawHitRay(GameContext *ctx);
Vector2 getRayEnd(GameContext *ctx);
Vector2 getNormal(Vector2 start, Vector2 end);
bool checkHitBands(GameContext *ctx, Ball *ball);
bool checkHitJaws(GameContext *ctx, Ball *ball);
bool isShooting(Player player);
void printVector2(Vector2 v);
bool isNotMoving(Ball ball);
Ball mkBall(float x, float y, Color color, BallKind kind, uint8_t number);
Hole mkHole(float x, float y, float angleL, float angleR, float angleC, JawPos jawPos);
void resolveBallCollision(Ball *a, Ball *b);
void moveParticule(Particule *particule);
void throwParticules(GameContext *ctx, int side);

#endif // POOL_H

/*

| Angle | Direction |
| ----- | --------- |
| 0°    | →         |
| 45°   | ↘         |
| 90°   | ↓         |
| 135°  | ↙         |
| 180°  | ←         |
| 225°  | ↖         |
| 270°  | ↑         |
| 315°  | ↗         |


*/