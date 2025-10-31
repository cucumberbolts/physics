#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

typedef struct {
    Vector2 pos;    // Position of pendulum's axis of rotation
    float length;   // Length of pendulum string
    float theta;    // Angular displacement of pendulum in radians with respect to the vertical
    float theta_d;  // Angular velocity of pendulum
    float theta_dd; // Angular acceleration of pendulum
} Pendulum;

static const float CONST_g = 3000.0f;
static Pendulum pen;

// Period measurement
static float timer = 0.0f;  // Time since last time the pendulum was at the peak
static float period = 0.0f; // The last time measurement for the period
static float start_pos_err = 0.110f;   // Error for this measurement
static bool in_error = false;

static float starting_angle = 1.5f;
static bool dampen = false;

void simple_pendulum_init() {
    pen = (Pendulum){ 
        .pos = (Vector2){ 0.0f, -100.0f },
        .length = 300.0f,
        .theta = starting_angle,
    };
}

void simple_pendulum_update(float dt) {
    float dampening = dampen ? pen.theta_d * 0.2f : 0;
    pen.theta_dd = -CONST_g/pen.length * sinf(pen.theta) - dampening;

    // pen.theta_dd = -CONST_g/pen.length * sinf(pen.theta);
    pen.theta_d += pen.theta_dd * dt;
    pen.theta += pen.theta_d * dt;

    // Start timer to measure the period
    if (pen.theta > starting_angle - start_pos_err && in_error == false) {
        period = timer;
        timer = 0.0f;
        in_error = true;
    } else if (pen.theta < starting_angle - start_pos_err && in_error == true) {
        timer += dt;
        in_error = false;
    } else {
        timer += dt;
    }
}

void simple_pendulum_render() {
    Vector2 bob_pos = { 
        .x = pen.pos.x + pen.length * sinf(pen.theta),
        .y = pen.pos.y + pen.length * cosf(pen.theta),
    };

    DrawLineEx(pen.pos, bob_pos, 10, BLACK);
    DrawCircleV(bob_pos, 20, BLUE);

    char disp[128];
    sprintf(disp, "Theta (radians): %f\nTimer (seconds): %f\nPeriod (seconds): %f\n", pen.theta, timer, period);
    DrawText(disp, 200, -320, 30, BLACK);
}
