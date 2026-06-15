#include "raycaster.h"

Texture2D textWood;

int level1[24][24] = {
    {1, 1, 1, 1, 1, 1, 6, 1, 1, 1, 2, 6, 2, 1, 1, 1, 1, 1, 6, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 6, 1, 1, 1, 1, 2, 6, 2, 1, 1, 1, 1, 1, 6, 1, 1, 1, 1, 1}};
void setupContext(GameContext *ctx)
{
    // Setup map

    float miniMapWidth = MAP_WIDTH * TILE_SIZE * MAP_SCALE;
    float miniMapHeight = MAP_HEIGHT * TILE_SIZE * MAP_SCALE;

    int xMap = 0;
    int yMap = 0;
    ctx->mapPos = (Vector2){.x = xMap, .y = yMap};
    ctx->map = &level1[0][0];

    // Setup player
    int xPlayer = xMap + (MAP_WIDTH * TILE_SIZE * 0.75);
    int yPlayer = yMap + (MAP_HEIGHT * TILE_SIZE * 0.75);
    ctx->player.dir = (Vector2){.x = 1, .y = 0};
    ctx->player.pos = (Vector2){.x = xPlayer, .y = yPlayer};
    ctx->player.vel = (Vector2){.x = 0, .y = 0};
    ctx->player.angle = PI;
    ctx->player.move.rot = ROT_NONE;
    ctx->player.move.dir = DIR_NONE;
    ctx->player.radius = PLAYER_RADIUS;

    ctx->player.screen = (Rectangle){
        .x = (WIDTH - PLAYER_SCREEN_WIDTH) / 2,
        .y = (HEIGHT - PLAYER_SCREEN_HEIGHT) / 2,
        .width = PLAYER_SCREEN_WIDTH,
        .height = PLAYER_SCREEN_HEIGHT,
    };

    ctx->textures[TILE_GREYSTONE] = LoadTexture("../build/textures/greystone.png");
    ctx->textures[TILE_BLUESTONE] = LoadTexture("../build/textures/bluestone.png");
    ctx->textures[TILE_COLORSTONE] = LoadTexture("../build/textures/colorstone.png");
    ctx->textures[TILE_WOOD] = LoadTexture("../build/textures/wood.png");
    ctx->textures[TILE_BRICK] = LoadTexture("../build/textures/redbrick.png");
    ctx->textures[TILE_EAGLE] = LoadTexture("../build/textures/eagle.png");
}

int main(int argc, char const *argv[])
{
    InitWindow(WIDTH, HEIGHT, "Raycaster");
    SetTargetFPS(10);

    void *plugin_handle = NULL;
    DrawFuncPtr game = NULL;
    const char *file = "../build/raycaster_plugin.so";
    long lastPluginTime = 0;
    GameContext ctx;

    setupContext(&ctx);

    if (reload_plugin(file, &lastPluginTime, &game, &plugin_handle))
        return 1;

    while (!WindowShouldClose())
    {
        if (should_reload(file, &lastPluginTime))
            reload_plugin(file, &lastPluginTime, &game, &plugin_handle);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        game(&ctx);
        EndDrawing();
    }

    for (size_t i = 0; i < TILE_COUNT; i++)
    {
        UnloadTexture(ctx.textures[i]);
    }

    return 0;
}
