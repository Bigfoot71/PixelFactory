#include "pixelfactory/pf.h"
#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PixelFactory - Raylib - Triangle 2D");

    pf_renderer2d_t rn = pf_renderer2d_create(SCREEN_WIDTH, SCREEN_HEIGHT, NULL);

    Texture tex = LoadTextureFromImage((Image) {
        .data = rn.fb.buffer,
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    });

    while (!WindowShouldClose())
    {
        pf_renderer2d_clear(&rn, PF_BLACK);

        pf_renderer2d_triangle_gradient(&rn, 400, 0, 800, 600, 0, 600, PF_RED, PF_GREEN, PF_BLUE);
        //pf_renderer2d_triangle(&rn, 400, 0, 800, 600, 0, 600, PF_RED);

        UpdateTexture(tex, rn.fb.buffer);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(tex, 0, 0, WHITE);
            DrawFPS(10,10);
        EndDrawing();
    }

    pf_renderer2d_delete(&rn);
    UnloadTexture(tex);

    CloseWindow();
}
