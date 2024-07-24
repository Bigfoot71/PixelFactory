#include "pixelfactory/pf.h"

#include <float.h>
#include <raylib.h>
#include <raymath.h>

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600

void
model_frag_proc(
    struct pf_renderer3d* rn,
    pf_vertex3d_t* vertex,
    pf_color_t* out_color,
    const void* attr)
{
    (void)rn;
    (void)vertex;

    *out_color = *(pf_color_t*)&((Material*)attr)->maps[MATERIAL_MAP_ALBEDO].color;
}

int main(void)
{
    // Init raylib window and set target FPS
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PixelForge - Animated Model");
    SetTargetFPS(60);

    // Create a rendering buffer in RAM
    pf_renderer3d_t rn = pf_renderer3d_create(SCREEN_WIDTH, SCREEN_HEIGHT, NULL, pf_depth_less);

    // Create a raylib raylib texture to render buffer
    Texture tex = LoadTextureFromImage((Image) {
        .data = rn.fb.buffer,
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    });

    // Load a 3D model with raylib
    Model model = LoadModel(RESOURCES_PATH "models/robot.glb");

    // Load the animations of the 3D model with raylib
    int animsCount = 0;
    unsigned int animIndex = 0;
    unsigned int animCurrentFrame = 0;
    ModelAnimation *modelAnimations = LoadModelAnimations(RESOURCES_PATH "models/robot.glb", &animsCount);

    // Create structures with mesh vertex buffers
    pf_vertexbuffer3d_t* pfMeshes = calloc(model.meshCount, sizeof(pf_vertexbuffer3d_t));

    for (int i = 0; i < model.meshCount; i++)
    {
        Mesh* mesh = &model.meshes[i];

        pfMeshes[i].positions = mesh->animVertices ? mesh->animVertices : mesh->vertices;
        pfMeshes[i].normals = mesh->animNormals ? mesh->animNormals : mesh->normals;
        pfMeshes[i].colors = (pf_color_t*)mesh->colors;
        pfMeshes[i].texcoords = mesh->texcoords;

        if (mesh->indices) {
            pfMeshes[i].num_indices = mesh->triangleCount * 3;
            pfMeshes[i].indices = mesh->indices;
        }

        pfMeshes[i].num_vertices = mesh->vertexCount;
    }

    // Start the main loop
    while (!WindowShouldClose())
    {
        // Select current animation
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) animIndex = (animIndex + 1)%animsCount;
        else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) animIndex = (animIndex + animsCount - 1)%animsCount;

        // Update model animation
        ModelAnimation anim = modelAnimations[animIndex];
        animCurrentFrame = (animCurrentFrame + 1)%anim.frameCount;
        UpdateModelAnimation(model, anim, animCurrentFrame);

        // Update camera position/diraction
        pf_mat4_look_at(rn.mat_view,
            (float[3]) { 10.0f*cosf(GetTime()), 5, 10.0f*sinf(GetTime()) },
            (float[3]) { 0, 2.5f, 0 }, (float[3]) { 0, 1, 0 });

        // Clear the destination buffer (RAM)
        pf_renderer3d_clear(&rn, PF_BLACK, FLT_MAX);

        // Rendering vertex buffers
        for (int i = 0; i < model.meshCount; i++) {
            pf_renderer3d_vertex_buffer_lines(&rn, &pfMeshes[i], NULL, NULL, model_frag_proc,
                &model.materials[model.meshMaterial[i]]);
        }

        // Updating the texture with the new buffer content
        UpdateTexture(tex, rn.fb.buffer);

        // Texture rendering via raylib
        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(tex, 0, 0, WHITE);
            DrawFPS(10, 10);
        EndDrawing();
    }

    // Unload assets
    UnloadModel(model);
    UnloadModelAnimations(modelAnimations, animsCount);

    // Unload the renderer and associated data
    pf_renderer3d_delete(&rn);
    UnloadTexture(tex);

    // Close raylib window
    CloseWindow();

    return 0;
}