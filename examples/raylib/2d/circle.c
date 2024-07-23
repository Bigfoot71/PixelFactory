#include "pixelfactory/pf.h"
#include <raylib.h>

#define SCREEN_W 800
#define SCREEN_H 600

int main()
{
    InitWindow(SCREEN_W, SCREEN_H, "lines");

    Image im = GenImageColor(800, 600, BLACK);
    
    pf_framebuffer_t fb = {
        .buffer = im.data,
        .w = im.width,
        .h = im.height
    };

    pf_renderer2d_t rn = {
        .fb = fb,
        .blend = NULL
    };

    Texture tex = LoadTextureFromImage(im);

    while (!WindowShouldClose())
    {
        pf_renderer2d_clear(&rn, PF_BLACK);
        pf_renderer2d_circle_gradient(&rn, 400, 300, 64, PF_BLUE, PF_RED);
        pf_renderer2d_circle_lines_thick(&rn, 400, 300, 128, 8, PF_RED);

        UpdateTexture(tex, im.data);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(tex, 0, 0, WHITE);
            DrawFPS(10,10);
        EndDrawing();
    }

    UnloadImage(im);

    CloseWindow();
}
