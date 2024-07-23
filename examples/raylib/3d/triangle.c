#include "pixelfactory/pf.h"
#include <raylib.h>
#include <float.h>

#define SCREEN_W 800
#define SCREEN_H 600

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
    InitWindow(SCREEN_W, SCREEN_H, "triangle");

    pf_renderer3d_t rn = pf_renderer3d_create(800, 600, NULL, NULL);

    Image im = {
        .data = rn.fb.buffer,
        .width = rn.fb.w,
        .height = rn.fb.h,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    };

    Texture tex = LoadTextureFromImage(im);

    pf_vertexbuffer3d_t triangle = { 0 };
    triangle.positions = triangle_positions;
    triangle.colors = triangle_colors;
    triangle.num_vertices = 3;

    while (!WindowShouldClose())
    {
        pf_mat4_look_at(rn.mat_view, (float[3]) { 3.0f*cosf(GetTime()), 0, 3.0f*sinf(GetTime()) }, (float[3]) { 0, 0, 0 }, (float[3]) { 0, 1, 0 });

        pf_renderer3d_clear(&rn, PF_BLACK, FLT_MAX);
        pf_renderer3d_vertex_buffer(&rn, &triangle, NULL, NULL, NULL, NULL);

        UpdateTexture(tex, im.data);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(tex, 0, 0, WHITE);
            DrawFPS(10,10);
        EndDrawing();
    }

    UnloadTexture(tex);
    pf_renderer3d_delete(&rn);

    CloseWindow();
}
