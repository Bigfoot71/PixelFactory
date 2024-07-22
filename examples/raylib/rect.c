#include "pixelfactory/pf.h"
#include <raylib.h>
#include <string.h>

#define SCREEN_W 800
#define SCREEN_H 600
int main()
{
    InitWindow(SCREEN_W, SCREEN_H, "rect");

    pf_renderer2d_t rn = pf_renderer2d_create(800, 600, NULL);

    Image im = {
        .data = rn.fb.buffer,
        .width = rn.fb.w,
        .height = rn.fb.h,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    };

    pf_camera2d_t cam = { 0 };
    cam.offset[0] = -400;
    cam.offset[1] = -300;
    cam.target[0] = 400;
    cam.target[1] = 300;
    cam.zoom = 1.0f;

    Texture tex = LoadTextureFromImage(im);

    while (!WindowShouldClose())
    {
        pf_renderer2d_clear(&rn, PF_BLACK);

        //pf_renderer2d_rect_gradient(&rn, -500, -500, 1400, 1100,
        //    PF_RED, PF_BLUE, PF_GREEN, PF_YELLOW);

        cam.rotation = GetTime();
        pf_mat3_t mat;
        pf_camera2d_get_view_matrix(&cam, mat);
        memcpy(rn.mat_view, mat, sizeof(pf_mat3_t));

        pf_renderer2d_rect(&rn, 100, 100, 700, 500, PF_RED);

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
