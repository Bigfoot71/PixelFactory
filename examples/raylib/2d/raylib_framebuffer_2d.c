#include "pixelfactory/pf.h"
#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void set_pixel_fb1(pf_framebuffer_t* fb, pf_color_t* color, int x, int y) {
    (void)fb;(void)x;(void)y;
    if (((x+y)/8)%2) {
        *color = PF_WHITE;
    } else {
        *color = PF_BLACK;
    }
}

void set_pixel_fb2(pf_framebuffer_t* fb, pf_color_t* color, int x, int y) {
    (void)fb;(void)x;(void)y;
    color->c.r = (color->c.r * 255) / 255;
    color->c.b = (color->c.b * 255) / 255;
    color->c.g = 0;
    color->c.a = 255;
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PixelFactory - Raylib - Framebuffer 2D");

    pf_framebuffer_t fb1 = pf_framebuffer_create(SCREEN_WIDTH, SCREEN_HEIGHT, PF_BLACK);
    pf_framebuffer_t fb2 = pf_framebuffer_create(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, PF_BLACK);

    Texture tex1 = LoadTextureFromImage((Image) {
        .data = fb1.buffer,
        .width = fb1.w,
        .height = fb1.h,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    });

    Texture tex2 = LoadTextureFromImage((Image) {
        .data = fb2.buffer,
        .width = fb2.w,
        .height = fb2.h,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    });

    while (!WindowShouldClose())
    {
        pf_framebuffer_map(&fb1, NULL, set_pixel_fb1);

        pf_framebuffer_copy(&fb2, NULL, &fb1, NULL);
        pf_framebuffer_map(&fb2, NULL, set_pixel_fb2);

        UpdateTexture(tex1, fb1.buffer);
        UpdateTexture(tex2, fb2.buffer);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(tex1, 0, 0, WHITE);
            DrawTexture(tex2, 0, 0, WHITE);
            DrawRectangle(0, 0, 120, 40, BLACK);
            DrawFPS(10, 10);
        EndDrawing();
    }

    pf_framebuffer_delete(&fb1);
    pf_framebuffer_delete(&fb2);

    UnloadTexture(tex1);
    UnloadTexture(tex2);

    CloseWindow();
}
