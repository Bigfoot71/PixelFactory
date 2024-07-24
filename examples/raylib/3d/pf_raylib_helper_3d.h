#ifndef PF_RAYLIB_HELPER_3D
#define PF_RAYLIB_HELPER_3D

#include "pixelfactory/pf.h"
#include <raylib.h>
#include <float.h>

typedef struct {
    pf_renderer3d_t rn3D;
    pf_renderer2d_t rn2D;
    Texture2D rayTex;
    Image rayIm;
} PF_Renderer;

typedef struct {
    pf_vertexbuffer3d_t* meshes;
    Material* materials;
    int* meshMaterial;
    int materialCount;
    int meshCount;
} PF_Model;

PF_Renderer
PF_LoadRenderer(int w, int h)
{
    PF_Renderer rn = { 0 };
    
    rn.rn3D = pf_renderer3d_create(w, h, pf_color_blend_alpha, pf_depth_less);

    rn.rn2D.fb = rn.rn3D.fb;
    pf_mat3_identity(rn.rn2D.mat_view);

    rn.rayIm = (Image) {
        .data = rn.rn3D.fb.buffer,
        .width = w,
        .height = h,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    };

    rn.rayTex = LoadTextureFromImage(rn.rayIm);

    return rn;
}

void
PF_UnloadRenderer(PF_Renderer rn)
{
    pf_renderer3d_delete(&rn.rn3D);
    UnloadTexture(rn.rayTex);
}

void
PF_Clear(PF_Renderer rn, Color color)
{
    pf_renderer3d_clear(&rn.rn3D, *(pf_color_t*)&color, FLT_MAX);
}

void
PF_UpdateRenderer(PF_Renderer rn)
{
    UpdateTexture(rn.rayTex, rn.rayIm.data);
}

void
PF_DrawRenderer(PF_Renderer rn)
{
    DrawTexture(rn.rayTex, 0, 0, WHITE);
}

PF_Model
PF_LoadModel(Model model)
{
    // Allocation des vertex buffers
    pf_vertexbuffer3d_t* meshes = calloc(model.meshCount, sizeof(pf_vertexbuffer3d_t));

    for (int i = 0; i < model.meshCount; i++) {
        Mesh* mesh = &model.meshes[i];

        meshes[i].positions = mesh->animVertices ? mesh->animVertices : mesh->vertices;
        meshes[i].normals = mesh->animNormals ? mesh->animNormals : mesh->normals;
        meshes[i].colors = (pf_color_t*)mesh->colors;
        meshes[i].texcoords = mesh->texcoords;

        if (mesh->indices) {
            meshes[i].num_indices = mesh->triangleCount * 3; // Correction de la taille de l'allocation
            meshes[i].indices = malloc(meshes[i].num_indices * sizeof(uint32_t));
            for (size_t j = 0; j < meshes[i].num_indices; j++) {
                meshes[i].indices[j] = mesh->indices[j];
            }
        }

        meshes[i].num_vertices = mesh->vertexCount;
    }

    PF_Model pfModel = { 0 };

    pfModel.meshes = meshes;
    pfModel.meshCount = model.meshCount;

    pfModel.materials = model.materials;
    pfModel.meshMaterial = model.meshMaterial;
    pfModel.materialCount = model.materialCount;

    return pfModel;
}

void
PF_ProcFrag_Model(
    struct pf_renderer3d* rn,
    pf_vertex3d_t* vertex,
    pf_color_t* out_color,
    const void* attr)
{
    (void)rn;
    (void)vertex;

    *out_color = *(pf_color_t*)&((Material*)attr)->maps[MATERIAL_MAP_ALBEDO].color;
}

void
PF_DrawModel(PF_Renderer rn, PF_Model pfModel)
{
    for (int i = 0; i < pfModel.meshCount; i++) {
        pf_renderer3d_vertex_buffer(&rn.rn3D, &pfModel.meshes[i], NULL, NULL, PF_ProcFrag_Model,
            &pfModel.materials[pfModel.meshMaterial[i]]);
    }
}

#endif //PF_RAYLIB_HELPER_3D
