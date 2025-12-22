cmake_minimum_required(VERSION 3.5)

project(physics)

# Automatically download raylib
include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(RAYLIB_VERSION 5.5)

FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(raylib)

set(SOURCES
    "src/main.c"
    "src/scenes.h"

    "src/title.c"
    "src/simple_pendulum.c"
    "src/double_pendulum.c"
    "src/bouncing_ball.c"
)

add_executable(${PROJECT_NAME} ${SOURCES})

set(SHELL_FILE_PATH "${raylib_SOURCE_DIR}/src/minshell.html")

if (EMSCRIPTEN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1 --shell-file ${SHELL_FILE_PATH}")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()

target_link_libraries(${PROJECT_NAME} raylib)
