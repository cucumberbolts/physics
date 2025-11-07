#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

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
static float timer = 0.0f;  // Time since last time the pendulum was at the peak
static float period = 0.0f; // The last time measurement for the period
static float start_pos_err = 0.110f;   // Error for this measurement
static bool in_error = false;

static float starting_angle = 3.1f;
static bool dampen = true;

void simple_pendulum_init() {
    pen = (Pendulum){ 
        .pos = (Vector2){ 0.0f, -100.0f },
        .len = 300.0f,
        .t = starting_angle,
    };
}

void simple_pendulum_update(float dt) {
    float dampening = dampen ? pen.td * 0.2f : 0;
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
}

void simple_pendulum_render() {
    Vector2 bob_pos = { 
        .x = pen.pos.x + pen.len * sinf(pen.t),
        .y = pen.pos.y + pen.len * cosf(pen.t),
    };

    DrawLineEx(pen.pos, bob_pos, 10, BLACK);
    DrawCircleV(bob_pos, 20, BLUE);

    char disp[128];
    sprintf(disp, "Theta (radians): %.3f\nTimer (seconds): %.3f\nPeriod (seconds): %.3f\n", pen.t, timer, period);
    DrawText(disp, 200, -320, 30, BLACK);
}
