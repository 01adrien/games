#if !defined(PLUGIN_H)
#define PLUGIN_H

#define PLUGIN_API __attribute__((visibility("default")))

#include "hotreload.h"
#include "raycaster.h"

PLUGIN_API int Draw(void *ctx);
PLUGIN_API int Update(void *ctx, float dt);

#endif // PLUGIN_H