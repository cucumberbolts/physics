#include <raylib.h>
#include <raymath.h>

#include "scenes.h"

static const Vector2 CONST_g = (Vector2){ 0.0f, 3200.0f };

typedef struct {
    Vector2 pos; // position, m
    Vector2 vel; // velocity, m/s
} Obj;

static Obj ball;

static float floor_height = 550.0f;

void bouncing_ball_init() {
    ball = (Obj){ 0 };
    ball.pos = (Vector2){ GetScreenWidth() * 0.5f, 10.0f };
}

void bouncing_ball_update(float dt) {
    ball.vel = Vector2Add(ball.vel, Vector2Scale(CONST_g, dt));
    ball.pos = Vector2Add(ball.pos, Vector2Scale(ball.vel, dt));

    if (ball.pos.y >= floor_height) {
        ball.pos.y = floor_height;
        ball.vel.y = -ball.vel.y;
    }
}

void bouncing_ball_render() {
    // Draw the circle
    DrawCircleV(ball.pos, 20, BLUE);
}

void bouncing_ball_unload() {}

SceneNum bouncing_ball_transition() {
    return BOUNCING_BALL;
}
