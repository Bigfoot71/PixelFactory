#include "pixelfactory/pf.h"
#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PixelFactory - Raylib - Lines 2D");

    pf_renderer_t rn = pf_renderer_load(SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    Texture tex = LoadTextureFromImage((Image) {
        .data = rn.fb.buffer,
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    });

    while (!WindowShouldClose())
    {
        pf_renderer_clear2d(&rn, PF_BLACK);

        for (int x = 0; x <= 800; x += 20)
        {
            if ((x/20)%2) pf_renderer_line2d_thick_gradient(&rn, x, -100, x, 700, 4, PF_YELLOW, PF_BLUE);
            else pf_renderer_line2d_thick_gradient(&rn, x, 700, x, -100, 4, PF_YELLOW, PF_BLUE);
        }

        for (int y = 0; y <= 600; y += 20)
        {
            if ((y/20)%2) pf_renderer_line2d_thick_gradient(&rn, -100, y, 900, y, 4, PF_CYAN, PF_RED);
            else pf_renderer_line2d_thick_gradient(&rn, 900, y, -100, y, 4, PF_CYAN, PF_RED);
        }

        pf_renderer_line2d_thick_gradient(&rn, -800, -600, 800, 600, 32, PF_RED, PF_BLUE);
        pf_renderer_line2d_thick_gradient(&rn, 800, 0, 0, 600, 32, PF_RED, PF_BLUE);

        UpdateTexture(tex, rn.fb.buffer);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(tex, 0, 0, WHITE);
        EndDrawing();
    }

    pf_renderer_delete(&rn);
    UnloadTexture(tex);

    CloseWindow();
}
