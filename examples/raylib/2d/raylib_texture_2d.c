#include "pixelfactory/pf.h"
#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void frag_proc(struct pf_renderer2d* rn, pf_vertex2d_t* vertex, pf_color_t* out_color, const void* attr)
{
    (void)rn;
    (void)out_color;

    float u = 0.5f + (vertex->texcoord[0] - 0.5f) * 1.5f;
    float v = 0.5f + (vertex->texcoord[1] - 0.5f) * 1.5f;
    u += sinf(v * 6.0f + GetTime() * 4.0f) * 0.1f;

    float dist = pf_vec2_distance(
        (float[2]) { u, v }, (float[2]) { 0.5f, 0.5f });

    if (dist < 0.5f) {
        *out_color = ((pf_texture2d_t*)attr)->sampler(attr, u, v);
    }
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PixelFactory - Raylib - Texture 2D");
    HideCursor();

    pf_renderer2d_t rn = pf_renderer2d_create(SCREEN_WIDTH, SCREEN_HEIGHT, pf_color_blend_alpha);

    Texture tex = LoadTextureFromImage((Image) {
        .data = rn.fb.buffer,
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    });

    Image image = LoadImage("../../examples/resources/bonhomme.png");
    pf_texture2d_t pfTex = pf_texture2d_create(image.data, image.width, image.height, image.format);

    while (!WindowShouldClose())
    {
        pf_renderer2d_clear(&rn, PF_WHITE);
        pf_renderer2d_texture2d_ex_tint(&rn, &pfTex, 800/2, 600/2, 0.65f, 0.65f, GetTime(), 256, 256, PF_GREEN);
        pf_renderer2d_texture2d_ex_map(&rn, &pfTex, GetMouseX(), GetMouseY(), 0.45f, 0.45f, 0, 256, 256, frag_proc);

        UpdateTexture(tex, rn.fb.buffer);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(tex, 0, 0, WHITE);
            DrawFPS(10,10);
        EndDrawing();
    }

    pf_renderer2d_delete(&rn);
    UnloadTexture(tex);
    UnloadImage(image);

    CloseWindow();
}
