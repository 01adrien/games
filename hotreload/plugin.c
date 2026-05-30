#include "plugin.h"

int Draw(void *ctx)
{
    DrawLineV((Vector2){.x = 10, .y = 10}, (Vector2){.x = 100, .y = 100}, RED);
    DrawLineV((Vector2){.x = 100, .y = 100}, (Vector2){.x = 200, .y = 200}, BLUE);
}
