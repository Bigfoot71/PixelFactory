#include "pixelfactory/pf.h"

#include <float.h>
#include <raylib.h>
#include <raymath.h>

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600

typedef struct {
    pf_texture2d_t texture;
    pf_vec3_t camPos;
} Uniforms;

static bool WallCollision(Camera3D* camera, const Image* imMap);
static void FragProcModel(struct pf_renderer3d* rn, pf_vertex_t* vertex, pf_color_t* outColor, const void* uniforms);
static void RendererMap(pf_renderer3d_t* rn, pf_color_t* outColor, float* outDepth, int x, int y, float u, float v);

int main(void)
{
    // Init raylib window and set target FPS
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PixelFactory - Raylib - First Person 3D");
    //SetTargetFPS(60);
    DisableCursor();

    // Create a rendering buffer in RAM
    pf_renderer3d_t rn = pf_renderer3d_create(SCREEN_WIDTH, SCREEN_HEIGHT, NULL, pf_depth_less);
    pf_mat4_perspective(rn.mat_proj, 45.0f, (float)rn.fb.w / rn.fb.h, 0.01f, 10.0f);
    rn.cull_mode = PF_CULL_BACK;

    // Create a raylib raylib texture to render buffer
    Texture tex = LoadTextureFromImage((Image) {
        .data = rn.fb.buffer,
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    });

    Camera3D camera = {
        (Vector3) { 1.2f, 0.5f, 1.2f },
        (Vector3) { 1.5f, 0.5f, 1.5f },
        (Vector3) { 0.0f, 1.0f, 0.0f },
        60.0f,
        CAMERA_PERSPECTIVE
    };

    Image imMap = LoadImage(RESOURCES_PATH "images/cubicmap.png");
    Model model = LoadModelFromMesh(GenMeshCubicmap(imMap, (Vector3) { 1.0f, 1.0f, 1.0f }));

    pf_vertex_buffer_t* pfMeshes = calloc(model.meshCount, sizeof(pf_vertex_buffer_t));
    for (int i = 0; i < model.meshCount; i++)
    {
        Mesh* mesh = &model.meshes[i];

        pfMeshes[i] = pf_vertex_buffer_create_3d(mesh->vertexCount,
            mesh->vertices, mesh->texcoords, mesh->normals,
            (pf_color_t*)mesh->colors);

        if (mesh->indices) {
            pfMeshes[i].num_indices = mesh->triangleCount * 3;
            pfMeshes[i].indices = mesh->indices;
        }
    }

    Image imAtlas = LoadImage(RESOURCES_PATH "images/cubicmap_atlas.png");
    pf_texture2d_t texAtlas = pf_texture2d_create(imAtlas.data, imAtlas.width, imAtlas.height, imAtlas.format);

    Uniforms uniforms = { 0 };
    uniforms.texture = texAtlas;

    while (!WindowShouldClose())
    {
        // Update

        Vector3 dir = {
            IsKeyDown(KEY_W) - IsKeyDown(KEY_S),
            IsKeyDown(KEY_D) - IsKeyDown(KEY_A),
            IsKeyDown(KEY_SPACE) - IsKeyDown(KEY_LEFT_SHIFT)
        };

        dir = Vector3Scale(Vector3Normalize(dir), 5.0f * GetFrameTime());

        UpdateCameraPro(&camera, dir, (Vector3) {
            .x = GetMouseDelta().x * 0.1f,
            .y = GetMouseDelta().y * 0.1f,
            .z = 0.0,
        }, 0.0f);

        pf_mat4_copy(rn.mat_view,
            MatrixToFloat(GetCameraMatrix(camera)));

        pf_vec3_copy(uniforms.camPos,
            *(pf_vec3_t*)&camera.position);

        if (camera.position.y < 0.5f)
        {
            camera.target.y += 0.5f - camera.position.y;
            camera.position.y = 0.5;
        }

        if (WallCollision(&camera, &imMap))
        {
            (void)WallCollision(&camera, &imMap);
        }

        // Draw

        pf_renderer3d_clear(&rn, PF_BLACK, FLT_MAX);
        for (int i = 0; i < model.meshCount; i++) {
            pf_proc3d_t proc = { 0 };
            proc.fragment = FragProcModel;
            proc.uniforms = &uniforms;
            pf_renderer3d_vertex_buffer(&rn, &pfMeshes[i], NULL, &proc);
        }
        pf_renderer3d_map(&rn, RendererMap);

        UpdateTexture(tex, rn.fb.buffer);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(tex, 0, 0, WHITE);
            DrawFPS(10, 10);
        EndDrawing();
    }

    // Unload assets
    UnloadImage(imAtlas);
    UnloadModel(model);

    // Unload the renderer and related data
    pf_renderer3d_delete(&rn);
    UnloadTexture(tex);

    // Close raylib window
    CloseWindow();

    return 0;
}

bool WallCollision(Camera3D* camera, const Image* imMap)
{
    Vector2 pos2D = { .x = camera->position.x, .y = camera->position.z };
    Vector2 rdPos2D = { .x = round(pos2D.x), .y = round(pos2D.y) };

    int xMax = PF_MIN(rdPos2D.x + 1, imMap->width - 1);
    int yMax = PF_MIN(rdPos2D.y + 1, imMap->height - 1);

    Rectangle camRect = { .x = pos2D.x - 0.2f, .y = pos2D.y - 0.2f, .width = 0.4f, .height = 0.4f };
    Vector2 result_disp = { .x = 0, .y = 0 };

    for (int y = fmaxf(rdPos2D.y - 1, 0); y <= yMax; y += 1)
    {
        for (int x = fmaxf(rdPos2D.x - 1, 0); x <= xMax; x += 1)
        {
            if ((x != rdPos2D.x || y != rdPos2D.y) && ((uint8_t*)imMap->data)[y * imMap->width + x] > 0)
            {
                Rectangle tileRec = { .x = x - 0.5f, .y = y - 0.5f, .width = 1.0f, .height = 1.0f };

                Vector2 dist = { .x = pos2D.x - x, .y = pos2D.y - y };
                Vector2 minDist = { .x = (camRect.width + tileRec.width) * 0.5f, .y = (camRect.height + tileRec.height) * 0.5f };

                Vector2 collisionVec = { .x = 0, .y = 0 };

                if (fabsf(dist.x) < minDist.x && fabsf(dist.y) < minDist.y)
                {
                    Vector2 overlap = {
                        .x = minDist.x - fabsf(dist.x),
                        .y = minDist.y - fabsf(dist.y),
                    };

                    if (overlap.x < overlap.y)
                    {
                        collisionVec.x = (dist.x > 0) ? overlap.x : -overlap.x;
                    }
                    else
                    {
                        collisionVec.y = (dist.y > 0) ? overlap.y : -overlap.y;
                    }
                }

                if (fabsf(collisionVec.x) > fabsf(result_disp.x)) result_disp.x = collisionVec.x;
                if (fabsf(collisionVec.y) > fabsf(result_disp.y)) result_disp.y = collisionVec.y;
            }
        }
    }

    float adx = fabsf(result_disp.x);
    float ady = fabsf(result_disp.y);

    if (adx > ady)
    {
        camera->position.x += result_disp.x;
        camera->target.x += result_disp.x;
    }
    else
    {
        camera->position.z += result_disp.y;
        camera->target.z += result_disp.y;
    }

    return (adx > 0 && ady > 0);
}

void FragProcModel(pf_renderer3d_t* rn, pf_vertex_t* vertex, pf_color_t* outColor, const void* uniforms)
{
    (void)rn;

    const Uniforms* u = uniforms;

    pf_vec2_t texcoord;
    pf_vertex_get_vec(vertex, PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX, texcoord);

    *outColor = u->texture.sampler(&u->texture, texcoord[0], texcoord[1]);
}

void RendererMap(pf_renderer3d_t* rn, pf_color_t* outColor, float* outDepth, int x, int y, float u, float v)
{
    (void)rn, (void)x, (void)y, (void)u, (void)v;
    *outColor = pf_color_lerpf(*outColor, PF_GRAY, PF_MIN(*outDepth, 10.0f) * 0.1f);
}
