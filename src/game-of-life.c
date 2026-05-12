#include "raylib.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"

#define BOARD_WIDTH 50
#define BOARD_HEIGHT 50
#define CELL_SIZE 10
#define IMAGE_SIZE 13
#define SPRITES_COUNT 10

void initBoardRandom(int n);
void copyBoardToBuffer();
int neighbours(int i, int j);
void onMouseClickFunc(int x, int y);
void onMouseMoveFunc(int x, int y);
void handleMouse(uint8_t *bytes, void (*func)(int, int));
void drawLife();
int handleKeyPressed(int index);
int getBoardPos(int pos, int boundary);

int board[BOARD_WIDTH][BOARD_HEIGHT] = {0};
int buffer[BOARD_WIDTH][BOARD_HEIGHT] = {0};
Image sprites[SPRITES_COUNT];

void gameOfLife()
{
    const int screenWidth = BOARD_WIDTH * CELL_SIZE;
    const int screenHeight = BOARD_HEIGHT * CELL_SIZE;
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(screenWidth, screenHeight, "Game of life");
    SetTargetFPS(10);

    int selectedSprite = 0;

    char spritePath[128];

    for (size_t i = 0; i < SPRITES_COUNT; i++)
    {
        sprintf(spritePath, "./assets/sprite%zu.png", i + 1);
        sprites[i] = LoadImage(spritePath);

        if (sprites[i].data == NULL)
        {
            printf("Erreur chargement sprite %zu\n", i);
        }
    }
    uint8_t *spriteAsBytes = (uint8_t *)sprites[selectedSprite].data;

    initBoardRandom(0);

    while (!WindowShouldClose())
    {

        BeginDrawing();

        ClearBackground(RAYWHITE);
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT))
        {
            selectedSprite = handleKeyPressed(selectedSprite);
            spriteAsBytes = (uint8_t *)sprites[selectedSprite].data;
        }

        drawLife();
        copyBoardToBuffer();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            handleMouse(spriteAsBytes, onMouseClickFunc);
        handleMouse(spriteAsBytes, onMouseMoveFunc);
        ImageDrawPixel(&sprites[selectedSprite], 0, BOARD_HEIGHT * CELL_SIZE, BLACK);
        DrawFPS(1, 1);
        EndDrawing();
    }
    CloseWindow();
}

int neighbours(int i, int j)
{
    if (j < 0 || j >= BOARD_HEIGHT || i < 0 || i >= BOARD_WIDTH)
    {
        return -1;
    }

    int aliveNeighbours = 0;
    int neighbourX;
    int neighbourY;
    for (int dx = -1; dx <= 1; dx++)
    {

        for (int dy = -1; dy <= 1; dy++)
        {

            if (dx == 0 && dy == 0)
            {
                continue;
            }

            neighbourX = getBoardPos(dx + i, BOARD_WIDTH);
            neighbourY = getBoardPos(dy + j, BOARD_HEIGHT);

            aliveNeighbours += buffer[neighbourX][neighbourY];
        }
    }

    return aliveNeighbours;
}

int getBoardPos(int pos, int boundary)
{

    if (pos <= -1)
        pos = boundary - (-pos);
    else if (pos >= boundary)
        pos = pos - boundary;

    return pos;
}

void drawLife()
{
    for (int i = 0; i < BOARD_WIDTH; i++)
    {

        for (int j = 0; j < BOARD_HEIGHT; j++)
        {
            int aliveNeighbours = neighbours(i, j);

            if (buffer[i][j] == 1) // alive
            {
                if (aliveNeighbours < 2 || aliveNeighbours > 3)
                {
                    board[i][j] = 0;
                }
                else
                {
                    board[i][j] = 1;
                }
            }
            else // dead
            {
                if (aliveNeighbours == 3)
                {
                    board[i][j] = 1;
                }
                else
                {
                    board[i][j] = 0;
                }
            }

            // RENDER

            int x = i * CELL_SIZE;
            int y = j * CELL_SIZE;
            if (buffer[i][j] == 1)
            {
                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, RED);
            }
            else
            {
                DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, BLACK);
            }
        }
    }
}

void initBoardRandom(int n)
{
    for (int i = 0; i < BOARD_WIDTH; i++)
        for (int j = 0; j < BOARD_HEIGHT; j++)
        {

            int r = GetRandomValue(0, n);
            board[i][j] = r;
            buffer[i][j] = r;
        }
}

void copyBoardToBuffer()
{
    for (int i = 0; i < BOARD_WIDTH; i++)
    {
        for (int j = 0; j < BOARD_HEIGHT; j++)
        {
            buffer[i][j] = board[i][j];
        }
    }
}

void onMouseClickFunc(int x, int y)
{
    buffer[x][y] = 1;
}

void onMouseMoveFunc(int x, int y)
{
    DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, GREEN);
}

void handleMouse(uint8_t *bytes, void (*func)(int, int))
{

    Vector2 mouse = GetMousePosition();

    int i = (int)mouse.x / CELL_SIZE;
    int j = (int)mouse.y / CELL_SIZE;
    if (i >= 0 && i <= BOARD_WIDTH && j >= 0 && j <= BOARD_HEIGHT)
    {

        for (size_t k = 0; k < IMAGE_SIZE; k++)
        {
            for (size_t l = 0; l < IMAGE_SIZE; l++)
            {
                if (bytes[k + l * IMAGE_SIZE] > 128)
                {
                    int x = getBoardPos(i + k - (IMAGE_SIZE / 2), BOARD_WIDTH);
                    int y = getBoardPos(j + l - (IMAGE_SIZE / 2) + 1, BOARD_HEIGHT);
                    func(x, y);
                }
            }
        }
    }
}

int handleKeyPressed(int index)
{
    if (IsKeyPressed(KEY_RIGHT))
    {
        if (index < SPRITES_COUNT)
        {
            return index + 1;
        }
    }
    else
    {
        if (index > 0)
        {
            return index - 1;
        }
    }

    return index;
}