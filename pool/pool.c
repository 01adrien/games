#include "pool.h"

Pool pool;
Player currentPlayer;
Ball *whiteBall = &pool.balls[0];

void setupGame()
{
    // table setup
    pool.table.height = TABLE_HEIGHT;
    pool.table.width = TABLE_WIDTH;
    pool.table.x = WIDTH / 3 / 2;
    pool.table.y = HEIGHT / 3 / 2;
    int gap = 0;
    // holes setup
    Vector2 center = (Vector2){.x = pool.table.x + gap, .y = pool.table.y + gap};
    for (size_t i = 0; i < MAX_HOLES; i++)
    {
        pool.holes[i].center = center;
        pool.holes[i].radius = HOLE_RADIUS;
        if (i == (MAX_HOLES / 2) - 1)
            center = (Vector2){
                .x = pool.table.x + gap,
                .y = pool.table.y + TABLE_HEIGHT - gap,
            };
        else
            center.x += (TABLE_WIDTH) / 2 - gap;
    }

    currentPlayer = pool.player[0];

    float angle = PI;
    currentPlayer.cue.length = CUE_LENGTH;
    currentPlayer.cue.dir = Vector2Normalize((Vector2){.x = cosf(angle), .y = sinf(angle)});
    currentPlayer.cue.pullBack = 0;
    currentPlayer.cue.pos = (Vector2){
        .x = pool.balls[0].center.x - (BALL_RADIUS * 2),
        .y = pool.balls[0].center.y,
    };
    currentPlayer.cue.state = CUE_IDLE;
    currentPlayer.cue.angle = angle;

    int xm = WIDTH / 3;
    int ym = HEIGHT / 2;

    pool.balls[0] = mkBall(xm, ym, WHITE);

    xm += 300;

    int ballIndex = 1;

    for (size_t i = 0; i < 5; i++)
    {
        for (size_t j = 0; j <= i; j++)
        {
            int x = xm + (i * BALL_RADIUS * 2);
            int y = ym + (j * BALL_RADIUS * 2) - (i * BALL_RADIUS);

            pool.balls[ballIndex++] = mkBall(x, y, RED);
        }
    }
};

void drawPool(Pool pool)
{
    drawTable(pool.table);
    for (size_t i = 0; i < MAX_BALLS; i++)
    {
        drawBall(pool.balls[i]);
    }

    drawCue(currentPlayer.cue);
}

void drawTable(Rectangle table)
{
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
        DrawCircleV(pool.holes[i].center, HOLE_RADIUS, BLACK);
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

        float length = cue.length;

        Vector2 end = Vector2Add(cue.pos, Vector2Scale(cue.dir, length));

        // Ombre
        DrawLineEx(
            (Vector2){cue.pos.x + 4, cue.pos.y + 4},
            (Vector2){end.x + 4, end.y + 4},
            10,
            Fade(BLACK, 0.25f));

        // Corps principal bois
        DrawLineEx(cue.pos, end, 8, (Color){160, 110, 60, 255});

        // Partie arrière plus foncée
        Vector2 gripStart = Vector2Lerp(cue.pos, end, 0.75f);

        DrawLineEx(gripStart, end, 10, (Color){60, 30, 20, 255});

        // Highlight
        DrawLineEx(
            (Vector2){cue.pos.x - 1, cue.pos.y - 1},
            (Vector2){end.x - 1, end.y - 1},
            2,
            Fade(WHITE, 0.18f));
    }
}

void moveCue(Cue *cue)
{

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
            whiteBall->velocity = Vector2Scale(cue->dir, -currentPlayer.cue.pullBack / 2);
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
        pool.balls[0].center,
        Vector2Scale(cue->dir, -distance));
}

void moveBalls()
{

    int moving = MAX_BALLS;

    for (size_t i = 0; i < MAX_BALLS; i++)
    {
        Ball *b = &pool.balls[i];
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
            checkHitBorder(b);
            for (size_t j = 0; j < MAX_BALLS; j++)
            {
                if (i == j)
                    continue;
                Ball *o = &pool.balls[j];
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

    if (moving == 0 && currentPlayer.cue.state == CUE_HIT)
    {
        currentPlayer.cue.state = CUE_IDLE;
        currentPlayer.cue.pullBack = 0;
    }
}

void handleInput()
{
    if (!isShooting(currentPlayer))
    {
        if (IsKeyDown(KEY_LEFT))
            currentPlayer.cue.state = CUE_ROTATE_CK;

        else if (IsKeyDown(KEY_RIGHT))
            currentPlayer.cue.state = CUE_ROTATE_ACK;

        else if (IsKeyDown(KEY_DOWN))
            currentPlayer.cue.state = CUE_PULL_BACK;

        else if (IsKeyDown(KEY_UP))
            currentPlayer.cue.state = CUE_RELEASE;

        else if (IsKeyDown(KEY_SPACE) && currentPlayer.cue.pullBack > 0)
            currentPlayer.cue.state = CUE_SHOOT;
        else
            currentPlayer.cue.state = CUE_IDLE;
    }
    if (IsKeyDown(KEY_BACKSPACE))
        setupGame();
}

void gameLoop()
{
    handleInput();
    if (whiteBall->state != BALL_MOVING)
        moveCue(&currentPlayer.cue);
    moveBalls();
    drawPool(pool);
}

bool isShooting(Player player)
{
    return player.cue.state == CUE_SHOOT || player.cue.state == CUE_HIT;
}

int main(int argc, char const *argv[])
{
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WIDTH, HEIGHT, "Arkanoid");
    SetTargetFPS(40);
    setupGame();
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

void printVector2(Vector2 v)
{
    printf("x = %.2f  |  y = %.2f\n", v.x, v.y);
}

void checkHitBorder(Ball *ball)
{

    if ((ball->center.x + ball->radius) >= (pool.table.x + pool.table.width))
        ball->velocity = Vector2Reflect(ball->velocity, (Vector2){.x = -1, .y = 0});

    else if ((ball->center.y + ball->radius) >= (pool.table.y + pool.table.height))
        ball->velocity = Vector2Reflect(ball->velocity, (Vector2){.x = 0, .y = 1});

    else if ((ball->center.y - ball->radius) <= pool.table.y)
        ball->velocity = Vector2Reflect(ball->velocity, (Vector2){.x = 0, .y = -1});

    else if ((ball->center.x - ball->radius) <= pool.table.x)
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
    // direction du choc (A -> B)
    Vector2 delta = Vector2Subtract(b->center, a->center);

    // distance entre les 2 centre
    float dist = Vector2Length(delta);

    if (dist == 0)
        return;

    Vector2 normal = Vector2Normalize(delta);

    float minDist = a->radius + b->radius;

    float overlap = minDist - dist;

    // corrige si les billes rentre un les unes dans les autres
    if (overlap > 0)
    {
        a->center = Vector2Subtract(a->center, Vector2Scale(normal, overlap * 0.5f));
        b->center = Vector2Add(b->center, Vector2Scale(normal, overlap * 0.5f));
    }

    // vitesse relative A vs B
    Vector2 relative = Vector2Subtract(a->velocity, b->velocity);

    // projection sur la normale (dot)
    float push = Vector2DotProduct(relative, normal);

    // elles s’éloignent déjà
    if (push < 0)
        return;

    Vector2 impulse = Vector2Scale(normal, push);

    // A perd vitesse sur l’axe du choc
    a->velocity = Vector2Subtract(a->velocity, impulse);
    // B gagne vitesse sur l’axe du choc
    b->velocity = Vector2Add(b->velocity, impulse);
}