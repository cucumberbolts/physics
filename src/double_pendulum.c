#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

typedef struct {
    Vector2 pos; // Position of pendulum's axis of rotation
    float t;     // Angular displacement of pendulum in radians with respect to the vertical
    float td;    // Angular velocity of pendulum
    float tdd;   // Angular acceleration of pendulum
} Pendulum;

typedef struct {
    Pendulum p1, p2; // Pendulum bars
    float len;       // Length of pendulum bars (same for both)
} DoublePendulum;

static const float CONST_g = 3000.0f;

DoublePendulum pen1;
DoublePendulum pen2;

DoublePendulum new_double_pendulum(Vector2 pos, float len, float t1, float td1, float t2, float td2) {
    // Don't need to initialize theta double dot
    // since it gets updated first in the update loop

    Pendulum p1 = {
        .pos = pos,
        .t   = t1,
        .td  = t2,
    };

    Pendulum p2 = {
        .pos = (Vector2){
            .x = p1.pos.x + len * sinf(p1.t),
            .y = p1.pos.y + len * cosf(p1.t),
        },
        .t   = t2,
        .td  = td2,
    };

    return (DoublePendulum){ p1, p2, len };
}

void update_double_pendulum(DoublePendulum* pen, float dt) {
    // Differential equations derived from the Euler-Lagrange equation
    // https://en.wikipedia.org/wiki/Double_pendulum#Lagrangian

    float sin_diff = sinf(pen->p1.t - pen->p2.t);

    float A = (4.0f / 3.0f) * pen->len;
    float B = 0.5f * pen->len * cosf(pen->p1.t - pen->p2.t);
    float C = -0.5f * pen->len * pen->p2.td * pen->p2.td * sin_diff
        -1.5f * CONST_g * sinf(pen->p1.t);
    float D = (1.0f / 3.0f) * pen->len;
    float E = 0.5f * pen->len * pen->p1.td * pen->p1.td * sin_diff
        -0.5f * CONST_g * sinf(pen->p2.t);

    float denom = 1.0f / (A * D - B * B);

    pen->p1.tdd = (C * D - B * E) * denom;
    pen->p2.tdd = (A * E - B * C) * denom;

    pen->p1.td += dt * pen->p1.tdd;
    pen->p1.t  += dt * pen->p1.td;

    pen->p2.td += dt * pen->p2.tdd;
    pen->p2.t  += dt * pen->p2.td;
}

void render_double_pendulum(DoublePendulum* const pen, Color colour) {
    Vector2 bob1_pos = {
        .x = pen->p1.pos.x + pen->len * sinf(pen->p1.t),
        .y = pen->p1.pos.y + pen->len * cosf(pen->p1.t),
    };

    Vector2 bob2_pos = {
        .x = bob1_pos.x + pen->len * sinf(pen->p2.t),
        .y = bob1_pos.y + pen->len * cosf(pen->p2.t),
    };

    // Draw the bars
    DrawLineEx(pen->p1.pos, bob1_pos, 20, colour);
    DrawLineEx(bob1_pos,    bob2_pos, 20, colour);
}

void double_pendulum_init() {
    Vector2 pos = { 0.0f, -100.f };
    pen1 = new_double_pendulum(pos, 200.f, 2.0f, 0.0f, 2.19f, 0.0f);
    pen2 = new_double_pendulum(pos, 200.f, 2.0f, 0.0f, 2.2f, 0.0f);
}

void double_pendulum_update(float dt) {
    update_double_pendulum(&pen1, dt);
    update_double_pendulum(&pen2, dt);
}

void double_pendulum_render() {
    render_double_pendulum(&pen1, RED);
    render_double_pendulum(&pen2, ORANGE);
}
