cmake_minimum_required(VERSION 3.12)
project(games C)

set(CMAKE_C_STANDARD 99)

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
)

FetchContent_MakeAvailable(raylib)

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
# VECTOR
# =========================

add_executable(vector
    vector/vector.c
    shared/utils.c
)

target_include_directories(vector PRIVATE
    vector
    shared
)

target_link_libraries(vector raylib m)