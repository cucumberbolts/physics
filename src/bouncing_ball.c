#include "simulations.h"

#include <raylib.h>
#include <raymath.h>

const Vector2 CONST_g = (Vector2){ 0.0f, 32000.0f };

typedef struct {
    Vector2 pos; // position, m
    Vector2 vel; // velocity, m/s
} Obj;

static Obj ball;

void bouncing_ball_init() {
    ball = (Obj){ 0 };
    ball.pos = (Vector2){ 0.0f, -450.0f };
}

void bouncing_ball_update(float dt) {
    ball.vel = Vector2Add(ball.vel, Vector2Scale(CONST_g, dt));
    ball.pos = Vector2Add(ball.pos, Vector2Scale(ball.vel, dt));

    if (ball.pos.y >= 450.0f) {
        ball.pos.y = 450.0f;
        ball.vel.y = -ball.vel.y;
    }
}

void bouncing_ball_render() {
    // Draw the circle
    DrawCircleV(ball.pos, 20, BLUE);
}
