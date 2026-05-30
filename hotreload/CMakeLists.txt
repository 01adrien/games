cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 99)
project(hotreload C)

set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
include(FetchContent)
FetchContent_Declare(
  raylib
  GIT_REPOSITORY "https://github.com/raysan5/raylib"
)
FetchContent_MakeAvailable(raylib)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# 1. Find Dependencies
# Find the system library for dynamic loading (dlopen, dlsym)
find_library(DL_LIBRARY dl REQUIRED)


# Define PROJECT_INCLUDE to be the path to the include directory of the project
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/src/") 

# 2. Shared Library (The Hot-Reloaded Game Logic)
file(GLOB_RECURSE PLUGIN_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/plugin/*.c")
add_library(plugin SHARED src/plugin/plugin.c)
target_include_directories(plugin PRIVATE ${PROJECT_INCLUDE})
target_link_libraries(plugin PRIVATE raylib)

# 3. Main Executable (The Loader)
add_executable(${PROJECT_NAME} src/main.c)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
# Link against dl for dynamic loading, and raylib for window/input management
target_link_libraries(${PROJECT_NAME} PRIVATE raylib ${DL_LIBRARY})

# 4. Optional: Prevent CMake from locking the .so file during builds
#set_target_properties(plugin PROPERTIES VERSION 1.0 SOVERSION 1)

