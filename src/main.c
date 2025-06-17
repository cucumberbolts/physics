#include "raylib.h"
#include "raymath.h"

const Vector2 CONST_g = (Vector2){ 0.0f, 2.0f };

typedef struct {
    Vector2 pos; // position
    Vector2 vel; // velocity
} Obj;

Obj ball;

void init() {
    ball = (Obj){ 0 };
    ball.pos = (Vector2){ 0.0f, 0.0f };
}

void update() {
    ball.vel = Vector2Add(ball.vel, CONST_g);
    ball.pos = Vector2Add(ball.pos, ball.vel);

    if (ball.pos.y >= 450) {
        ball.vel.y = -ball.vel.y;
    }
}

void render() {
    // Draw the circle
    DrawCircleV(ball.pos, 20, BLUE);
}

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Physics");

    SetTargetFPS(60);

    init();

    Camera2D cam = { 
        .target = (Vector2){ 0.0f, 0.0f },
        .offset = (Vector2){ (float)screenWidth*0.5f, (float)screenHeight*0.5f },
        .zoom = 0.5f,
    };

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        update();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(cam);
        render();
        EndMode2D();
        EndDrawing();
    }

    CloseWindow();
}
