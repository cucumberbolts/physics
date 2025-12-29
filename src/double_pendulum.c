#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdint.h>

#include "scenes.h"

typedef struct {
    // .x -> theta 1
    // .y -> theta 2
    // .z -> theta-dot 1
    // .w -> theta-dot 2
    Vector4 state;
    float len;
    Vector2 pos;
} DoublePendulum;

static const float CONST_g = 3000.0f;

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

    const float t1 = pen->state.x;
    const float t2 = pen->state.y;
    const float w1 = pen->state.z;
    const float w2 = pen->state.w;
    
    float kinetic = w2 * w2
           + 4.0f * w1 * w1
           + 3.0f * w1 * w2 * cosf(t1 - t2);

    kinetic *= m * pen->len * pen->len / 6.0f;

    float dist = -0.5f * pen->len * (3.0f * cosf(t1) + cosf(t2));
    float potential = CONST_g * (2 * pen->len + dist);

    return (Energy){ kinetic, potential };
}

Vector4 dp_ode(Vector4* const s, float len, float g) {
    // Returns the time derivative of each element in the vector
    // Differential equations derived from the Euler-Lagrange equation
    // https://en.wikipedia.org/wiki/Double_pendulum#Lagrangian

    const float t1 = s->x;
    const float t2 = s->y;
    const float w1 = s->z;
    const float w2 = s->w;

    float sin_diff = sinf(t1 - t2);

    float A = (4.0f / 3.0f) * len;
    float B = 0.5f * len * cosf(t1 - t2);
    float C = -0.5f * len * w2 * w2 * sin_diff -1.5f * g * sinf(t1);
    float D = (1.0f / 3.0f) * len;
    float E = 0.5f * len * w1 * w1 * sin_diff -0.5f * g * sinf(t2);

    float denom = 1.0f / (A * D - B * B);

    // Angular acceleration (theta-double-dot) for both pendulums
    float a1 = (C * D - B * E) * denom;
    float a2 = (A * E - B * C) * denom;

    return (Vector4){ .x = w1, .y = w2, .z = a1, .w = a2 };
}

Vector4 dp_euler(Vector4* s, float len, float g, float dt) {
    Vector4 k = dp_ode(s, len, g);
    Vector4 state = Vector4Add(*s, Vector4Scale(k, dt));
    return state;
}

Vector4 dp_rk2(Vector4* s, float len, float g, float dt) {
    Vector4 k1 = dp_ode(s, len, g);

    Vector4 a = Vector4Add(*s, Vector4Scale(k1, dt));
    Vector4 k2 = dp_ode(&a, len, g);

    Vector4 ds = Vector4Scale(Vector4Add(k1, k2), dt * 0.5f);
    return Vector4Add(*s, ds);
}

Vector4 dp_rk4(Vector4* s, float len, float g, float dt) {
    Vector4 k1 = dp_ode(s, len, g);

    Vector4 a = Vector4Add(*s, Vector4Scale(k1, 0.5f * dt));
    Vector4 k2 = dp_ode(&a, len, g);

    Vector4 b = Vector4Add(*s, Vector4Scale(k2, 0.5f * dt));
    Vector4 k3 = dp_ode(&b, len, g);

    Vector4 c = Vector4Add(*s, Vector4Scale(k3, dt));
    Vector4 k4 = dp_ode(&c, len, g);

    Vector4 ds = Vector4Add(
        Vector4Scale(Vector4Add(k1, k4), dt / 6.0f),
        Vector4Scale(Vector4Add(k2, k3), dt / 3.0f)
    );

    return Vector4Add(*s, ds);
}

void dp_render(DoublePendulum* const pen, Color colour) {
    Vector2 end1 = {
        .x = pen->pos.x + pen->len * sinf(pen->state.x),
        .y = pen->pos.y + pen->len * cosf(pen->state.x),
    };

    Vector2 end2 = {
        .x = end1.x + pen->len * sinf(pen->state.y),
        .y = end1.y + pen->len * cosf(pen->state.y),
    };

    // Draw the bars
    DrawLineEx(pen->pos, end1, 20, colour);
    DrawLineEx(end1,     end2, 20, colour);
}

#define EXPORT_CSV 0

void double_pendulum_init() {
    Vector2 pos = { GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f };

    const float th1 = 2.0f;
    const float th2 = 2.2f;
    const float thd1 = 0.0f;
    const float thd2 = 0.0f;

    pen1 = (DoublePendulum){
        .state = (Vector4){ th1, th2, thd1, thd2 },
        .len = 200.0f,
        .pos = pos,
    };

    pen2 = (DoublePendulum){
        .state = (Vector4){ th1, th2, thd1, thd2 },
        .len = 200.0f,
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
    printf("%f, %f, %f, %f, %f, %f, %f\n", GetTime(), e1_total, e2_total, pen1.state.x, pen1.state.y, pen1.state.z, pen1.state.w);
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
