/**
 *  Copyright (c) 2024 Le Juez Victor
 *
 *  This software is provided "as-is", without any express or implied warranty. In no event 
 *  will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial 
 *  applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you 
 *  wrote the original software. If you use this software in a product, an acknowledgment 
 *  in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented
 *  as being the original software.
 *
 *   3. This notice may not be removed or altered from any source distribution.
 */

#include "pixelfactory/components/pf_processors.h"
#include "pixelfactory/core/pf_renderer2d.h"
#include "pixelfactory/core/pf_renderer3d.h"

/* Default Processor 2D Functions */

void
pf_proc2d_vertex_default(
    pf_vertex2d_t* out_vertex,
    const pf_mat3_t transform,
    const void* uniforms,
    void* varying)
{
    (void)uniforms;
    (void)varying;

    pf_vec2_transform(out_vertex->position, out_vertex->position, transform);
}

void
pf_proc2d_rasterizer_default(
    pf_vertex2d_t* out_vertex,
    pf_vertex2d_t* vertex_1,
    pf_vertex2d_t* vertex_2,
    pf_vertex2d_t* vertex_3,
    pf_vec3_t bary,
    void* varying)
{
    (void)varying;

    pf_vec2_bary_v_r(out_vertex->position, vertex_1->position, vertex_2->position, vertex_3->position, bary);
    pf_vec2_bary_v_r(out_vertex->texcoord, vertex_1->texcoord, vertex_2->texcoord, vertex_3->texcoord, bary);
    out_vertex->color = pf_color_bary_v(vertex_1->color, vertex_2->color, vertex_3->color, bary);
}

void
pf_proc2d_fragment_default(
    struct pf_renderer2d* rn,
    pf_vertex2d_t* vertex,
    pf_color_t* out_color,
    const void* uniforms,
    void* varying)
{
    (void)rn;
    (void)uniforms;
    (void)varying;

    *out_color = vertex->color;
}

void
pf_proc2d_fragment_texture_as_uniform(
    struct pf_renderer2d* rn,
    pf_vertex2d_t* vertex,
    pf_color_t* out_color,
    const void* uniforms,
    void* varying)
{
    (void)rn;
    (void)uniforms;
    (void)varying;

    const pf_texture2d_t* tex = uniforms;
    *out_color = tex->sampler(tex, vertex->texcoord[0], vertex->texcoord[1]);
    *out_color = pf_color_blend_mul(*out_color, vertex->color);
}

/* Default Processor 3D Functions */

void pf_proc3d_vertex_default(
    pf_vertex3d_t* out_vertex,
    pf_vec4_t out_homogeneous,
    const pf_mat4_t mat_model,
    const pf_mat4_t mat_normal,
    const pf_mat4_t mat_mvp,
    const void* uniforms,
    void* varying)
{
    (void)uniforms;
    (void)varying;

    pf_vec4_t position;
    position[3] = 1.0f;
    pf_vec3_copy(position, out_vertex->position);
    pf_vec4_transform(out_homogeneous, position, mat_mvp);

    pf_vec3_transform(out_vertex->position, out_vertex->position, mat_model);
    pf_vec3_transform(out_vertex->normal, out_vertex->normal, mat_normal);
}

void
pf_proc3d_screen_projection_default(
    const pf_renderer3d_t* rn,
    pf_vertex3d_t* vertices,
    pf_vec4_t homogeneous[],
    size_t vertices_count,
    int screen_pos[][2],
    void* varying)
{
    (void)vertices;
    (void)varying;

    for (size_t i = 0; i < vertices_count; ++i)
    {
        pf_vec4_t* h = &homogeneous[i];

        // Division of XY coordinates by weight
        float invW = 1.0f / (*h)[3];
        (*h)[0] *= invW;
        (*h)[1] *= invW;

        // Convert homogeneous coordinates to screen coordinates
        screen_pos[i][0] = (rn->viewport_position[0] + ((*h)[0] + 1.0f) * 0.5f * rn->viewport_dimension[0]) + 0.5f;
        screen_pos[i][1] = (rn->viewport_position[1] + (1.0f - (*h)[1]) * 0.5f * rn->viewport_dimension[1]) + 0.5f;
    }
}

void
pf_proc3d_screen_projection_perspective_correct(
    const pf_renderer3d_t* rn,
    pf_vertex3d_t* vertices,
    pf_vec4_t homogeneous[],
    size_t vertices_count,
    int screen_pos[][2],
    void* varying)
{
    (void)varying;

    for (size_t i = 0; i < vertices_count; ++i)
    {
        pf_vertex3d_t* v = &vertices[i];
        pf_vec4_t* h = &homogeneous[i];

        // Calculation of the reciprocal of Z for the perspective correct
        (*h)[2] = 1.0f / (*h)[2];

        // Division of texture coordinates by the Z axis (perspective correct)
        v->texcoord[0] *= (*h)[2];
        v->texcoord[1] *= (*h)[2];

        // REVIEW: Division of vertex colors by the Z axis (perspective correct)
        //v->color = pf_color_scale(v->color, (*h)[2]);

        // Division of XY coordinates by weight
        float invW = 1.0f / (*h)[3];
        (*h)[0] *= invW;
        (*h)[1] *= invW;

        // Convert homogeneous coordinates to screen coordinates
        screen_pos[i][0] = (rn->viewport_position[0] + ((*h)[0] + 1.0f) * 0.5f * rn->viewport_dimension[0]) + 0.5f;
        screen_pos[i][1] = (rn->viewport_position[1] + (1.0f - (*h)[1]) * 0.5f * rn->viewport_dimension[1]) + 0.5f;
    }
}

void
pf_proc3d_rasterizer_default(
    pf_vertex3d_t* out_vertex,
    pf_vertex3d_t* v1,
    pf_vertex3d_t* v2,
    pf_vertex3d_t* v3,
    pf_vec3_t bary,
    float z_depth,
    void* varying)
{
    (void)varying;
    (void)z_depth;

    pf_vec2_bary_v_r(out_vertex->texcoord, v1->texcoord, v2->texcoord, v3->texcoord, bary);
    pf_vec3_bary_v(out_vertex->position, v1->position, v2->position, v3->position, bary);
    pf_vec3_bary_v(out_vertex->normal, v1->normal, v2->normal, v3->normal, bary);
    out_vertex->color = pf_color_bary_v(v1->color, v2->color, v3->color, bary);
}

void
pf_proc3d_rasterizer_perspective_correct(
    pf_vertex3d_t* out_vertex,
    pf_vertex3d_t* v1,
    pf_vertex3d_t* v2,
    pf_vertex3d_t* v3,
    pf_vec3_t bary,
    float z_depth,
    void* varying)
{
    (void)varying;

    float u = bary[0];
    float v = bary[1];
    float w = bary[2];

    // Interpolation of vertex position coordinates
    out_vertex->position[0] = u * v1->position[0] + v * v2->position[0] + w * v3->position[0];
    out_vertex->position[1] = u * v1->position[1] + v * v2->position[1] + w * v3->position[1];
    out_vertex->position[2] = u * v1->position[2] + v * v2->position[2] + w * v3->position[2];

    // Interpolation of texture coordinates with perspective correction
    out_vertex->texcoord[0] = z_depth * (u * v1->texcoord[0] + v * v2->texcoord[0] + w * v3->texcoord[0]);
    out_vertex->texcoord[1] = z_depth * (u * v1->texcoord[1] + v * v2->texcoord[1] + w * v3->texcoord[1]);

    // Interpolation of normals
    out_vertex->normal[0] = u * v1->normal[0] + v * v2->normal[0] + w * v3->normal[0];
    out_vertex->normal[1] = u * v1->normal[1] + v * v2->normal[1] + w * v3->normal[1];
    out_vertex->normal[2] = u * v1->normal[2] + v * v2->normal[2] + w * v3->normal[2];

    // Interpolation of vertex colors
    out_vertex->color.a[0] = u * v1->color.a[0] + v * v2->color.a[0] + w * v3->color.a[0];
    out_vertex->color.a[1] = u * v1->color.a[1] + v * v2->color.a[1] + w * v3->color.a[1];
    out_vertex->color.a[2] = u * v1->color.a[2] + v * v2->color.a[2] + w * v3->color.a[2];
    out_vertex->color.a[3] = u * v1->color.a[3] + v * v2->color.a[3] + w * v3->color.a[3];
}

void
pf_proc3d_fragment_default(
    struct pf_renderer3d* rn,
    pf_vertex3d_t* vertex,
    pf_color_t* out_color,
    const void* uniforms,
    void* varying)
{
    (void)rn;
    (void)uniforms;
    (void)varying;

    *out_color = vertex->color;
}
