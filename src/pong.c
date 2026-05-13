
#include "pong.h"

State gameState = STATE_MENU;
Player playerL, playerR;
Ball ball;

Ball menuBall;

bool rayTracing = false;

char menu[][32] = {
    {"[G]ame"},
    {"[E]xit"},
    {"[D]ifficulty"},
};

void pong()
{
    InitAudioDevice();
    Music music = LoadMusicStream("./assets/music.mp3");

    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(screenWidth, screenHeight, "Pong");
    SetTargetFPS(40);
    setUpGame(1);
    ball.direction.x = BALL_SPEED;
    playerR.score = 0;
    playerL.score = 0;

    menuBall.direction.x = 10;
    menuBall.direction.y = 45;
    menuBall.center = (Vector2){.x = GetRandomValue(60, SCREEN_WIDTH - 60), .y = GetRandomValue(60, SCREEN_HEIGHT - 60)};
    menuBall.radius = BALL_SIZE / 2;

    PlayMusicStream(music);
    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        game();
        // DrawFPS(1, 1);
        EndDrawing();
    }
    CloseWindow();
    UnloadMusicStream(music);
    CloseAudioDevice();
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

    ball.center.x = SCREEN_WIDTH / 2;
    ball.center.y = SCREEN_HEIGHT / 2;
    ball.direction.y = GetRandomValue(0 - baseAngle, 0 + baseAngle);
    ball.radius = BALL_SIZE / 2;
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
        Vector2 originLeft = {.x = -RACKET_THICK / 2, .y = RACKET_LENGTH / 2};
        left.height = playerL.racket.height;
        left.width = playerL.racket.width;
        left.x = playerL.racket.x - RACKET_THICK / 2;
        left.y = playerL.racket.y + RACKET_LENGTH / 2;
        DrawRectanglePro(left, originLeft, playerL.angle * playerL.direction, BLACK);

        DrawCircleV(ball.center, ball.radius, BLACK);
        char score[5];
        snprintf(score, 5, "%d", playerL.score);
        DrawText(score, 60, SCREEN_HEIGHT - 80, 60, BLACK);
        snprintf(score, 5, "%d", playerR.score);
        DrawText(score, SCREEN_WIDTH - 80, SCREEN_HEIGHT - 80, 60, BLACK);

        int lineSize = 5;
        Vector2 startLine = {.x = SCREEN_WIDTH / 2, .y = 0};
        Vector2 endLine = {.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT};
        DrawLineDashed(startLine, endLine, lineSize, 10, BLACK);

        DrawRectangle(0, 0, SCREEN_WIDTH, BORDER_PADDING, YELLOW);
        DrawRectangle(0, SCREEN_HEIGHT - BORDER_PADDING, SCREEN_WIDTH, BORDER_PADDING, YELLOW);

        if (rayTracing)
        {
            drawRayTracing();
        }
    }
}

bool onKeyPressed()
{
    if (gameState == STATE_PLAYING)
    {
        if (IsKeyPressed(KEY_RIGHT) || IsKeyDown(KEY_RIGHT))
        {
            if (!isLimitUp(playerL.racket.y - BORDER_PADDING))
                playerL.direction = TOP;
            else
                playerL.direction = NONE;
        }
        else if (IsKeyPressed(KEY_LEFT) || IsKeyDown(KEY_LEFT))
        {
            if (!isLimitDown(playerL.racket.y + RACKET_LENGTH + BORDER_PADDING))
                playerL.direction = BOTTOM;
            else
                playerL.direction = NONE;
        }
        else if (IsKeyPressed(KEY_UP) || IsKeyDown(KEY_UP))
        {
            if (!isLimitUp(playerR.racket.y - BORDER_PADDING))
                playerR.direction = TOP;
            else
                playerR.direction = NONE;
        }
        else if (IsKeyPressed(KEY_DOWN) || IsKeyDown(KEY_DOWN))
        {
            if (!isLimitDown(playerR.racket.y + RACKET_LENGTH + BORDER_PADDING))
                playerR.direction = BOTTOM;
            else
                playerR.direction = NONE;
        }
        else if (IsKeyDown(M_KEY))
            gameState = STATE_MENU;
        else if (IsKeyDown(KEY_SPACE) || IsKeyPressed(KEY_SPACE))
            gameState = STATE_PAUSE;
        else if (IsKeyDown(KEY_BACKSPACE) || IsKeyPressed(KEY_BACKSPACE))
            rayTracing = !rayTracing;
        else
        {
            playerR.direction = NONE;
            playerL.direction = NONE;
        }
    }
    else if (gameState == STATE_MENU)
    {
        if (IsKeyDown(KEY_G))
            gameState = STATE_PAUSE;

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
    else if (gameState == STATE_PAUSE)
    {
        if (IsKeyDown(KEY_SPACE) || IsKeyPressed(KEY_SPACE))
            gameState = STATE_PLAYING;
        else if (IsKeyDown(M_KEY))
            gameState = STATE_MENU;
        else if (IsKeyDown(KEY_BACKSPACE) || IsKeyPressed(KEY_BACKSPACE))
            rayTracing = !rayTracing;
    }

    return true;
}

void checkWallColision(Player *player)
{
    if (
        isLimitDown(player->racket.y + RACKET_LENGTH + BORDER_PADDING) ||
        isLimitUp(player->racket.y - BORDER_PADDING))
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
    char str[2];
    for (size_t i = 0; i < 3; i++)
    {
        snprintf(str, 2, "%d", i + 1);
        DrawText(str, x, y, 20, i == (ball.direction.x - 1) ? RED : BLACK);
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

    DrawCircleV(menuBall.center, menuBall.radius, BLACK);
    if (
        isLimitDown(menuBall.center.y + menuBall.radius) ||
        isLimitUp(menuBall.center.y - menuBall.radius))
    {
        menuBall.direction.y = -menuBall.direction.y;
    }
    else if (
        isLimitLeft(menuBall.center.x - menuBall.radius) ||
        isLimitRight(menuBall.center.x + menuBall.radius))
    {
        menuBall.direction.x = -menuBall.direction.x;
        menuBall.direction.y = -menuBall.direction.y;
    }

    Vector2 dir = {.x = cos(degToRad(menuBall.direction.y)), .y = sin(degToRad(menuBall.direction.y))};

    menuBall.center.y += (menuBall.direction.x) * dir.y;
    menuBall.center.x += (menuBall.direction.x) * dir.x;
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

    // RAY TRACING BALL
    Vector2 dir = {.x = cos(degToRad(ball.direction.y)), .y = sin(degToRad(ball.direction.y))};
    // la balle va vers la droite
    if (dir.x >= 0)
    {
        float right = (SCREEN_WIDTH - ball.center.x) / dir.x;
        rayTraceBall(right, dir, ball.center);
    }
    //  la balle va vers la gauche
    else
    {
        float left = ball.center.x / -dir.x;
        rayTraceBall(left, dir, ball.center);
    }
}

void moveBall()
{
    if (
        isLimitDown(ball.center.y + ball.radius + BORDER_PADDING) ||
        isLimitUp(ball.center.y - ball.radius - BORDER_PADDING))
        ball.direction.y = -ball.direction.y;

    else if (
        isLimitLeft(ball.center.x - ball.radius - BORDER_PADDING) ||
        isLimitRight(ball.center.x + ball.radius + BORDER_PADDING))
    {
        bool playerRWin = isLimitLeft(ball.center.x + ball.radius);
        if (playerRWin)
            playerR.score++;
        else
            playerL.score++;

        gameState = STATE_PAUSE;
        setUpGame(playerRWin ? 180 : 0);
        return;
    }
    else
    {
        if (CheckCollisionCircleRec(ball.center, ball.radius, playerR.racket))
        {
            float bounce = racketBounce(playerR.racket, -1);
            ball.direction.y = bounce - (playerR.angle);
        }

        else if (CheckCollisionCircleRec(ball.center, ball.radius, playerL.racket))
        {
            float bounce = racketBounce(playerL.racket, 1);
            ball.direction.y = bounce + (playerL.angle);
        }
    }

    Vector2 dir = {.x = cos(degToRad(ball.direction.y)), .y = sin(degToRad(ball.direction.y))};

    ball.center.y += (ball.direction.x * SPEED_SCALE) * dir.y;
    ball.center.x += (ball.direction.x * SPEED_SCALE) * dir.x;
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

void rayTraceBall(float side, Vector2 dir, Vector2 start)
{
    if (side <= 0)
        return;
    if (dir.y == 0)
        return; // balle horizontale, pas de rebond
    if (dir.x == 0)
        return; // balle verticale, jamais de side
    // descend
    if (dir.y >= 0)
    {
        float bottom = (SCREEN_HEIGHT - start.y) / dir.y;
        float min = fminf(side, bottom);
        Vector2 impact = {start.x + min * dir.x, start.y + min * dir.y};
        DrawLineV(start, impact, GREEN);
        if (min == side)
            return;
        Vector2 dirImpact = {.x = dir.x, .y = -dir.y};
        rayTraceBall(side - bottom, dirImpact, impact);
    }
    // monte
    else
    {
        float top = start.y / -dir.y;
        float min = fminf(side, top);
        Vector2 impact = {start.x + min * dir.x, start.y + min * dir.y};
        DrawLineV(start, impact, GREEN);
        if (min == side)
            return;
        Vector2 dirImpact = {.x = dir.x, .y = -dir.y};
        rayTraceBall(side - top, dirImpact, impact);
    }
}
// -----------------------------------------------