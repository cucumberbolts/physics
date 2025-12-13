#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

#include "scenes.h"

typedef struct {
    Vector2 pos; // Position of pendulum's axis of rotation
    float len;   // Length of pendulum string
    float t;     // Angular displacement of pendulum in radians with respect to the vertical
    float td;    // Angular velocity of pendulum
    float tdd;   // Angular acceleration of pendulum
} Pendulum;

static const float CONST_g = 3000.0f;
static Pendulum pen;

// Period measurement
static float timer;  // Time since last time the pendulum was at the peak
static float period; // The last time measurement for the period
static float start_pos_err = 0.110f;   // Error for this measurement
static bool in_error;

static float starting_angle = 3.0f;
static bool dampen = false;

// Button to return to title
static Rectangle back_btn = { 0 };
static bool back_btn_hovered;
static bool back_btn_pressed;

void simple_pendulum_init() {
    pen = (Pendulum){ 
        .pos = (Vector2){ GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f },
        .len = 300.0f,
        .t = starting_angle,
    };

    SetWindowTitle("Simple Pendulum");

    float timer = 0.0f;
    float period = 0.0f;
    bool in_error = false;

    back_btn = (Rectangle) {
        .x = 50,
        .y = 50,
        .width = 150,
        .height = 60,
    };
    back_btn_hovered = false;
    back_btn_pressed = false;
}

void simple_pendulum_update(float dt) {
    float dampening = dampen ? pen.td * 0.5f : 0;
    pen.tdd = -CONST_g/pen.len * sinf(pen.t) - dampening;

    // pen.tdd = -CONST_g/pen.len * sinf(pen.t);
    pen.td += dt * pen.tdd;
    pen.t  += dt * pen.td;

    // Start timer to measure the period
    if (pen.t > starting_angle - start_pos_err && in_error == false) {
        period = timer;
        timer = 0.0f;
        in_error = true;
    } else if (pen.t < starting_angle - start_pos_err && in_error == true) {
        timer += dt;
        in_error = false;
    } else {
        timer += dt;
    }

    if (IsKeyPressed(KEY_D)) dampen = !dampen;

    Vector2 mouse_pos = GetMousePosition();
    back_btn_hovered = CheckCollisionPointRec(mouse_pos, back_btn);
    back_btn_pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && back_btn_hovered;
}

void simple_pendulum_render() {
    Vector2 bob_pos = { 
        .x = pen.pos.x + pen.len * sinf(pen.t),
        .y = pen.pos.y + pen.len * cosf(pen.t),
    };

    DrawLineEx(pen.pos, bob_pos, 10, BLACK);
    DrawCircleV(bob_pos, 20, BLUE);

    char disp[128];
    sprintf(disp, "Theta (radians): %.3f\nTimer (seconds): %.3f\nPeriod (seconds): %.3f\nDampened: %s\n", pen.t, timer, period, dampen ? "true" : "false");
    DrawText(disp, GetScreenWidth() - 400, 30, 30, BLACK);

    // Draw the back button
    if (back_btn_hovered) {
        DrawRectangleRec(back_btn, BLUE);
        DrawRectangleLinesEx(back_btn, 5.0f, DARKBLUE);
        DrawText("Back", back_btn.x + 39, back_btn.y + 15, 30, DARKBLUE);
    } else {
        DrawRectangleRec(back_btn, LIGHTGRAY);
        DrawRectangleLinesEx(back_btn, 5.0f, DARKGRAY);
        DrawText("Back", back_btn.x + 39, back_btn.y + 15, 30, DARKGRAY);
    }
}

void simple_pendulum_unload() {}

SceneNum simple_pendulum_transition() {
    return back_btn_pressed ? TITLE : SIMPLE_PENDULUM;
}
