#include "pixelfactory/pf.h"
#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

static float positions[] = {

    0, 0,
    800, 0,
    800, 600,

    800, 600,
    400, 300,
    0, 600

};

static float texcoords[] = {

    0, 0,
    1, 0,
    1, 1,

    1, 1,
    0.5, 0.5,
    0, 1

};

typedef struct {
    pf_texture2d_t texture;
    pf_color_t tint;
} PF_Material;

void frag_proc(
    struct pf_renderer2d* rn,
    pf_vertex_t* vertex,
    pf_color_t* out_color,
    const void* uniforms)
{
    (void)rn;

    const PF_Material* material = uniforms;

    pf_vec2_t texcoord;
    pf_vertex_get_vec(vertex, PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX, texcoord);
    *out_color = material->texture.sampler(&material->texture, texcoord[0], texcoord[1]);
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PixelFactory - Raylib - Mesh 2D");

    pf_renderer2d_t rn = pf_renderer2d_create(SCREEN_WIDTH, SCREEN_HEIGHT, NULL);

    Texture tex = LoadTextureFromImage((Image) {
        .data = rn.fb.buffer,
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    });

    pf_vertexbuffer_t mesh = pf_vertexbuffer_create_2d(6, positions, texcoords, NULL);
    Image image = LoadImage(RESOURCES_PATH "images/bonhomme.png");

    PF_Material material;
    material.texture = pf_texture2d_create(image.data, image.width, image.height, image.format);
    material.tint = PF_BLUE;

    pf_proc2d_t proc = { 0 };
    proc.fragment = frag_proc;
    proc.uniforms = &material;

    while (!WindowShouldClose())
    {
        pf_renderer2d_clear(&rn, PF_BLACK);
        pf_renderer2d_vertexbuffer(&rn, &mesh, NULL, &proc);

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
