#include "pixelfactory/pf.h"
#include <raylib.h>

#define SCREEN_W 800
#define SCREEN_H 600

void set_pixel_fb1(pf_framebuffer_t* fb, pf_color_t* color, int x, int y) {
    (void)fb;(void)x;(void)y;
    if (((x+y)/8)%2) {
        *color = (pf_color_t) {
            .v = 0xFFFFFFFF
        };
    } else {
        *color = (pf_color_t) {
            .v = 0x00000000
        };
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
    InitWindow(SCREEN_W, SCREEN_H, "basic");

    Image im1 = GenImageColor(800, 600, BLACK);
    Image im2 = GenImageColor(400, 300, BLACK);
    
    pf_framebuffer_t fb1 = {
        .buffer = im1.data,
        .w = im1.width,
        .h = im1.height
    };

    pf_framebuffer_t fb2 = {
        .buffer = im2.data,
        .w = im2.width,
        .h = im2.height
    };

    Texture tex1 = LoadTextureFromImage(im1);
    Texture tex2 = LoadTextureFromImage(im2);

    while (!WindowShouldClose())
    {
        pf_framebuffer_map(&fb1, NULL, set_pixel_fb1);

        pf_framebuffer_copy(&fb2, NULL, &fb1, NULL);
        pf_framebuffer_map(&fb2, NULL, set_pixel_fb2);

        UpdateTexture(tex1, im1.data);
        UpdateTexture(tex2, im2.data);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(tex1, 0, 0, WHITE);
            DrawTexture(tex2, 0, 0, WHITE);
            DrawRectangle(0, 0, 120, 40, BLACK);
            DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadImage(im1);
    UnloadImage(im2);

    CloseWindow();
}
