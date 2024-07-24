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
    pf_vertex2d_t* vertex,
    pf_color_t* out_color,
    const void* attr)
{
    (void)rn;
    const PF_Material* material = attr;
    *out_color = material->texture.sampler(attr, vertex->texcoord[0], vertex->texcoord[1]);
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

    pf_vertexbuffer2d_t mesh = { 0 };
    mesh.positions = positions;
    mesh.texcoords = texcoords;
    mesh.num_vertices = 6;

    Image image = LoadImage("../../examples/resources/bonhomme.png");

    PF_Material material;
    material.texture = pf_texture2d_create(image.data, image.width, image.height, image.format);
    material.tint = PF_BLUE;

    while (!WindowShouldClose())
    {
        pf_renderer2d_clear(&rn, PF_BLACK);
        pf_renderer2d_vertex_buffer(&rn, &mesh, NULL, NULL, frag_proc, &material);

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
