#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

typedef struct {
    Vector2 pos; // Position of pendulum's axis of rotation
    float t;     // Angular displacement of pendulum in radians with respect to the vertical
    float td;    // Angular velocity of pendulum
    // float tdd;   // Angular acceleration of pendulum
} Pendulum;

typedef struct {
    Pendulum p1, p2; // Pendulum bars
    float len;       // Length of pendulum bars (same for both)
} DoublePendulum;

static const float CONST_g = 3000.0f;

DoublePendulum pen1;
DoublePendulum pen2;

typedef struct {
    float kinetic;
    float potential;
} Energy;

DoublePendulum dp_new(Vector2 pos, float len, float t1, float td1, float t2, float td2) {
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

Energy dp_energy(DoublePendulum* const pen) {
    const float m = 1.0f; // Placeholder for mass (can be ignored)
    
    float cos_diff = cosf(pen->p1.t - pen->p2.t);
    float kinetic = pen->p2.td * pen->p2.td
           + 4.0f * pen->p1.td * pen->p1.td
           + 3.0f * pen->p1.td * pen->p2.td * cos_diff;

    kinetic *= m * pen->len * pen->len / 6.0f;

    float dist = -0.5f * pen->len * (3.0f * cosf(pen->p1.t) + cosf(pen->p2.t));
    float potential = CONST_g * (2 * pen->len + dist);

    return (Energy){ kinetic, potential };
}

Vector2 dp_diff_eq(DoublePendulum* const pen) {
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

    float p1_tdd = (C * D - B * E) * denom;
    float p2_tdd = (A * E - B * C) * denom;

    return (Vector2){ p1_tdd, p2_tdd };
}

void dp_move(DoublePendulum* pen, Vector2 k, float dt) {
    pen->p1.t  += dt * pen->p1.td;
    pen->p2.t  += dt * pen->p2.td;
    pen->p1.td += dt * k.x;
    pen->p2.td += dt * k.y;
}

void dp_euler(DoublePendulum* pen, float dt) {
    Vector2 k = dp_diff_eq(pen);
    dp_move(pen, k, dt);
}

void dp_rk2(DoublePendulum* pen, float dt) {
    Vector2 k1 = dp_diff_eq(pen);

    DoublePendulum pen1 = *pen;
    dp_move(&pen1, k1, dt);
    Vector2 k2 = dp_diff_eq(&pen1);

    pen->p1.t  += 0.5 * dt * (pen->p1.td + pen1.p1.td);
    pen->p2.t  += 0.5 * dt * (pen->p2.td + pen1.p2.td);
    pen->p1.td += 0.5 * dt * (k1.x + k2.x);
    pen->p2.td += 0.5 * dt * (k1.y + k2.y);
}

void dp_rk4(DoublePendulum* pen, float dt) {
    Vector2 k1 = dp_diff_eq(pen);

    DoublePendulum pen1 = *pen;
    dp_move(&pen1, k1, 0.5 * dt);
    Vector2 k2 = dp_diff_eq(&pen1);

    DoublePendulum pen2 = *pen;
    dp_move(&pen2, k2, 0.5 * dt);
    Vector2 k3 = dp_diff_eq(&pen2);

    DoublePendulum pen3 = *pen;
    dp_move(&pen3, k3, dt);
    Vector2 k4 = dp_diff_eq(&pen3);

    pen->p1.t  += dt / 6.0f * (pen->p1.td + 2.0f * pen1.p1.td + 2.0f * pen2.p1.td + pen3.p1.td);
    pen->p2.t  += dt / 6.0f * (pen->p2.td + 2.0f * pen1.p2.td + 2.0f * pen2.p2.td + pen3.p2.td);
    pen->p1.td += dt / 6.0f * (k1.x + 2.0f * k2.x + 2.0f * k3.x + k4.x);
    pen->p2.td += dt / 6.0f * (k1.y + 2.0f * k2.y + 2.0f * k3.y + k4.y);
}

void dp_render(DoublePendulum* const pen, Color colour) {
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
    pen1 = dp_new(pos, 200.f, 2.0f, 0.0f, 2.2f, 0.0f);
    pen2 = dp_new(pos, 200.f, 2.0f, 0.0f, 2.2f, 0.0f);
}

void double_pendulum_update(float dt) {
    dp_rk4(&pen1, dt);
    // dp_rk4(&pen2, dt);
    dp_euler(&pen2, dt);
}

void double_pendulum_render() {
    // Render the pendulums
    dp_render(&pen1, RED);
    dp_render(&pen2, ORANGE);

    // Display the pendulums' energies
    Energy e1 = dp_energy(&pen1);
    Energy e2 = dp_energy(&pen2);

    float e1_total = e1.kinetic + e1.potential;
    float e2_total = e2.kinetic + e2.potential;

    // Scale down since CONST_g is massive
    e1_total *= 0.001f;
    e2_total *= 0.001f;

    char disp[128];
    sprintf(disp, "Pendulum 1 energy: %.2f\nPendulum 2 energy: %.2f\n", e1_total, e2_total);
    DrawText(disp, 200, -320, 30, BLACK);
}
