
#include "stdbool.h"

typedef int (*DrawFuncPtr)(void *);

int reload_plugin(const char *file, long *lastPluginTime, DrawFuncPtr *DrawPlugin, void **plugin_handle);
bool should_reload(const char *file, long *lastPluginTime);