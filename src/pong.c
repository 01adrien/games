#include "raylib.h"
#include "raymath.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "string.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 400
#define RACKET_LENGTH 60
#define RACKET_THICK 10
#define BALL_SIZE 20
#define RACKET_SPEED 6
#define SPEED_SCALE 2.5
#define BALL_SPEED 2
#define OPTIONS_MENU_COUNT 3
#define ANGLE 50
#define M_KEY 59
#define Q_KEY 65

typedef enum state
{
    STATE_MENU = 0,
    STATE_PLAYING,
    STATE_DIFFICULTY
} State;

void game();
void setUpGame(int startDir);
void onKeyPressed();
void drawGame();
void moveBall();
void displayMenu();
void displayDifficulty();
void drawRayTracing();
double degToRad(int deg);
float racketBounce(Rectangle racket, int direction);
bool isLimitUp(Rectangle r);
bool isLimitDown(Rectangle r);
bool isLimitRight(Rectangle r);
bool isLimitLeft(Rectangle r);

Rectangle racketL, racketR, ball;
State gameState = STATE_MENU;
// direction of the ball {.x = speed, .y = angle}
Vector2 ballVec;

char menu[][32] = {
    {"[G]ame"},
    {"[E]xit"},
    {"[D]ifficulty"},
};

void pong()
{
    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(screenWidth, screenHeight, "Pong");
    SetTargetFPS(40);
    setUpGame(1);
    ballVec.x = BALL_SPEED;
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        game();
        DrawFPS(1, 1);
        EndDrawing();
    }
    CloseWindow();
}

void game()
{
    onKeyPressed();
    if (gameState == STATE_PLAYING)
    {
        moveBall(ball);
    }
    drawGame();
}

void setUpGame(int startDir)
{
    int yRacket = SCREEN_HEIGHT / 2 - RACKET_LENGTH / 2;
    racketL.x = 0;
    racketL.y = yRacket;
    racketL.height = RACKET_LENGTH;
    racketL.width = RACKET_THICK;

    racketR.x = SCREEN_WIDTH - RACKET_THICK;
    racketR.y = yRacket;
    racketR.height = RACKET_LENGTH;
    racketR.width = RACKET_THICK;

    ball.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
    ball.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
    ball.height = BALL_SIZE;
    ball.width = BALL_SIZE;

    ballVec.y = GetRandomValue(45, 45 + 90);
    ballVec.y *= startDir;
    drawGame();
}

void drawGame()
{
    if (gameState == STATE_MENU)
        displayMenu();
    else if (gameState == STATE_DIFFICULTY)
        displayDifficulty();
    else
    {
        DrawRectangleRec(racketL, BLACK);
        DrawRectangleRec(racketR, BLACK);
        DrawRectangleRec(ball, BLACK);
        Vector2 startL = {.x = racketL.x + RACKET_THICK, .y = racketL.y + (RACKET_LENGTH / 2)};
        drawRayTracing();
    }
}

void onKeyPressed()
{
    if (gameState == STATE_PLAYING)
    {

        if (IsKeyPressed(KEY_RIGHT) || IsKeyDown(KEY_RIGHT))
        {
            if (!isLimitUp(racketL))
                racketL.y -= RACKET_SPEED;
        }
        else if (IsKeyPressed(KEY_LEFT) || IsKeyDown(KEY_LEFT))
        {
            if (!isLimitDown(racketL))
                racketL.y += RACKET_SPEED;
        }
        else if (IsKeyPressed(KEY_UP) || IsKeyDown(KEY_UP))
        {
            if (!isLimitUp(racketR))
                racketR.y -= RACKET_SPEED;
        }
        else if (IsKeyPressed(KEY_DOWN) || IsKeyDown(KEY_DOWN))
        {
            if (!isLimitDown(racketR))
                racketR.y += RACKET_SPEED;
        }
        else if (IsKeyDown(M_KEY))
            gameState = STATE_MENU;
    }
    else if (gameState == STATE_MENU)
    {
        if (IsKeyDown(KEY_G))
            gameState = STATE_PLAYING;

        if (IsKeyDown(KEY_D))
            gameState = STATE_DIFFICULTY;

        else if (IsKeyDown(KEY_E))
            CloseWindow();
    }
    else if (gameState == STATE_DIFFICULTY)
    {
        if (IsKeyDown(KEY_ONE) || IsKeyDown(KEY_TWO) || IsKeyDown(KEY_THREE))
        {
            ballVec.x = GetKeyPressed() - KEY_ZERO;
            gameState = STATE_MENU;
        }
        else if (IsKeyDown(Q_KEY))
            gameState = STATE_MENU;
    }
}

void displayDifficulty()
{
    int y = 80;
    int x = (SCREEN_WIDTH / 2) - 60;

    DrawText("Difficulty", x, y, 30, BLACK);
    y += 30;
    const char *d[] = {"1", "2", "3"};
    for (size_t i = 0; i < 3; i++)
    {
        DrawText(d[i], x, y, 20, i == (ballVec.x - 1) ? RED : BLACK);
        y += 20;
    }
}

void displayMenu()
{
    int y = 80;
    int x = (SCREEN_WIDTH / 2) - 60;

    DrawText("Menu", x, y, 30, BLACK);
    y += 30;
    for (size_t i = 0; i < OPTIONS_MENU_COUNT; i++)
    {
        char *o = menu[i];
        DrawText(o, x, y, 20, BLACK);
        y += 20;
    }
}

void drawRayTracing()
{
    // RAY TRACING RACKET LEFT
    Vector2 startLeft = {.x = racketL.x + RACKET_THICK, .y = racketL.y};
    Vector2 endLeftTop = {.y = 0, .x = tan(degToRad(ANGLE)) * startLeft.y};
    Vector2 endLeftBottom = {.y = SCREEN_HEIGHT, .x = (tan(degToRad(180 - ANGLE)) * (SCREEN_HEIGHT - startLeft.y)) * -1};
    DrawLineV(startLeft, endLeftTop, RED);
    startLeft.y += RACKET_LENGTH;
    DrawLineV(startLeft, endLeftBottom, RED);

    // RAY TRACING RACKET RIGHT
    Vector2 startRight = {.x = racketR.x, .y = racketR.y};
    Vector2 endRightTop = {.y = 0, .x = SCREEN_WIDTH - (tan(degToRad(ANGLE)) * startRight.y)};
    Vector2 endRightBottom = {.y = SCREEN_HEIGHT, .x = SCREEN_WIDTH - ((tan(degToRad(180 - ANGLE)) * (SCREEN_HEIGHT - startRight.y)) * -1)};
    DrawLineV(startRight, endRightTop, RED);
    startRight.y += RACKET_LENGTH;
    DrawLineV(startRight, endRightBottom, RED);
}

void moveBall()
{
    double deltaX, deltaY;
    if (isLimitDown(ball) || isLimitUp(ball))
    {
        printf("LIMIT\n");
        ballVec.y += 90;
    }

    else if (isLimitLeft(ball) || isLimitRight(ball))
    {
        setUpGame(isLimitLeft(ball) ? -1 : 1);
        return;
    }
    else
    {
        if (CheckCollisionRecs(racketR, ball))
            ballVec.y = racketBounce(racketR, -1);

        else if (CheckCollisionRecs(racketL, ball))
            ballVec.y = racketBounce(racketL, 1);
    }

    ball.y += (ballVec.x * SPEED_SCALE) * cos(degToRad(ballVec.y));
    ball.x += (ballVec.x * SPEED_SCALE) * sin(degToRad(ballVec.y));
}

float racketBounce(Rectangle racket, int direction)
{
    if (ball.y < (racket.y + RACKET_LENGTH / 2))
    {
        printf("TOP\n");
        return GetRandomValue(90, 180 - ANGLE) * direction;
    }
    else
    {
        printf("BOTTOM\n");
        return GetRandomValue(ANGLE, 90) * direction;
    }
}

bool isLimitUp(Rectangle r)
{
    return r.y <= 0;
}

bool isLimitDown(Rectangle r)
{
    return (r.y + r.height) >= SCREEN_HEIGHT;
}

bool isLimitRight(Rectangle r)
{
    return (r.x + r.width) >= SCREEN_WIDTH;
}

bool isLimitLeft(Rectangle r)
{
    return r.x <= 0;
}

double degToRad(int deg)
{
    return deg * (PI / 180);
}
