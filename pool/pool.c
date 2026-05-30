#include "pool.h"

void setupGame(GameContext *ctx)
{

    ctx->setupGame = setupGame;

    ctx->pool.table.height = TABLE_HEIGHT;
    ctx->pool.table.width = TABLE_WIDTH;
    ctx->pool.table.x = WIDTH / 3 / 2;
    ctx->pool.table.y = HEIGHT / 3 / 2;
    int gap = 0;
    // holes setup
    Vector2 center = (Vector2){.x = ctx->pool.table.x + gap, .y = ctx->pool.table.y + gap};
    for (size_t i = 0; i < MAX_HOLES; i++)
    {
        ctx->pool.holes[i].center = center;
        ctx->pool.holes[i].radius = HOLE_RADIUS;
        if (i == (MAX_HOLES / 2) - 1)
            center = (Vector2){
                .x = ctx->pool.table.x + gap,
                .y = ctx->pool.table.y + TABLE_HEIGHT - gap,
            };
        else
            center.x += (TABLE_WIDTH) / 2 - gap;
    }

    int xm = WIDTH / 3;
    int ym = HEIGHT / 2;

    ctx->pool.balls[0] = mkBall(xm, ym, WHITE);
    ctx->whiteBall = &ctx->pool.balls[0];

    xm += 300;

    int ballIndex = 1;

    for (size_t i = 0; i < 5; i++)
    {
        for (size_t j = 0; j <= i; j++)
        {
            int x = xm + (i * BALL_RADIUS * 2);
            int y = ym + (j * BALL_RADIUS * 2) - (i * BALL_RADIUS);

            ctx->pool.balls[ballIndex++] = mkBall(x, y, RED);
        }
    }

    float angle = PI;
    ctx->pool.player[0].cue.length = CUE_LENGTH;
    ctx->pool.player[0].cue.dir = Vector2Normalize((Vector2){.x = cosf(angle), .y = sinf(angle)});
    ctx->pool.player[0].cue.pullBack = 0;
    ctx->pool.player[0].cue.pos = (Vector2){

        .x = ctx->pool.balls[0].center.x - (BALL_RADIUS * 2),
        .y = ctx->pool.balls[0].center.y,
    };
    ctx->pool.player[0].cue.state = CUE_IDLE;
    ctx->pool.player[0].cue.angle = angle;

    ctx->currentPlayer = &ctx->pool.player[0];
}

void drawPool(GameContext *ctx)
{
    drawTable(ctx);
    for (size_t i = 0; i < MAX_BALLS; i++)
    {
        drawBall(ctx->pool.balls[i]);
    }
    drawCue(ctx->currentPlayer->cue);
}

void drawTable(GameContext *ctx)
{
    Rectangle table = ctx->pool.table;

    Rectangle outerRect = {
        table.x - TABLE_BORDER,
        table.y - TABLE_BORDER,
        table.width + TABLE_BORDER * 2,
        table.height + TABLE_BORDER * 2};

    // Ombre globale
    DrawRectangleRounded(
        (Rectangle){
            outerRect.x + 8,
            outerRect.y + 8,
            outerRect.width,
            outerRect.height},
        0.04f,
        24,
        Fade(BLACK, 0.25f));

    // Bois externe
    DrawRectangleRounded(outerRect, 0.04f, 24, (Color){92, 51, 23, 255});

    // Bord intérieur plus clair
    DrawRectangleRoundedLinesEx(outerRect, 0.04f, 24, 6, (Color){140, 90, 50, 255});

    // Tapis
    DrawRectangleRounded(table, 0.03f, 24, (Color){18, 120, 75, 255});

    // Contour tapis
    DrawRectangleRoundedLinesEx(table, 0.03f, 24, 3, Fade(BLACK, 0.25f));

    for (size_t i = 0; i < MAX_HOLES; i++)
        DrawCircleV(ctx->pool.holes[i].center, HOLE_RADIUS, BLACK);
}

void drawBall(Ball ball)
{
    // Ombre
    DrawCircleV((Vector2){ball.center.x + 4, ball.center.y + 4}, ball.radius, Fade(BLACK, 0.25f));

    // Corps principal
    DrawCircleV(ball.center, ball.radius, ball.color);

    // Highlight
    DrawCircleV(
        (Vector2){
            ball.center.x - ball.radius * 0.35f,
            ball.center.y - ball.radius * 0.35f},
        ball.radius * 0.25f,
        Fade(WHITE, 0.7f));

    // Contour subtil
    DrawCircleLinesV(ball.center, ball.radius, Fade(BLACK, 0.25f));
}

void drawCue(Cue cue)
{
    if (cue.state != CUE_HIT)
    {
        printVector2(cue.pos);
        float length = cue.length;
        Vector2 end = Vector2Add(cue.pos, Vector2Scale(cue.dir, length));
        DrawLineEx(
            (Vector2){cue.pos.x + 4, cue.pos.y + 4},
            (Vector2){end.x + 4, end.y + 4},
            10,
            Fade(BLACK, 0.25f));
        DrawLineEx(cue.pos, end, 8, (Color){160, 110, 60, 255});

        Vector2 gripStart = Vector2Lerp(cue.pos, end, 0.75f);
        DrawLineEx(gripStart, end, 10, (Color){60, 30, 20, 255});
        DrawLineEx(
            (Vector2){cue.pos.x - 1, cue.pos.y - 1},
            (Vector2){end.x - 1, end.y - 1},
            2,
            Fade(WHITE, 0.18f));
    }
}

void moveCue(GameContext *ctx)
{
    Cue *cue = &ctx->currentPlayer->cue;
    Ball *whiteBall = ctx->whiteBall;

    switch (cue->state)
    {
    case CUE_IDLE:
    case CUE_HIT:
        break;

    case CUE_PULL_BACK:
        cue->pullBack += cue->pullBack >= MAX_PULL_BACK ? 0 : 1;
        break;

    case CUE_RELEASE:
        cue->pullBack -= cue->pullBack > 0 ? 1 : 0;
        break;

    case CUE_SHOOT:
        if (isNotMoving(*whiteBall))
        {
            whiteBall->velocity = Vector2Scale(cue->dir, -ctx->currentPlayer->cue.pullBack / 2);
        }

        if (CheckCollisionPointCircle(cue->pos, whiteBall->center, whiteBall->radius))
        {
            cue->state = CUE_HIT;
            whiteBall->state = BALL_MOVING;
        }
        else
            cue->pullBack -= 6;

        break;

    case CUE_ROTATE_CK:
        cue->angle += 0.03f;
        break;

    case CUE_ROTATE_ACK:
        cue->angle -= 0.03f;
        break;

    default:
        printf("Unknow cue state\n");
        break;
    }

    cue->dir = Vector2Normalize((Vector2){
        cosf(cue->angle),
        sinf(cue->angle)});

    float distance = BALL_RADIUS * 2 + cue->pullBack;

    cue->pos = Vector2Subtract(
        whiteBall->center,
        Vector2Scale(cue->dir, -distance));
}

void moveBalls(GameContext *ctx)
{

    int moving = MAX_BALLS;

    for (size_t i = 0; i < MAX_BALLS; i++)
    {
        Ball *b = &ctx->pool.balls[i];
        if (b->state == BALL_MOVING)
        {
            moving++;
            if (isNotMoving(*b))
            {
                moving--;
                b->state = BALL_IDLE;
            }
            b->center = Vector2Add(b->center, b->velocity);
            b->velocity = Vector2Scale(b->velocity, 0.99);
            checkHitBorder(ctx, b);
            for (size_t j = 0; j < MAX_BALLS; j++)
            {
                if (i == j)
                    continue;
                Ball *o = &ctx->pool.balls[j];
                if (CheckCollisionCircles(b->center, b->radius, o->center, o->radius))
                {
                    resolveBallCollision(b, o);
                    o->state = BALL_MOVING;
                }
            }
        }
        else
            moving--;
    }

    if (moving == 0 && ctx->currentPlayer->cue.state == CUE_HIT)
    {
        ctx->currentPlayer->cue.state = CUE_IDLE;
        ctx->currentPlayer->cue.pullBack = 0;
    }
}

void handleInput(GameContext *ctx)
{
    if (!isShooting(*(ctx->currentPlayer)))
    {
        if (IsKeyDown(KEY_LEFT))
            ctx->currentPlayer->cue.state = CUE_ROTATE_CK;

        else if (IsKeyDown(KEY_RIGHT))
            ctx->currentPlayer->cue.state = CUE_ROTATE_ACK;

        else if (IsKeyDown(KEY_DOWN))
            ctx->currentPlayer->cue.state = CUE_PULL_BACK;

        else if (IsKeyDown(KEY_UP))
            ctx->currentPlayer->cue.state = CUE_RELEASE;

        else if (IsKeyDown(KEY_SPACE) && ctx->currentPlayer->cue.pullBack > 0)
            ctx->currentPlayer->cue.state = CUE_SHOOT;
        else
            ctx->currentPlayer->cue.state = CUE_IDLE;
    }
    if (IsKeyDown(KEY_BACKSPACE))
        setupGame(ctx);
}

void gameLoop(GameContext *ctx)
{
    handleInput(ctx);
    if (ctx->whiteBall->state != BALL_MOVING)
        moveCue(ctx);
    moveBalls(ctx);
    drawPool(ctx);
}

bool isShooting(Player player)
{
    return player.cue.state == CUE_SHOOT || player.cue.state == CUE_HIT;
}

void checkHitBorder(GameContext *ctx, Ball *ball)
{

    if ((ball->center.x + ball->radius) >= (ctx->pool.table.x + ctx->pool.table.width))
        ball->velocity = Vector2Reflect(ball->velocity, (Vector2){.x = -1, .y = 0});

    else if ((ball->center.y + ball->radius) >= (ctx->pool.table.y + ctx->pool.table.height))
        ball->velocity = Vector2Reflect(ball->velocity, (Vector2){.x = 0, .y = 1});

    else if ((ball->center.y - ball->radius) <= ctx->pool.table.y)
        ball->velocity = Vector2Reflect(ball->velocity, (Vector2){.x = 0, .y = -1});

    else if ((ball->center.x - ball->radius) <= ctx->pool.table.x)
        ball->velocity = Vector2Reflect(ball->velocity, (Vector2){.x = 1, .y = 0});
}

bool isNotMoving(Ball ball)
{
    return (int)ball.velocity.x == 0 && (int)ball.velocity.y == 0;
}

Ball mkBall(float x, float y, Color color)
{
    return (Ball){
        .center.x = x,
        .center.y = y,
        .color = color,
        .velocity = (Vector2){0, 0},
        .radius = BALL_RADIUS,
        .state = BALL_IDLE,
        .number = 0,
    };
}

void resolveBallCollision(Ball *a, Ball *b)
{
    Vector2 impact = Vector2Subtract(b->center, a->center);
    int distance = Vector2Length(impact);
    Vector2 normalImpact = Vector2Normalize(impact);
    int overlap = (BALL_RADIUS * 2) - distance;
    if (overlap)
    {
        a->center = Vector2Subtract(a->center, Vector2Scale(normalImpact, overlap * .5f));
        b->center = Vector2Subtract(b->center, Vector2Scale(normalImpact, overlap * .5f));
    }
    Vector2 relativeVelocity = Vector2Subtract(a->velocity, b->velocity);
    float impulse = Vector2DotProduct(relativeVelocity, normalImpact);
    if (impulse <= 0)
        return;
    a->velocity = Vector2Subtract(a->velocity, Vector2Scale(normalImpact, impulse));
    b->velocity = Vector2Add(b->velocity, Vector2Scale(normalImpact, impulse));
}

int main(int argc, char const *argv[])
{
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WIDTH, HEIGHT, "Pool");
    SetTargetFPS(40);
    GameContext ctx;
    setupGame(&ctx);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        gameLoop(&ctx);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

void printVector2(Vector2 v)
{
    printf("x = %.2f  |  y = %.2f\n", v.x, v.y);
}
