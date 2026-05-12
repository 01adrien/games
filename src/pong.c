
#include "pong.h"

State gameState = STATE_MENU;
Player playerL, playerR;
Ball ball;

char menu[][32] = {
    {"[G]ame"},
    {"[E]xit"},
    {"[D]ifficulty"},
};

float lerp(float x, float y, float a)
{
    return (1.0f - a) * x + a * y;
}

float damper(float x, float g, float factor)
{
    return lerp(x, g, factor);
}

void pong()
{

    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(screenWidth, screenHeight, "Pong");
    SetTargetFPS(40);
    setUpGame(1);
    ball.direction.x = BALL_SPEED;
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
    bool isPressed = onKeyPressed();
    if (gameState == STATE_PLAYING)
    {
        moveBall();
        movePlayer(&playerR);
        movePlayer(&playerL);
        checkWallColision(&playerR);
        checkWallColision(&playerL);
    }
    drawGame();
}

void setUpGame(int baseAngle)
{
    int yRacket = SCREEN_HEIGHT / 2 - RACKET_LENGTH / 2;

    playerL.racket.x = 5;
    playerL.racket.y = yRacket;
    playerL.racket.height = RACKET_LENGTH;
    playerL.racket.width = RACKET_THICK;

    playerL.angle = 0;
    playerL.speed = 0;

    playerR.racket.x = SCREEN_WIDTH - RACKET_THICK - 5;
    playerR.racket.y = yRacket;
    playerR.racket.height = RACKET_LENGTH;
    playerR.racket.width = RACKET_THICK;

    playerR.angle = 0;
    playerR.speed = 0;

    ball.center.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
    ball.center.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
    ball.direction.y = GetRandomValue(0 - baseAngle, 0 + baseAngle);
    ball.radius = BALL_SIZE / 2;

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
        Rectangle right;
        Vector2 originRight = {.x = RACKET_THICK / 2, .y = RACKET_LENGTH / 2};
        right.height = playerR.racket.height;
        right.width = playerR.racket.width;
        right.x = playerR.racket.x + RACKET_THICK / 2;
        right.y = playerR.racket.y + RACKET_LENGTH / 2;
        DrawRectanglePro(right, originRight, playerR.angle * playerR.direction, BLACK);

        Rectangle left;
        Vector2 originLeft = {.x = -(RACKET_THICK / 2), .y = (RACKET_LENGTH / 2)};
        left.height = playerL.racket.height;
        left.width = playerL.racket.width;
        left.x = playerL.racket.x - RACKET_THICK / 2;
        left.y = playerL.racket.y + RACKET_LENGTH / 2;
        DrawRectanglePro(left, originLeft, playerL.angle * playerL.direction, BLACK);

        DrawCircleV(ball.center, ball.radius, BLACK);
        // drawRayTracing();
    }
}

bool onKeyPressed()
{

    if (gameState == STATE_PLAYING)
    {

        if (IsKeyPressed(KEY_RIGHT) || IsKeyDown(KEY_RIGHT))
        {
            if (!isLimitUp(playerL.racket.y))
                playerL.direction = TOP;
            else
                playerL.direction = NONE;
        }
        else if (IsKeyPressed(KEY_LEFT) || IsKeyDown(KEY_LEFT))
        {
            if (!isLimitDown(playerL.racket.y - RACKET_LENGTH))
                playerL.direction = BOTTOM;
            else
                playerL.direction = NONE;
        }
        else if (IsKeyPressed(KEY_UP) || IsKeyDown(KEY_UP))
        {
            if (!isLimitUp(playerR.racket.y))
                playerR.direction = TOP;
            else
                playerR.direction = NONE;
        }
        else if (IsKeyPressed(KEY_DOWN) || IsKeyDown(KEY_DOWN))
        {
            if (!isLimitDown(playerR.racket.y - RACKET_LENGTH))
                playerR.direction = BOTTOM;
            else
                playerR.direction = NONE;
        }
        else if (IsKeyDown(M_KEY))
        {

            gameState = STATE_MENU;
        }
        else
        {
            playerR.direction = NONE;
            playerL.direction = NONE;
        }
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
            ball.direction.x = GetKeyPressed() - KEY_ZERO;
            gameState = STATE_MENU;
        }
        else if (IsKeyDown(Q_KEY))
            gameState = STATE_MENU;
    }
    return true;
}

void checkWallColision(Player *player)
{
    if (isLimitDown(player->racket.y + RACKET_LENGTH) || isLimitUp(player->racket.y))
    {
        player->direction = NONE;
        player->speed = 0;
    }
}

void movePlayer(Player *player)
{
    if (player->direction != NONE)
    {

        player->speed += player->direction;
        if (player->angle < MAX_ANGLE_RACKET)
            player->angle += 0.30;
    }
    else
    {
        player->speed *= FRICTION_SPEED;
        player->angle *= FRICTION_ANGLE;
    }

    player->racket.y += player->speed;
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
        DrawText(d[i], x, y, 20, i == (ball.direction.x - 1) ? RED : BLACK);
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
    Vector2 startLeft = {.x = playerL.racket.x + RACKET_THICK, .y = playerL.racket.y};
    Vector2 endLeftTop = {.y = 0, .x = tan(degToRad(ANGLE)) * startLeft.y};
    Vector2 endLeftBottom = {.y = SCREEN_HEIGHT, .x = (tan(degToRad(180 - ANGLE)) * (SCREEN_HEIGHT - startLeft.y)) * -1};
    DrawLineV(startLeft, endLeftTop, RED);
    startLeft.y += RACKET_LENGTH;
    DrawLineV(startLeft, endLeftBottom, RED);

    // RAY TRACING RACKET RIGHT
    Vector2 startRight = {.x = playerR.racket.x, .y = playerR.racket.y};
    Vector2 endRightTop = {.y = 0, .x = SCREEN_WIDTH - (tan(degToRad(ANGLE)) * startRight.y)};
    Vector2 endRightBottom = {.y = SCREEN_HEIGHT, .x = SCREEN_WIDTH - ((tan(degToRad(180 - ANGLE)) * (SCREEN_HEIGHT - startRight.y)) * -1)};
    DrawLineV(startRight, endRightTop, RED);
    startRight.y += RACKET_LENGTH;
    DrawLineV(startRight, endRightBottom, RED);
}

void moveBall()
{
    double deltaX, deltaY;
    if (isLimitDown(ball.center.y + ball.radius) || isLimitUp(ball.center.y - ball.radius))
        ball.direction.y = -ball.direction.y;

    else if (isLimitLeft(ball.center.x + ball.radius) || isLimitRight(ball.center.x + ball.radius))
    {
        setUpGame(isLimitLeft(ball.center.x + ball.radius) ? 180 : 0);
        return;
    }
    else
    {
        if (CheckCollisionCircleRec(ball.center, ball.radius, playerR.racket))
        {
            float bounce = racketBounce(playerR.racket, -1);
            printf("bounce = %.2f  ||  angle = %.2f \n", bounce, playerR.angle);
            ball.direction.y = bounce - (playerR.angle);
        }

        else if (CheckCollisionCircleRec(ball.center, ball.radius, playerL.racket))
        {
            float bounce = racketBounce(playerL.racket, 1);
            printf("bounce = %.2f  ||  angle = %.2f\n", bounce, playerL.angle);
            ball.direction.y = bounce + (playerL.angle);
        }
    }

    ball.center.y += (ball.direction.x * SPEED_SCALE) * sin(degToRad(ball.direction.y));
    ball.center.x += (ball.direction.x * SPEED_SCALE) * cos(degToRad(ball.direction.y));
}

float racketBounce(Rectangle racket, int direction)
{
    int angle = 0;
    if (direction > 0) // vers droite
    {
        if ((ball.center.y) < (racket.y + RACKET_LENGTH / 2))
            return GetRandomValue(360 - ANGLE, 360); // haut-droite
        else
            return GetRandomValue(0, ANGLE); // bas-droite
    }
    else // vers gauche
    {
        if (ball.center.y < (racket.y + RACKET_LENGTH / 2))
            return GetRandomValue(180, 180 + ANGLE); // haut-gauche
        else
            return GetRandomValue(180 - ANGLE, 180); // bas-gauche
    }
}

bool isLimitUp(int y)
{
    return y <= 0;
}

bool isLimitDown(int y)
{
    return y >= SCREEN_HEIGHT;
}

bool isLimitRight(int x)
{
    return x >= SCREEN_WIDTH;
}

bool isLimitLeft(int x)
{
    return x <= 0;
}

double degToRad(int deg)
{
    return deg * (PI / 180);
}

// -----------------------------------------------
