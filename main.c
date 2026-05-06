/*******************************************************************************************
 *
 *   raylib [shapes] example - collision area
 *
 *   Example complexity rating: [★★☆☆] 2/4
 *
 *   Example originally created with raylib 2.5, last time updated with raylib 2.5
 *
 *   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
 *   BSD-like license that allows static linking with closed source software
 *
 *   Copyright (c) 2013-2025 Ramon Santamaria (@raysan5)
 *
 ********************************************************************************************/

#include "raylib.h"
#include "stdio.h"
#include "stdbool.h"

#define BOARD_WIDTH 100
#define BOARD_HEIGHT 100
#define CELL_SIZE 10

int board[BOARD_WIDTH][BOARD_HEIGHT] = {0};

int buffer[BOARD_WIDTH][BOARD_HEIGHT] = {0};

int aliveNeighbours = 0;

void initBoardRandom();

void copyBoardToBuffer();

int drawNeighbours(int i, int j);

void handleMouseMove();

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    const int screenWidth = BOARD_WIDTH * CELL_SIZE;
    const int screenHeight = BOARD_HEIGHT * CELL_SIZE;

    SetConfigFlags(FLAG_WINDOW_HIGHDPI);

    InitWindow(screenWidth, screenHeight, "Game of life");

    SetTargetFPS(10);

    initBoardRandom();

    //----------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // DPI scaling
        printf("screen: %i x %i\n", GetScreenWidth(), GetScreenHeight());
        printf("board:  %i x %i\n", BOARD_WIDTH * CELL_SIZE, BOARD_HEIGHT * CELL_SIZE);
        for (int i = 0; i < BOARD_WIDTH; i++)
        {

            for (int j = 0; j < BOARD_HEIGHT; j++)
            {

                /*
                Underpopulation: A live cell with < 2 neighbors dies.
                Survival: A live cell with 2 or 3 neighbors lives.
                Overpopulation: A live cell with > 3 neighbors dies.
                Reproduction: A dead cell with exactly 3 neighbors becomes a live
                */

                aliveNeighbours = drawNeighbours(i, j);

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

        copyBoardToBuffer();

        handleMouseMove();
        // drawNeighbours(cellX, cellY);

        DrawFPS(1, 1);

        EndDrawing();
        //-----------------------------------------------------
    }

    // De-Initialization
    //---------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //----------------------------------------------------------

    return 0;
}

int drawNeighbours(int i, int j)
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

            neighbourX = dx + i;
            neighbourY = dy + j;

            if (neighbourX == -1)
            {
                neighbourX = BOARD_WIDTH - 1;
            }
            else if (neighbourX == BOARD_WIDTH)
            {
                neighbourX = 0;
            }

            if (neighbourY == -1)
            {
                neighbourY = BOARD_HEIGHT - 1;
            }
            else if (neighbourY == BOARD_HEIGHT)
            {
                neighbourY = 0;
            }

            aliveNeighbours += buffer[neighbourX][neighbourY];
        }
    }

    return aliveNeighbours;
}

void initBoardRandom(void)
{
    for (int i = 0; i < BOARD_WIDTH; i++)
        for (int j = 0; j < BOARD_HEIGHT; j++)
        {

            int r = GetRandomValue(0, 1);
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

void handleMouseMove()
{

    Vector2 mouse = GetMousePosition();

    int cellX = (int)mouse.x / CELL_SIZE;
    int cellY = (int)mouse.y / CELL_SIZE;

    printf("i = %i | j = %i  <|>  x = %i | y = %i\n",
           (int)(mouse.x / CELL_SIZE), (int)(mouse.y / CELL_SIZE), (int)mouse.x, (int)mouse.y);

    DrawRectangle(cellX * CELL_SIZE, cellY * CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);

    buffer[cellX][cellY] = 1;
}