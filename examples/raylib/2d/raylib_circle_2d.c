#include "pixelfactory/pf.h"
#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PixelFactory - Raylib - Circle 2D");

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
        pf_renderer2d_circle_gradient(&rn, 400, 300, 64, PF_BLUE, PF_RED);
        pf_renderer2d_circle_lines_thick(&rn, 400, 300, 128, 8, PF_RED);

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
