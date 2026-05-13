#if !defined(PONG_H)
#define PONG_H

#include "raylib.h"
#include "raymath.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "string.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define RACKET_LENGTH 60
#define RACKET_THICK 10
#define BALL_SIZE 20
#define SPEED_SCALE 2.5
#define BALL_SPEED 2
#define OPTIONS_MENU_COUNT 3
#define ANGLE 50
#define M_KEY 59
#define Q_KEY 65
#define FRICTION_SPEED 0.99
#define MAX_ANGLE_RACKET 15
#define FRICTION_ANGLE 0.60
#define BORDER_PADDING 10

typedef enum state
{
    STATE_MENU = 0,
    STATE_PLAYING,
    STATE_DIFFICULTY,
    STATE_PAUSE
} State;

typedef enum direction
{
    TOP = -1,
    NONE,
    BOTTOM,
} Direction;

typedef struct player
{
    Rectangle racket;
    Direction direction;
    float angle;
    int speed;
    int score;
} Player;

typedef struct ball
{
    Vector2 center;
    Vector2 direction; // direction of the ball {.x = speed, .y = angle}
    int radius;
} Ball;

void game();
void setUpGame(int baseAngle);
bool onKeyPressed();
void drawGame();
void moveBall();
void displayMenu();
void displayDifficulty();
void drawRayTracing();
double degToRad(int deg);
float racketBounce(Rectangle racket, int direction);
bool isLimitUp(int y);
bool isLimitDown(int y);
bool isLimitRight(int x);
bool isLimitLeft(int x);
void checkWallColision(Player *player);
void movePlayer(Player *player);
void rayTraceBall(float side, Vector2 dir, Vector2 start);

float lerp(float x, float y, float a)
{
    return (1.0f - a) * x + a * y;
}

float damper(float x, float g, float factor)
{
    return lerp(x, g, factor);
}
/*

             270°
              ↑
    180° ←         → 0°
              ↓
             90°


INPUT → direction
haut = -1
bas  = +1
rien = 0

PHYSIQUE → vitesse
vitesse += direction * accélération
vitesse *= friction

POSITION → mouvement
position += vitesse


accélération = 1.2
friction = 0.96
vitesse max = 250

DrawMesh;
GenMeshPlane;


*/
#endif //
