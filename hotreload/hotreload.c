#include "plugin.h"
#include <stdlib.h>

int main()
{

  InitWindow(640, 480, "hotreload");

  void *plugin_handle = NULL;
  DrawFuncPtr DrawPlugin = NULL;
  const char *file = "../build/hotreload_plugin.so";
  long lastPluginTime = 0;

  if (reload_plugin(file, &lastPluginTime, &DrawPlugin, &plugin_handle))
    return 1;

  while (!WindowShouldClose())
  {
    if (should_reload(file, &lastPluginTime))
    {
      if (reload_plugin(file, &lastPluginTime, &DrawPlugin, &plugin_handle))
      {
        break;
      }
    }
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawPlugin(NULL);
    EndDrawing();
  }

  if (plugin_handle)
    dlclose(plugin_handle);

  CloseWindow();
}
