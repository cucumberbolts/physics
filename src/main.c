#include <stdlib.h>
#include <stdio.h>

#include <raylib.h>

#ifdef PLATFORM_WEB
    #include <emscripten/emscripten.h>
#endif

#include "simulations.h"

typedef struct {
    void (*init)(void);
    void (*update)(float);
    void (*render)(void);
    const char* name;
} Simulation;

#define SIMULATION(FUNC, NAME) (Simulation){\
    .init   = FUNC##_init,\
    .update = FUNC##_update,\
    .render = FUNC##_render,\
    .name = #NAME\
}

Simulation simulations[] = {
    SIMULATION(bouncing_ball, "Bouncing Ball"),
    SIMULATION(simple_pendulum, "Simple Pendulum"),
    SIMULATION(double_pendulum, "Double Pendulum"),
};

typedef struct {
    float fps;

    size_t current_sim;
    int display_fps;

    float last_time;
    float last_frame;

    Camera2D cam;

    bool debug_mode;
} AppData;

// Update and draw one frame
void UpdateDrawFrame(void* app_data);

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Physics");

    AppData d = {
        .fps = 60.0f,

        .current_sim = 2,
        .display_fps = false,

        .last_time = GetTime(),
        .last_frame = 0.0f,

        .cam = {
            .target = (Vector2){ 0.0f, 0.0f },
            .offset = (Vector2){ (float)screenWidth*0.5f, (float)screenHeight*0.5f },
            .zoom = 1.0f,
        },

        .debug_mode = false,
    };

    // Initialize the current simulation
    simulations[d.current_sim].init();

#ifdef PLATFORM_WEB
    emscripten_set_main_loop_arg(UpdateDrawFrame, (void*)&d, 0, 1);
#else
    SetTargetFPS(d.fps);
    while (!WindowShouldClose()) {
        UpdateDrawFrame((void*)&d);
    }
#endif

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
        simulations[d->current_sim].update(dt);
    else if (d->debug_mode && IsKeyPressed(KEY_RIGHT))
        simulations[d->current_sim].update(1.0f/d->fps);

    if (IsKeyPressed(KEY_SPACE))
        d->debug_mode = !d->debug_mode;

    // if (last_frame > period) {
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
    BeginMode2D(d->cam);
    simulations[d->current_sim].render();
    EndMode2D();

    EndDrawing();

    d->last_frame = 0.0f;
    // }
}
