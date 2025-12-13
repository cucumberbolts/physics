cmake_minimum_required(VERSION 3.5)

project(physics)

set(SOURCES
    "src/main.c"
    "src/scenes.h"

    "src/title.c"
    "src/simple_pendulum.c"
    "src/double_pendulum.c"
    "src/bouncing_ball.c"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} ${SOURCES})

add_subdirectory(lib/raylib)

if (EMSCRIPTEN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1 --shell-file ${CMAKE_SOURCE_DIR}/lib/raylib/src/minshell.html")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()

target_link_libraries(${PROJECT_NAME} raylib)
