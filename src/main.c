#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>

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
};

static size_t current_sim = 0;
static int display_fps = true;

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    const float fps = 120;

    InitWindow(screenWidth, screenHeight, "Physics");

    // Initialize the current simulation
    simulations[current_sim].init();

    Camera2D cam = { 
        .target = (Vector2){ 0.0f, 0.0f },
        .offset = (Vector2){ (float)screenWidth*0.5f, (float)screenHeight*0.5f },
        .zoom = 0.5f,
    };

    float last_time = GetTime();
    float last_frame = 0.0f;
    float period = 1.0f/fps;

    while (!WindowShouldClose()) {   // Detect window close button or ESC key
        float time = GetTime();
        float dt = time - last_time;
        last_time = time;
        last_frame += dt;

        // Update the current simulation
        simulations[current_sim].update(dt);

        if (last_frame > period) {
            BeginDrawing();
            ClearBackground(RAYWHITE);

            // Display FPS
            if (IsKeyPressed(KEY_F))
                display_fps = !display_fps;
            if (display_fps) {
                char fps_display[16];
                sprintf(fps_display, "FPS: %f", 1.0f/last_frame);
                DrawText(fps_display, 10, 10, 40, BLACK);
            }

            // Render the current simulation
            BeginMode2D(cam);
            simulations[current_sim].render();
            EndMode2D();

            EndDrawing();

            last_frame = 0.0f;
        }
    }

    CloseWindow();
}
