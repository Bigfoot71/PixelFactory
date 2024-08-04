#include "pixelfactory/pf.h"
#include <raylib.h>
#include <float.h>

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600

typedef struct {
    Material material;
    pf_vec3_t cam_pos;
} uniforms_t;

void
model_frag_proc(
    pf_renderer_t* rn,
    pf_vertex_t* vertex,
    pf_color_t* out_color,
    const void* uniforms)
{
    (void)rn;

    const uniforms_t* u = uniforms;

    pf_vec3_t position;
    pf_vertex_get_vec(vertex, PF_ATTRIB_POSITION, position);

    pf_vec2_t texcoord;
    pf_vertex_get_vec(vertex, PF_ATTRIB_TEXCOORD, texcoord);

    pf_vec3_t normal;
    pf_vertex_get_vec(vertex, PF_ATTRIB_NORMAL, normal);

    // constants
    const pf_vec3_t light_pos = { 10.0f, 10.0f, 10.0f };
    const pf_vec3_t light_color = { 1.0f, 1.0f, 1.0f };
    const pf_vec3_t ambient = { 0.2f, 0.2f, 0.2f };

    pf_vec3_t N;
    pf_vec3_normalize_r(N, normal);

    pf_vec3_t V;
    pf_vec3_direction_r(V, u->cam_pos, position);

    pf_vec3_t L;
    pf_vec3_direction_r(L, light_pos, position);

    pf_vec3_t nL;
    pf_vec3_neg_r(nL, L);

    // model color
    pf_color_t final_color = *(pf_color_t*)&u->material.maps[MATERIAL_MAP_ALBEDO].color;

    // diffuse
    float NdotL = PF_MAX(pf_vec3_dot(N, L), 0.0f);

    pf_vec3_t diffuse;
    pf_vec3_scale(diffuse, light_color, NdotL);
    
    // specular
    pf_vec3_t reflect_dir;
    pf_vec3_reflect_r(reflect_dir, nL, N);
    float spec = powf(PF_MAX(pf_vec3_dot(V, reflect_dir), 0.0), 32);

    pf_vec3_t specular;
    pf_vec3_scale(specular, light_color, spec);

    // combine values
    pf_vec3_t final_color_v = { 0 };
    pf_vec3_add(final_color_v, final_color_v, ambient);
    pf_vec3_add(final_color_v, final_color_v, diffuse);
    pf_vec3_add(final_color_v, final_color_v, specular);

    final_color.c.r = PF_MIN(final_color.c.r * final_color_v[0], 255);
    final_color.c.g = PF_MIN(final_color.c.g * final_color_v[1], 255);
    final_color.c.b = PF_MIN(final_color.c.b * final_color_v[2], 255);

    *out_color = final_color;
}

int main(void)
{
    // Init raylib window and set target FPS
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PixelFactory - Raylib - Lighting 3D");
    SetTargetFPS(60);

    // Create a rendering buffer in RAM
    pf_renderer_t rn = pf_renderer_load(SCREEN_WIDTH, SCREEN_HEIGHT, PF_RENDERER_3D);
    rn.conf3d->depth_test = pf_depth_less;

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
    pf_vertexbuffer_t* pfMeshes = calloc(model.meshCount, sizeof(pf_vertexbuffer_t));
    for (int i = 0; i < model.meshCount; i++)
    {
        Mesh* mesh = &model.meshes[i];

        pfMeshes[i] = pf_vertexbuffer_create_3d(mesh->vertexCount,
            mesh->animVertices ? mesh->animVertices : mesh->vertices,
            mesh->texcoords,
            mesh->animNormals ? mesh->animNormals : mesh->normals,
            (pf_color_t*)mesh->colors);

        if (mesh->indices) {
            pfMeshes[i].num_indices = mesh->triangleCount * 3;
            pfMeshes[i].indices = mesh->indices;
        }
    }

    // Declare custom uniforms struct
    uniforms_t uniforms = { 0 };

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
        uniforms.cam_pos[0] = 8.0f*cosf(GetTime());
        uniforms.cam_pos[1] = 5.0f;
        uniforms.cam_pos[2] = 8.0f*sinf(GetTime());
        pf_mat4_look_at(rn.conf3d->mat_view, uniforms.cam_pos,
            (float[3]) { 0, 2.5f, 0 }, (float[3]) { 0, 1, 0 });

        // Clear the destination buffer (RAM)
        pf_renderer_clear3d(&rn, PF_BLACK, FLT_MAX);

        // Rendering vertex buffers
        for (int i = 0; i < model.meshCount; i++) {
            pf_proc3d_t proc = { 0 };
            proc.fragment = model_frag_proc;
            proc.vertex = pf_proc3d_vertex_normal_transform;
            uniforms.material = model.materials[model.meshMaterial[i]];
            proc.uniforms = &uniforms;
            pf_renderer_vertexbuffer3d(&rn, &pfMeshes[i], NULL, &proc);
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
    pf_renderer_delete(&rn);
    UnloadTexture(tex);

    // Close raylib window
    CloseWindow();

    return 0;
}
