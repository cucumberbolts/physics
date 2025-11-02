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
static Pendulum p1, p2;
static float length = 200.0f;

void double_pendulum_init() {
    // Don't need to initialize since it gets updated
    // first in the update loop

    p1 = (Pendulum){ 
        .pos = (Vector2){ 0.0f, -100.0f },
        .length = length,
        .theta = 0.0f,
        .theta_d = 0.0f,
    };

    p2 = (Pendulum){ 
        .pos = (Vector2){ 
            .x = p1.pos.x + p1.length * sinf(p1.theta),
            .y = p1.pos.y + p1.length * cosf(p1.theta),
        },
        .length = length,
        .theta = 3.1f,
        .theta_d = 0.0f,
    };
}

void double_pendulum_update(float dt) {
    // Differential equations derived from the Euler-Lagrange equation
    // https://en.wikipedia.org/wiki/Double_pendulum#Lagrangian

    float sin_diff = sinf(p1.theta - p2.theta);

    float A = (4.0f / 3.0f) * length;
    float B = 0.5f * length * cosf(p1.theta - p2.theta);
    float C = -0.5f * length * p2.theta_d * p2.theta_d * sin_diff
        -1.5f * CONST_g * sinf(p1.theta);
    float D = (1.0f / 3.0f) * length;
    float E = 0.5f * length * p1.theta_d * p1.theta_d * sin_diff
        -0.5f * CONST_g * sinf(p2.theta);

    float denom = 1.0f / (A * D - B * B);

    p1.theta_dd = (C * D - B * E) * denom;
    p2.theta_dd = (A * E - B * C) * denom;

    p1.theta_d += dt * p1.theta_dd;
    p1.theta   += dt * p1.theta_d;

    p2.theta_d += dt * p2.theta_dd;
    p2.theta   += dt * p2.theta_d;
}

void double_pendulum_render() {
    Vector2 bob1_pos = {
        .x = p1.pos.x + p1.length * sinf(p1.theta),
        .y = p1.pos.y + p1.length * cosf(p1.theta),
    };

    Vector2 bob2_pos = {
        .x = bob1_pos.x + p2.length * sinf(p2.theta),
        .y = bob1_pos.y + p2.length * cosf(p2.theta),
    };

    // Draw the strings
    DrawLineEx(p1.pos, bob1_pos, 20, BLACK);
    DrawLineEx(bob1_pos, bob2_pos, 20, BLACK);
}
