#include "hotreload.h"

int reload_plugin(const char *file, long *lastPluginTime, DrawFuncPtr *DrawPlugin, void **plugin_handle)
{
    TraceLog(LOG_INFO, "Reloading plugin");
    if (*plugin_handle)
    {
        dlclose(*plugin_handle);
        *plugin_handle = NULL;
        *DrawPlugin = NULL;
    }

    *plugin_handle = dlopen(file, RTLD_NOW);
    if (!(*plugin_handle))
    {
        TraceLog(LOG_ERROR, "Failed to load plugin: %s\n", dlerror());
        return 1;
    }

    // 3. Clear any existing error state and look up the "Draw" symbol string
    char *error = dlerror();
    *DrawPlugin = (DrawFuncPtr)dlsym(*plugin_handle, "Draw");
    error = dlerror();
    if (error != NULL)
    {
        TraceLog(LOG_ERROR, "Failed to find 'Draw' function: %s\n", error);
        dlclose(*plugin_handle);
        return 1;
    }
    error = dlerror();
    if (error != NULL)
    {
        TraceLog(LOG_ERROR, "Failed to find 'Update' function: %s\n", error);
        dlclose(*plugin_handle);
        return 1;
    }
    *lastPluginTime = GetFileModTime(file);
    return 0;
}

bool should_reload(const char *file, long *lastPluginTime)
{
    const long newTime = GetFileModTime(file);
    if (newTime <= *lastPluginTime)
        return false;

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 500000000; // 500 milliseconds (0.5 seconds)
    nanosleep(&ts, NULL);
    return true;
}