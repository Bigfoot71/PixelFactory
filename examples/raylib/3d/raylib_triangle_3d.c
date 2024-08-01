#include "pixelfactory/pf.h"
#include <raylib.h>
#include <float.h>

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600

static float triangle_positions[] = {
     0,  1, 0,
    -1, -1, 0,
     1, -1, 0
};

static pf_color_t triangle_colors[] = {
    { .c = { 255, 0, 0, 255 } },
    { .c = { 0, 255, 0, 255 } },
    { .c = { 0, 0, 255, 255 } }
};

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PixelFactory - Raylib - Triangle 3D");

    pf_renderer_t rn = pf_renderer_load(SCREEN_WIDTH, SCREEN_HEIGHT, PF_RENDERER_3D);

    Texture tex = LoadTextureFromImage((Image) {
        .data = rn.fb.buffer,
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    });

    pf_vertexbuffer_t triangle = pf_vertexbuffer_create_3d(
        3, triangle_positions, NULL, NULL, triangle_colors);

    while (!WindowShouldClose())
    {
        pf_mat4_look_at(rn.conf3d->mat_view,
            (float[3]) { 3.0f*cosf(GetTime()), 0, 3.0f*sinf(GetTime()) },
            (float[3]) { 0, 0, 0 }, (float[3]) { 0, 1, 0 });

        pf_renderer_clear3d(&rn, PF_BLACK, FLT_MAX);
        pf_renderer_vertexbuffer3d(&rn, &triangle, NULL, NULL);

        UpdateTexture(tex, rn.fb.buffer);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(tex, 0, 0, WHITE);
            DrawFPS(10,10);
        EndDrawing();
    }

    pf_renderer_delete(&rn);
    UnloadTexture(tex);

    CloseWindow();
}
