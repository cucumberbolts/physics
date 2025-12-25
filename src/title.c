#include <raylib.h>
#include <stdint.h>

#include "scenes.h"

static SceneNum hovered_scene;
static SceneNum selected_scene;
static bool button_pressed;

// No button for title page or bouncing ball
#define SCENE_OFFSET SIMPLE_PENDULUM
#define DISP_SCENE_CNT (SCENE_CNT - SCENE_OFFSET)
static Rectangle buttons[DISP_SCENE_CNT] = { 0 };

static float menu_width = 400.f;
static float menu_height = 400.f;
static float button_padding = 10.0f;

void resize_menu(float width, float height) {
    menu_width = width;
    menu_height = height;

    float button_width = menu_width;
    float button_height = (menu_height - (DISP_SCENE_CNT - 1) * button_padding) / (float)DISP_SCENE_CNT;

    float menu_x = GetScreenWidth() * 0.5 - menu_width * 0.5;
    float menu_y = GetScreenHeight() * 0.5 - menu_height * 0.5;

    for (int i = 0; i < DISP_SCENE_CNT; ++i) {
        buttons[i] = (Rectangle){
            .x = menu_x,
            .y = menu_y + i * (button_height + button_padding),
            .width = menu_width,
            .height = button_height,
        };
    }
}

void title_init() {
    SetWindowTitle("Pendulum Simulations");
    selected_scene = TITLE;

    resize_menu(menu_width, menu_height);
}

void title_update(float dt) {
    if (IsWindowResized()) {
        resize_menu(menu_width, menu_height);
    }

    Vector2 mouse_pos = GetMousePosition();

    for (int i = 0; i < DISP_SCENE_CNT; ++i) {
        if (CheckCollisionPointRec(mouse_pos, buttons[i])) {
            hovered_scene = SCENE_OFFSET + i;
            break;
        } else {
            hovered_scene = TITLE;
        }
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hovered_scene != TITLE) {
        selected_scene = hovered_scene;
    }
}

void title_render() {
    for (int i = 0; i < DISP_SCENE_CNT; ++i) {
        SceneNum scene = SCENE_OFFSET + i;

        const char* name = SCENE_NAMES[scene];
        int32_t font_size = 30;
        int32_t text_width = MeasureText(name, font_size);
        int32_t text_height = 20;
        int32_t x = buttons[i].x + buttons[i].width * 0.5f - text_width * 0.5f;
        int32_t y = buttons[i].y + buttons[i].height * 0.5f - text_height * 0.5f;

        if (scene == hovered_scene) {
            DrawRectangleRec(buttons[i], BLUE);
            DrawRectangleLinesEx(buttons[i], 5.0f, DARKBLUE);
            DrawText(name, x, y, font_size, DARKBLUE);
        } else {
            DrawRectangleRec(buttons[i], LIGHTGRAY);
            DrawRectangleLinesEx(buttons[i], 5.0f, DARKGRAY);
            DrawText(name, x, y, font_size, DARKGRAY);
        }
    }
}

void title_unload() {}

SceneNum title_transition() {
    return selected_scene;
}
