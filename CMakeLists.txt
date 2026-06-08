cmake_minimum_required(VERSION 3.12)
project(games C)

set(CMAKE_C_STANDARD 99)

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
)

FetchContent_MakeAvailable(raylib)

# 1. Find Dependencies
# Find the system library for dynamic loading (dlopen, dlsym)
find_library(DL_LIBRARY dl REQUIRED)

# =========================================================
# HOT RELOAD GAME FUNCTION
# =========================================================

function(add_hot_game NAME)

    # ---------------------------------
    # Plugin (.so)
    # ---------------------------------

    add_library(${NAME}_plugin SHARED
        ${NAME}/plugin.c
    )

    target_include_directories(${NAME}_plugin PRIVATE
        ${NAME}
        shared
    )

    target_link_libraries(${NAME}_plugin PRIVATE
        raylib
        m
    )

    # Prevent lib prefix:
    # vector_plugin.so instead of libvector_plugin.so

    set_target_properties(${NAME}_plugin PROPERTIES
        PREFIX ""
        OUTPUT_NAME "${NAME}_plugin"
    )

    # ---------------------------------
    # Loader executable
    # ---------------------------------

    add_executable(${NAME}
        ${NAME}/${NAME}.c
        shared/utils.c
        shared/hotreload.c
    )

    target_include_directories(${NAME} PRIVATE
        ${NAME}
        shared
    )

    target_link_libraries(${NAME} PRIVATE
        raylib
        m
        ${DL_LIBRARY}
    )

endfunction()


# =========================
# PONG
# =========================

add_executable(pong
    pong/main.c
    pong/pong.c
    shared/utils.c
)

target_include_directories(pong PRIVATE
    pong
    shared
)

target_link_libraries(pong raylib m)

# =========================
# ARKANOID
# =========================

add_executable(arkanoid
    arkanoid/arkanoid.c
    shared/utils.c
)

target_include_directories(arkanoid PRIVATE
    arkanoid
    shared
)

target_link_libraries(arkanoid raylib m)

# =========================
# POLL
# =========================

add_executable(pool
    pool/pool.c
    shared/utils.c
)

target_include_directories(pool PRIVATE
    pool
    shared
)

target_link_libraries(pool raylib m)

# =========================
# TRIGO
# =========================

add_executable(trigo
    trigo/trigo.c
    shared/utils.c
)

target_include_directories(trigo PRIVATE
    trigo
    shared
)

target_link_libraries(trigo raylib m)


# =========================
# HOTRELOAD
# =========================

# add_hot_game(vector)
add_hot_game(raycaster)
