#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdint.h>

#include "scenes.h"

typedef struct {
    double t1, t2, w1, w2;
} State;

State state_add(State a, State b) {
    return (State){
        a.t1 + b.t1,
        a.t2 + b.t2,
        a.w1 + b.w1,
        a.w2 + b.w2,
    };
}

State state_scale(State a, double b) {
    return (State){
        a.t1 * b,
        a.t2 * b,
        a.w1 * b,
        a.w2 * b,
    };
}

typedef struct {
    State state;
    double len;
    Vector2 pos;
} DoublePendulum;

static const double CONST_g = 3000.0;

static DoublePendulum pen1;
static DoublePendulum pen2;

typedef struct {
    float kinetic;
    float potential;
} Energy;

// Button to return to title
static Rectangle back_btn = { 0 };
static bool back_btn_hovered;
static bool back_btn_pressed;

Energy dp_energy(DoublePendulum* const pen) {
    const float m = 1.0f; // Placeholder for mass (can be ignored)

    const float t1 = pen->state.t1;
    const float t2 = pen->state.t2;
    const float w1 = pen->state.w1;
    const float w2 = pen->state.w2;
    
    float kinetic = w2 * w2
           + 4.0f * w1 * w1
           + 3.0f * w1 * w2 * cosf(t1 - t2);

    kinetic *= m * pen->len * pen->len / 6.0f;

    float dist = -0.5f * pen->len * (3.0f * cosf(t1) + cosf(t2));
    float potential = CONST_g * (2 * pen->len + dist);

    return (Energy){ kinetic, potential };
}

State dp_ode(State* const s, double len, double g) {
    // Returns the time derivative of each element in the vector
    // Differential equations derived from the Euler-Lagrange equation
    // https://en.wikipedia.org/wiki/Double_pendulum#Lagrangian

    double sin_diff = sin(s->t1 - s->t2);

    double A = (4.0 / 3.0) * len;
    double B = 0.5 * len * cos(s->t1 - s->t2);
    double C = -0.5 * len * s->w2 * s->w2 * sin_diff -1.5 * g * sin(s->t1);
    double D = (1.0 / 3.0) * len;
    double E = 0.5 * len * s->w1 * s->w1 * sin_diff -0.5 * g * sin(s->t2);

    double denom = 1.0 / (A * D - B * B);

    // Angular acceleration (theta-double-dot) for both pendulums
    double a1 = (C * D - B * E) * denom;
    double a2 = (A * E - B * C) * denom;

    return (State){ .t1 = s->w1, .t2 = s->w2, .w1 = a1, .w2 = a2 };
}

State dp_euler(State* s, double len, double g, float dt) {
    State k = dp_ode(s, len, g);
    State state = state_add(*s, state_scale(k, dt));
    return state;
}

State dp_rk2(State* s, double len, double g, float dt) {
    State k1 = dp_ode(s, len, g);

    State a = state_add(*s, state_scale(k1, dt));
    State k2 = dp_ode(&a, len, g);

    State ds = state_scale(state_add(k1, k2), dt * 0.5);
    return state_add(*s, ds);
}

State dp_rk4(State* s, double len, double g, float dt) {
    State k1 = dp_ode(s, len, g);

    State a = state_add(*s, state_scale(k1, 0.5 * dt));
    State k2 = dp_ode(&a, len, g);

    State b = state_add(*s, state_scale(k2, 0.5 * dt));
    State k3 = dp_ode(&b, len, g);

    State c = state_add(*s, state_scale(k3, dt));
    State k4 = dp_ode(&c, len, g);

    State ds = state_add(
        state_scale(state_add(k1, k4), dt / 6.0),
        state_scale(state_add(k2, k3), dt / 3.0)
    );

    return state_add(*s, ds);
}

void dp_render(DoublePendulum* const pen, Color colour) {
    Vector2 end1 = {
        .x = pen->pos.x + pen->len * sin(pen->state.t1),
        .y = pen->pos.y + pen->len * cos(pen->state.t1),
    };

    Vector2 end2 = {
        .x = end1.x + pen->len * sin(pen->state.t2),
        .y = end1.y + pen->len * cos(pen->state.t2),
    };

    // Draw the bars
    DrawLineEx(pen->pos, end1, 20, colour);
    DrawLineEx(end1,     end2, 20, colour);
}

#define EXPORT_CSV 0

void double_pendulum_init() {
    Vector2 pos = { GetScreenWidth() * 0.5, GetScreenHeight() * 0.5 };

    const double th1 = 2.0;
    const double th2 = 2.2;
    const double thd1 = 0.0;
    const double thd2 = 0.0;

    pen1 = (DoublePendulum){
        .state = (State){ th1, th2, thd1, thd2 },
        .len = 200.0,
        .pos = pos,
    };

    pen2 = (DoublePendulum){
        .state = (State){ th1, th2, thd1, thd2 },
        .len = 200.0,
        .pos = pos,
    };

    SetWindowTitle("Double Pendulum");

    back_btn = (Rectangle) {
        .x = 50,
        .y = 50,
        .width = 150,
        .height = 60,
    };
    back_btn_hovered = false;
    back_btn_pressed = false;
    
#if EXPORT_CSV
    printf("Time, Red (RK2) Energy, Orange (RK4) Energy, t1, t2, w1, w2\n");
#endif
}

void double_pendulum_update(float dt) {
    pen1.state = dp_rk2(&pen1.state, pen1.len, CONST_g, dt);
    pen2.state = dp_rk4(&pen2.state, pen2.len, CONST_g, dt);

    Vector2 mouse_pos = GetMousePosition();
    back_btn_hovered = CheckCollisionPointRec(mouse_pos, back_btn);
    back_btn_pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && back_btn_hovered;
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

#if EXPORT_CSV
    printf("%lf, %lf, %lf, %lf, %lf, %lf, %lf\n", GetTime(), e1_total, e2_total, pen1.state.t1, pen1.state.t2, pen1.state.w1, pen1.state.w2);
#endif

    char disp[128];
    sprintf(disp, "Red pendulum energy: %.2f\nOrange pendulum energy: %.2f\n", e1_total, e2_total);
    DrawText(disp, GetScreenWidth() - 520, 30, 30, BLACK);

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

void double_pendulum_unload() {}

SceneNum double_pendulum_transition() {
    return back_btn_pressed ? TITLE : DOUBLE_PENDULUM;
}
