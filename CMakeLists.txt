cmake_minimum_required(VERSION 3.5)

project(physics)

set(SOURCES
    "src/main.c"
    "src/simulations.h"

    "src/simple_pendulum.c"
    "src/double_pendulum.c"
    "src/bouncing_ball.c"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} ${SOURCES})

add_subdirectory(lib/raylib)
target_link_libraries(${PROJECT_NAME} raylib)
