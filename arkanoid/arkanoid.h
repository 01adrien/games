#if !defined(ARKANOID_H)
#define ARKANOID_H

#include "utils.h"

// Screen
#define WIDTH 800
#define HEIGHT 600
// Paddle
#define PADDLE_WIDTH 80
#define PADDLE_HEIGHT 10
#define PADDLE_SPEED 1
// Breaker
#define BREAKER_SPEED 7
#define BREAKER_RADIUS 10
// Blocks
#define BLOCK_AREA_HEIGHT HEIGHT / 2
#define BLOCK_AREA_WIDTH WIDTH
#define BLOCK_WIDTH 200
#define BLOCK_HEIGHT 60
//
#define MAX_LEVEL 6
#define FRICTION 0.97

typedef enum paddleMove
{
    MOVE_LEFT = -1,
    MOVE_NONE,
    MOVE_RIGHT,
} PaddleMove;

typedef struct paddle
{
    Rectangle p;
    PaddleMove move;
    int speed;
    int score;
    int level;
    int destroyedBlocks;
} Paddle;

typedef struct breaker
{
    Vector2 center;
    Vector2 direction; // {.x = speed, .y = angle}
    int radius;
    bool hit;
    bool moving;
} Breaker;

typedef enum blockType
{
    BLOCK_EASY = 0,
    BLOCK_MEDIUM,
    BLOCK_HARD,
} BlockType;

typedef struct block
{
    BlockType type;
    int pv;
} Block;

typedef Block (*Level)(int, int);

void inputHandler();
void gameLoop();
void movePaddle();
void moveBreaker();
void checkColision();
void drawGame();
void setUpGame(int level);
void initBlocks(Level level);
bool isLimitUp(int y);
bool isLimitDown(int y);
bool isLimitRight(int x);
bool isLimitLeft(int x);
Block mkBlock(BlockType type);
Block levelOne(int i, int j);
Block levelTwo(int i, int j);
Block levelThree(int i, int j);
Block levelFour(int i, int j);
Block levelFive(int i, int j);
Block levelSix(int i, int j);
Vector2 colisionSide(Rectangle rec);
#endif // ARKANOID_H
