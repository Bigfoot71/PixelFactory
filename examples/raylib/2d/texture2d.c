#include "pixelfactory/pf.h"
#include "pixelfactory/pf_color.h"
#include "pixelfactory/pf_framebuffer.h"
#include <raylib.h>

#define SCREEN_W 800
#define SCREEN_H 600

void frag_proc(struct pf_renderer2d* rn, pf_vertex2d_t* vertex, pf_color_t* out_color, const void* attr)
{
    (void)out_color;

    int x = vertex->position[0];
    int y = vertex->position[1];

    x = (int)(x + sin(y * 0.1f + GetTime() * 10.0f) * 5.0f);

    if (x > 0 && x < (int)rn->fb.w)
    {
        pf_color_t* dst = rn->fb.buffer + y * rn->fb.w + x;
        *dst = ((pf_texture2d_t*)attr)->sampler(attr,
            vertex->texcoord[0], vertex->texcoord[1]);
    }
}

int main()
{
    InitWindow(SCREEN_W, SCREEN_H, "texture2d");
    HideCursor();

    pf_renderer2d_t rn = pf_renderer2d_create(800, 600, pf_color_blend_alpha);

    Image im = {
        .data = rn.fb.buffer,
        .width = rn.fb.w,
        .height = rn.fb.h,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    };

    Image image = LoadImage("../../examples/resources/bonhomme.png");
    pf_texture2d_t pfTex = pf_texture2d_create(image.data, image.width, image.height, image.format);

    Texture tex = LoadTextureFromImage(im);

    while (!WindowShouldClose())
    {
        pf_renderer2d_clear(&rn, PF_WHITE);
        pf_renderer2d_texture2d_ex_tint(&rn, &pfTex, 800/2, 600/2, 0.5f, 0.5f, GetTime(), 256, 256, PF_GREEN);
        pf_renderer2d_texture2d_ex_map(&rn, &pfTex, GetMouseX(), GetMouseY(), 0.25f, 0.25f, 0, 256, 256, frag_proc);

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
