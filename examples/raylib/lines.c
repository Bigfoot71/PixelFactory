#include "pixelfactory/pf.h"
#include <raylib.h>

#define SCREEN_W 800
#define SCREEN_H 600

int main()
{
    InitWindow(SCREEN_W, SCREEN_H, "lines");

    pf_renderer2d_t rn = pf_renderer2d_create(800, 600, NULL);

    Image im = {
        .data = rn.fb.buffer,
        .width = rn.fb.w,
        .height = rn.fb.h,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    };

    Texture tex = LoadTextureFromImage(im);

    while (!WindowShouldClose())
    {
        pf_renderer2d_clear(&rn, PF_BLACK);

        for (int x = 0; x <= 800; x += 20)
        {
            if ((x/20)%2) pf_renderer2d_line_thick_gradient(&rn, x, -100, x, 700, 4, PF_YELLOW, PF_BLUE);
            else pf_renderer2d_line_thick_gradient(&rn, x, 700, x, -100, 4, PF_YELLOW, PF_BLUE);
        }

        for (int y = 0; y <= 600; y += 20)
        {
            if ((y/20)%2) pf_renderer2d_line_thick_gradient(&rn, -100, y, 900, y, 4, PF_CYAN, PF_RED);
            else pf_renderer2d_line_thick_gradient(&rn, 900, y, -100, y, 4, PF_CYAN, PF_RED);
        }

        pf_renderer2d_line_thick_gradient(&rn, -800, -600, 800, 600, 32, PF_RED, PF_BLUE);
        pf_renderer2d_line_thick_gradient(&rn, 800, 0, 0, 600, 32, PF_RED, PF_BLUE);

        UpdateTexture(tex, im.data);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(tex, 0, 0, WHITE);
        EndDrawing();
    }

    UnloadTexture(tex);
    pf_renderer2d_delete(&rn);

    CloseWindow();
}
