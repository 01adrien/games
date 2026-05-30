#if !defined(PLUGIN_H)
#define PLUGIN_H

#define PLUGIN_API __attribute__((visibility("default")))

#include "hotreload.h"

PLUGIN_API int Draw(void *ctx);

#endif // PLUGIN_H