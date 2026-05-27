#include "arkanoid.h"

Paddle paddle;
Breaker breaker;

Block blocks[BLOCK_AREA_WIDTH / BLOCK_WIDTH][BLOCK_AREA_HEIGHT / BLOCK_HEIGHT] = {0};

const Color blockTypeToColors[] = {
    [BLOCK_EASY] = YELLOW,
    [BLOCK_MEDIUM] = BLUE,
    [BLOCK_HARD] = RED,
};

const int blockTypeToPv[] = {
    [BLOCK_EASY] = 1,
    [BLOCK_MEDIUM] = 2,
    [BLOCK_HARD] = 3,
};

const Level levels[] = {
    levelOne,
    levelTwo,
    levelThree,
    levelFour,
    levelFive,
    levelSix,
};

// -----
// GAME RELATED FUNCTIONS
// -----

void inputHandler()
{
    if (IsKeyPressed(KEY_RIGHT) || IsKeyDown(KEY_RIGHT))
        paddle.move = MOVE_RIGHT;
    else if (IsKeyPressed(KEY_LEFT) || IsKeyDown(KEY_LEFT))
        paddle.move = MOVE_LEFT;
    else if (IsKeyPressed(KEY_SPACE) || IsKeyDown(KEY_SPACE))
        breaker.moving = !breaker.moving ? true : breaker.moving;
    else if (IsKeyPressed(KEY_UP))
    {
        if (paddle.level < MAX_LEVEL)
        {
            setUpGame(paddle.level + 1);
        }
    }
    else if (IsKeyPressed(KEY_DOWN))
    {

        if (paddle.level > 1)
        {
            setUpGame(paddle.level - 1);
        }
    }

    else
        paddle.move = MOVE_NONE;
}

void setUpGame(int level)
{
    paddle.score = 0;
    paddle.speed = 0;
    paddle.move = MOVE_NONE;
    paddle.p.x = WIDTH / 2 - PADDLE_WIDTH / 2;
    paddle.p.y = HEIGHT - PADDLE_HEIGHT;
    paddle.p.height = PADDLE_HEIGHT;
    paddle.p.width = PADDLE_WIDTH;
    paddle.level = level;
    paddle.destroyedBlocks = 0;

    breaker.center = (Vector2){.x = WIDTH / 2, .y = HEIGHT - PADDLE_HEIGHT - BREAKER_RADIUS};
    breaker.radius = BREAKER_RADIUS;
    breaker.direction = Vector2Normalize((Vector2){1, -1});
    breaker.moving = false;
    breaker.hit = false;

    initBlocks(levels[paddle.level - 1]);
}

void gameLoop()
{
    inputHandler();
    drawGame();
    checkColision();
    movePaddle();
    moveBreaker();
}

void drawGame()
{

    DrawRectangleRec(paddle.p, BLACK);
    DrawCircleV(breaker.center, breaker.radius, BLACK);
    for (size_t i = 0; i < BLOCK_AREA_WIDTH / BLOCK_WIDTH; i++)
    {
        for (size_t j = 0; j < BLOCK_AREA_HEIGHT / BLOCK_HEIGHT; j++)
        {
            int x = i * BLOCK_WIDTH;
            int y = j * BLOCK_HEIGHT;
            Block block = blocks[i][j];
            if (block.pv > 0)
            {
                DrawRectangle(x, y, BLOCK_WIDTH, BLOCK_HEIGHT, blockTypeToColors[block.type]);
                DrawRectangleLines(x, y, BLOCK_WIDTH, BLOCK_HEIGHT, BLACK);
            }
        }
    }
}

// -----
// PHYSICS RELATED FUNCTIONS
// -----

void movePaddle()
{
    if (paddle.move != MOVE_NONE)
        paddle.speed += PADDLE_SPEED * paddle.move;
    else
        paddle.speed *= FRICTION;

    paddle.p.x += paddle.speed;
    if (!breaker.moving)
        breaker.center.x = paddle.p.x + PADDLE_WIDTH / 2;
}

void moveBreaker()
{
    if (breaker.moving)
    {
        breaker.center.x += breaker.direction.x * BREAKER_SPEED;
        breaker.center.y += breaker.direction.y * BREAKER_SPEED;
    }
}

void checkColision()
{
    // PADDLE WALL COLLISION
    if (paddle.p.x <= 0)
    {
        paddle.move = MOVE_NONE;
        paddle.speed = 0;
        paddle.p.x = 1;
    }
    else if ((paddle.p.x + PADDLE_WIDTH) >= WIDTH)
    {
        paddle.move = MOVE_NONE;
        paddle.p.x = WIDTH - PADDLE_WIDTH - 1;
        paddle.speed = 0;
    }

    Vector2 normal;
    // BREAKER WALL COLLISION
    if (isLimitDown(breaker.center.y + breaker.radius))
    {
        setUpGame(paddle.level);
    }
    else if (isLimitUp(breaker.center.y - breaker.radius))
    {
        normal = (Vector2){.x = 0, .y = -1};
        breaker.direction = Vector2Reflect(breaker.direction, normal);
    }
    else if (isLimitRight(breaker.center.x + breaker.radius))
    {
        normal = (Vector2){.x = -1, .y = 0};
        breaker.direction = Vector2Reflect(breaker.direction, normal);
    }
    else if (isLimitLeft(breaker.center.x - breaker.radius))
    {
        normal = (Vector2){.x = 1, .y = 0};
        breaker.direction = Vector2Reflect(breaker.direction, normal);
    }
    else if (CheckCollisionCircleRec(breaker.center, breaker.radius, paddle.p))
    {
        normal = (Vector2){.x = 0, .y = -1};
        breaker.direction = Vector2Reflect(breaker.direction, normal);
        breaker.hit = false;
    }

    // BREAKER BLOCKS COLISION
    for (size_t i = 0; i < BLOCK_AREA_WIDTH / BLOCK_WIDTH; i++)
    {
        for (size_t j = 0; j < BLOCK_AREA_HEIGHT / BLOCK_HEIGHT; j++)
        {
            int x = i * BLOCK_WIDTH;
            int y = j * BLOCK_HEIGHT;
            Rectangle rec = {.x = x, .y = y, .height = BLOCK_HEIGHT, .width = BLOCK_WIDTH};
            Block *block = &blocks[i][j];
            if (
                block->pv && !breaker.hit &&
                CheckCollisionCircleRec(breaker.center, breaker.radius, rec))
            {
                block->pv -= 1;
                block->type -= 1;
                breaker.direction = Vector2Reflect(breaker.direction, colisionSide(rec));
                DrawRectangleRec(rec, RED);
                // breaker.hit = true;
                if (!block->pv)
                    paddle.destroyedBlocks++;
                if (paddle.destroyedBlocks == ((BLOCK_AREA_WIDTH / BLOCK_WIDTH) * (BLOCK_AREA_HEIGHT / BLOCK_HEIGHT)))
                    setUpGame(paddle.level + 1);
            }
        }
    }
}

Vector2 colisionSide(Rectangle rec)
{
    Vector2 p1 = {.x = rec.x, .y = rec.y};
    Vector2 p2 = {.x = rec.x, .y = rec.y + rec.height};

    // left
    if (CheckCollisionCircleLine(breaker.center, breaker.radius, p1, p2))
        return (Vector2){.x = 1, .y = 0};

    // top
    p2 = (Vector2){.x = rec.x + rec.width, .y = rec.y};
    if (CheckCollisionCircleLine(breaker.center, breaker.radius, p1, p2))
        return (Vector2){.x = 0, .y = -1};

    // right
    p1 = (Vector2){.x = rec.x + rec.width, .y = rec.y + rec.height};
    if (CheckCollisionCircleLine(breaker.center, breaker.radius, p1, p2))
        return (Vector2){.x = -1, .y = 0};

    // bottom
    p2 = (Vector2){.x = rec.x, .y = rec.y + rec.height};
    if (CheckCollisionCircleLine(breaker.center, breaker.radius, p1, p2))
        return (Vector2){.x = 0, .y = -1};
}

bool isLimitUp(int y)
{
    return y <= 0;
}

bool isLimitDown(int y)
{
    return y >= HEIGHT;
}

bool isLimitRight(int x)
{
    return x >= WIDTH;
}

bool isLimitLeft(int x)
{
    return x <= 0;
}

// -----
// BLOCKS DESIGN RELATED FUNCTIONS
// ----

Block mkBlock(BlockType type)
{
    return (Block){
        .type = type,
        .pv = blockTypeToPv[type],
    };
}

void initBlocks(Level level)
{
    for (size_t i = 0; i < BLOCK_AREA_WIDTH / BLOCK_WIDTH; i++)
    {
        for (size_t j = 0; j < BLOCK_AREA_HEIGHT / BLOCK_HEIGHT; j++)
        {
            blocks[i][j] = level(i, j);
        }
    }
}

Block levelOne(int i, int j)
{
    if (j < 3)
        return mkBlock(BLOCK_EASY);

    if (j < 6)
        return mkBlock(BLOCK_MEDIUM);

    return mkBlock(BLOCK_HARD);
}

Block levelTwo(int i, int j)
{
    if ((i + j) % 2 == 0)
        return mkBlock(BLOCK_EASY);

    return mkBlock(BLOCK_MEDIUM);
}

Block levelThree(int i, int j)
{
    int cx = (BLOCK_AREA_WIDTH / BLOCK_WIDTH) / 2;
    int cy = (BLOCK_AREA_HEIGHT / BLOCK_HEIGHT) / 2;

    int dx = i - cx;
    int dy = j - cy;

    int dist = dx * dx + dy * dy;

    if (dist < 16)
        return mkBlock(BLOCK_HARD);

    if (dist < 36)
        return mkBlock(BLOCK_MEDIUM);

    return mkBlock(BLOCK_EASY);
}

Block levelFour(int i, int j)
{
    int cx = (BLOCK_AREA_WIDTH / BLOCK_WIDTH) / 2;
    int cy = (BLOCK_AREA_HEIGHT / BLOCK_HEIGHT) / 2;

    int dx = i - cx;
    int dy = j - cy;

    int dist = dx * dx + dy * dy;

    if (dist < 16)
        return mkBlock(BLOCK_HARD);

    if (dist < 36)
        return mkBlock(BLOCK_MEDIUM);

    return mkBlock(BLOCK_EASY);
}

Block levelFive(int i, int j)
{
    if ((i + j) % 5 == 0)
        return mkBlock(BLOCK_HARD);

    if ((i - j) % 3 == 0)
        return mkBlock(BLOCK_MEDIUM);

    return mkBlock(BLOCK_EASY);
}

Block levelSix(int i, int j)
{
    int v = (i * j + i + j) % 3;

    return mkBlock(v);
}

int main(int argc, char const *argv[])
{
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WIDTH, HEIGHT, "Arkanoid");
    SetTargetFPS(40);
    setUpGame(1);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        gameLoop();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}