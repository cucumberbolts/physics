#include <stdlib.h>
#include <stdio.h>

#include <raylib.h>

#ifdef PLATFORM_WEB
    #include <emscripten/emscripten.h>
#endif

#include "scenes.h"

INIT_SCENE_ARR(scenes);

typedef struct {
    float fps;

    size_t current_scn;

    float last_time;
    float last_frame;

    bool display_fps;
    bool debug_mode;
} AppData;

// Update and draw one frame
void UpdateDrawFrame(void* app_data);

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Pendulum Simulations");
    SetWindowMinSize(screenWidth, screenHeight);

    AppData d = {
        .fps = 60.0f,

        .current_scn = TITLE,

        .last_time = GetTime(),
        .last_frame = 0.0f,

        .display_fps = false,
        .debug_mode = false,
    };

    // Initialize the current simulation
    scenes[d.current_scn].init();

#ifdef PLATFORM_WEB
    emscripten_set_main_loop_arg(UpdateDrawFrame, (void*)&d, 0, 1);
#else
    SetTargetFPS(d.fps);
    while (!WindowShouldClose()) {
        UpdateDrawFrame((void*)&d);
    }
#endif

    scenes[d.current_scn].unload();

    CloseWindow();
}

void UpdateDrawFrame(void* app_data) {
    AppData* d = (AppData*)app_data;

    // Update delta time
    float time = GetTime();
    float dt = time - d->last_time;
    d->last_time = time;
    d->last_frame += dt;

    // Update the current simulation
    if (!d->debug_mode)
        scenes[d->current_scn].update(dt);
    else if (d->debug_mode && IsKeyPressed(KEY_RIGHT))
        scenes[d->current_scn].update(1.0f/d->fps);

    if (IsKeyPressed(KEY_SPACE))
        d->debug_mode = !d->debug_mode;

    if (d->current_scn != scenes[d->current_scn].transition()) {
        scenes[d->current_scn].unload();
        d->current_scn = scenes[d->current_scn].transition();
        scenes[d->current_scn].init();
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

#if 0
    // Display FPS
    if (IsKeyPressed(KEY_F))
        display_fps = !display_fps;
    if (display_fps) {
        char fps_display[16];
        sprintf(fps_display, "FPS: %d", (int)roundf(1.0f/last_frame));
        DrawText(fps_display, 10, 10, 40, BLACK);
    }
#endif

    // Render the current simulation
    scenes[d->current_scn].render();

    EndDrawing();

    d->last_frame = 0.0f;
}
