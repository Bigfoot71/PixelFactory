#include "pixelfactory/pf.h"
#include <raylib.h>

#define SCREEN_W 800
#define SCREEN_H 600

int main()
{
    InitWindow(SCREEN_W, SCREEN_H, "triangle");

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

        pf_renderer2d_triangle_gradient(&rn, 400, 0, 800, 600, 0, 600, PF_RED, PF_GREEN, PF_BLUE);
        //pf_renderer2d_triangle(&rn, 400, 0, 800, 600, 0, 600, PF_RED);

        UpdateTexture(tex, im.data);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(tex, 0, 0, WHITE);
            DrawFPS(10,10);
        EndDrawing();
    }

    UnloadTexture(tex);
    pf_renderer2d_delete(&rn);

    CloseWindow();
}
