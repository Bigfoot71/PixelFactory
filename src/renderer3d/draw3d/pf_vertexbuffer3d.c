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

#include "pixelfactory/pf_renderer3d.h"

/* Internal Functions Declarations */

void
pf_renderer3d_triangle_INTERNAL(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, const pf_vertex3d_t* v3,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal, const pf_mat4_t mat_mvp,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_rasterizer_fn rast_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_point_INTERNAL(
    pf_renderer3d_t* rn, const pf_vertex3d_t* point, float radius,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal, const pf_mat4_t mat_mvp,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_line_INTERNAL(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, float thick,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal, const pf_mat4_t mat_mvp,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr);


/* Public API Functions */

void
pf_renderer3d_vertex_buffer(pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, const pf_mat4_t transform,
                            pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
                            void* attr)
{
    pf_renderer3d_vertex_buffer_ex(rn, vb, transform, vert_proc, NULL, NULL, NULL, frag_proc, attr);
}

void
pf_renderer3d_vertex_buffer_ex(pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, const pf_mat4_t transform,
                               pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
                               pf_proc3d_screen_projection_fn proj_proc,
                               pf_proc3d_rasterizer_fn rast_proc,
                               pf_proc3d_fragment_fn frag_proc,
                               void* attr)
{
    float* positions = vb->positions;
    if (positions == NULL) return;

    float* texcoords = vb->texcoords;
    pf_color_t* colors = vb->colors;
    uint16_t* indices = vb->indices;
    float* normals = vb->normals;

    /* Preparation of matrices */

    pf_mat4_t mat_model;
    pf_mat4_t mat_normal;
    if (transform == NULL) {
        pf_mat4_identity(mat_model);
        pf_mat4_identity(mat_normal);
    } else {
        pf_mat4_copy(mat_model, transform);
        pf_mat4_inverse(mat_normal, mat_model);
        pf_mat4_transpose(mat_normal, mat_normal);
    }

    pf_mat4_t mat_mvp;
    pf_mat4_mul_r(mat_mvp, mat_model, rn->mat_view);
    pf_mat4_mul(mat_mvp, mat_mvp, rn->mat_proj);

    /* Set default processors if not set */

    if (vert_proc == NULL) vert_proc = pf_proc3d_vertex_default;
    if (clip_proc == NULL) clip_proc = pf_proc3d_clip_triangle;
    if (proj_proc == NULL) proj_proc = pf_proc3d_screen_projection_perspective_correct;
    if (rast_proc == NULL) rast_proc = pf_proc3d_rasterizer_perspective_correct;
    if (frag_proc == NULL) frag_proc = pf_proc3d_fragment_default;

    /* Iterates through all vertices in the vertex buffer */

    uint8_t has_indices = (indices != NULL);
    uint32_t num = (has_indices) ? vb->num_indices : vb->num_vertices;

    for (uint32_t i = 0; i < num; i += 3) {
        uint32_t index_1, index_2, index_3;

        if (has_indices) {
            index_1 = indices[i + 0];
            index_2 = indices[i + 1];
            index_3 = indices[i + 2];
        } else {
            index_1 = i + 0;
            index_2 = i + 1;
            index_3 = i + 2;
        }

        pf_vertex3d_t v1 = { 0 };
        pf_vertex3d_t v2 = { 0 };
        pf_vertex3d_t v3 = { 0 };

        pf_vec3_copy_f(v1.position, positions + 3 * index_1);
        pf_vec3_copy_f(v2.position, positions + 3 * index_2);
        pf_vec3_copy_f(v3.position, positions + 3 * index_3);

        if (texcoords != NULL) {
            pf_vec2_copy_f(v1.texcoord, texcoords + 2 * index_1);
            pf_vec2_copy_f(v2.texcoord, texcoords + 2 * index_2);
            pf_vec2_copy_f(v3.texcoord, texcoords + 2 * index_3);
        }

        if (normals != NULL) {
            pf_vec3_copy_f(v1.normal, normals + 3 * index_1);
            pf_vec3_copy_f(v2.normal, normals + 3 * index_2);
            pf_vec3_copy_f(v3.normal, normals + 3 * index_3);
        }

        if (colors != NULL) {
            v1.color = colors[index_1];
            v2.color = colors[index_2];
            v3.color = colors[index_3];
        } else {
            v1.color = PF_WHITE;
            v2.color = PF_WHITE;
            v3.color = PF_WHITE;
        }

        v1.index = index_1;
        v2.index = index_2;
        v3.index = index_3;

        pf_renderer3d_triangle_INTERNAL(
            rn, &v1, &v2, &v3, mat_model, mat_normal, mat_mvp,
            vert_proc, clip_proc, proj_proc,
            rast_proc, frag_proc,
            attr);
    }
}

void
pf_renderer3d_vertex_buffer_points(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
    pf_renderer3d_vertex_buffer_points_ex(rn, vb, transform, vert_proc, NULL, NULL, frag_proc, attr);
}

void
pf_renderer3d_vertex_buffer_points_ex(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
    float* positions = vb->positions;
    if (positions == NULL) return;

    float* texcoords = vb->texcoords;
    pf_color_t* colors = vb->colors;
    uint16_t* indices = vb->indices;
    float* normals = vb->normals;

    /* Preparation of matrices */

    pf_mat4_t mat_model;
    pf_mat4_t mat_normal;
    if (transform == NULL) {
        pf_mat4_identity(mat_model);
        pf_mat4_identity(mat_normal);
    } else {
        pf_mat4_copy(mat_model, transform);
        pf_mat4_inverse(mat_normal, mat_model);
        pf_mat4_transpose(mat_normal, mat_normal);
    }

    pf_mat4_t mat_mvp;
    pf_mat4_mul_r(mat_mvp, mat_model, rn->mat_view);
    pf_mat4_mul(mat_mvp, mat_mvp, rn->mat_proj);

    /* Set default processors if not set */

    if (vert_proc == NULL) vert_proc = pf_proc3d_vertex_default;
    if (clip_proc == NULL) clip_proc = pf_proc3d_clip_point;
    if (proj_proc == NULL) proj_proc = pf_proc3d_screen_projection_default;
    if (frag_proc == NULL) frag_proc = pf_proc3d_fragment_default;

    /* Iterates through all vertices in the vertex buffer */

    uint8_t has_indices = (indices != NULL);
    uint32_t num = (has_indices) ? vb->num_indices : vb->num_vertices;

    for (uint32_t i = 0; i < num; i++) {
        uint32_t index = (has_indices) ? indices[i] : i;

        pf_vertex3d_t vertex = { 0 };
        pf_vec3_copy_f(vertex.position, positions + 3 * index);

        if (texcoords != NULL) pf_vec2_copy_f(vertex.texcoord, texcoords + index);
        if (normals != NULL) pf_vec3_copy_f(vertex.normal, normals + index);
        if (colors != NULL) vertex.color = colors[i];
        else vertex.color = PF_WHITE;
        vertex.index = index;

        pf_renderer3d_point_INTERNAL(
            rn, &vertex, 0, mat_model, mat_normal, mat_mvp,
            vert_proc, clip_proc, proj_proc, frag_proc,
            attr);
    }
}

void
pf_renderer3d_vertex_buffer_points_thick(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, float radius, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
    pf_renderer3d_vertex_buffer_points_thick_ex(rn, vb, radius, transform, vert_proc, NULL, NULL, frag_proc, attr);
}

void
pf_renderer3d_vertex_buffer_points_thick_ex(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, float radius, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
    float* positions = vb->positions;
    if (positions == NULL) return;

    float* texcoords = vb->texcoords;
    pf_color_t* colors = vb->colors;
    uint16_t* indices = vb->indices;
    float* normals = vb->normals;

    /* Preparation of matrices */

    pf_mat4_t mat_model;
    pf_mat4_t mat_normal;
    if (transform == NULL) {
        pf_mat4_identity(mat_model);
        pf_mat4_identity(mat_normal);
    } else {
        pf_mat4_copy(mat_model, transform);
        pf_mat4_inverse(mat_normal, mat_model);
        pf_mat4_transpose(mat_normal, mat_normal);
    }

    pf_mat4_t mat_mvp;
    pf_mat4_mul_r(mat_mvp, mat_model, rn->mat_view);
    pf_mat4_mul(mat_mvp, mat_mvp, rn->mat_proj);

    /* Set default processors if not set */

    if (vert_proc == NULL) vert_proc = pf_proc3d_vertex_default;
    if (clip_proc == NULL) clip_proc = pf_proc3d_clip_point;
    if (proj_proc == NULL) proj_proc = pf_proc3d_screen_projection_default;
    if (frag_proc == NULL) frag_proc = pf_proc3d_fragment_default;

    /* Iterates through all vertices in the vertex buffer */

    uint8_t has_indices = (indices != NULL);
    uint32_t num = (has_indices) ? vb->num_indices : vb->num_vertices;

    for (uint32_t i = 0; i < num; i++) {
        uint32_t index = (has_indices) ? indices[i] : i;

        pf_vertex3d_t vertex = { 0 };
        pf_vec3_copy_f(vertex.position, positions + 3 * index);

        if (texcoords != NULL) pf_vec2_copy_f(vertex.texcoord, texcoords + index);
        if (normals != NULL) pf_vec3_copy_f(vertex.normal, normals + index);
        if (colors != NULL) vertex.color = colors[i];
        else vertex.color = PF_WHITE;
        vertex.index = index;

        pf_renderer3d_point_INTERNAL(
            rn, &vertex, radius, mat_model, mat_normal, mat_mvp,
            vert_proc, clip_proc, proj_proc, frag_proc,
            attr);
    }
}

void
pf_renderer3d_vertex_buffer_lines(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
    pf_renderer3d_vertex_buffer_lines_ex(rn, vb, transform, vert_proc, NULL, NULL, frag_proc, attr);
}

void
pf_renderer3d_vertex_buffer_lines_ex(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
    float* positions = vb->positions;
    if (positions == NULL) return;

    float* texcoords = vb->texcoords;
    pf_color_t* colors = vb->colors;
    uint16_t* indices = vb->indices;
    float* normals = vb->normals;

    /* Preparation of matrices */

    pf_mat4_t mat_model;
    pf_mat4_t mat_normal;
    if (transform == NULL) {
        pf_mat4_identity(mat_model);
        pf_mat4_identity(mat_normal);
    } else {
        pf_mat4_copy(mat_model, transform);
        pf_mat4_inverse(mat_normal, mat_model);
        pf_mat4_transpose(mat_normal, mat_normal);
    }

    pf_mat4_t mat_mvp;
    pf_mat4_mul_r(mat_mvp, mat_model, rn->mat_view);
    pf_mat4_mul(mat_mvp, mat_mvp, rn->mat_proj);

    /* Set default processors if not set */

    if (vert_proc == NULL) vert_proc = pf_proc3d_vertex_default;
    if (clip_proc == NULL) clip_proc = pf_proc3d_clip_line;
    if (proj_proc == NULL) proj_proc = pf_proc3d_screen_projection_default;
    if (frag_proc == NULL) frag_proc = pf_proc3d_fragment_default;

    /* Iterates through all vertices in the vertex buffer */

    uint8_t has_indices = (indices != NULL);
    uint32_t num = (has_indices) ? vb->num_indices : vb->num_vertices;

    for (uint32_t i = 0; i < num; i += 3) {
        uint32_t tri_indices[3];

        if (has_indices) {
            tri_indices[0] = indices[i + 0];
            tri_indices[1] = indices[i + 1];
            tri_indices[2] = indices[i + 2];
        } else {
            tri_indices[0] = i + 0;
            tri_indices[1] = i + 1;
            tri_indices[2] = i + 2;
        }

        for (uint32_t j = 0; j < 3; ++j) {
            uint32_t index_1 = tri_indices[j];
            uint32_t index_2 = tri_indices[(j + 1) % 3];

            pf_vertex3d_t v1 = { 0 };
            pf_vertex3d_t v2 = { 0 };

            pf_vec3_copy_f(v1.position, positions + 3 * index_1);
            pf_vec3_copy_f(v2.position, positions + 3 * index_2);

            if (texcoords != NULL) {
                pf_vec2_copy_f(v1.texcoord, texcoords + 2 * index_1);
                pf_vec2_copy_f(v2.texcoord, texcoords + 2 * index_2);
            }

            if (normals != NULL) {
                pf_vec3_copy_f(v1.normal, normals + 3 * index_1);
                pf_vec3_copy_f(v2.normal, normals + 3 * index_2);
            }

            if (colors != NULL) {
                v1.color = colors[index_1];
                v2.color = colors[index_2];
            } else {
                v1.color = PF_WHITE;
                v2.color = PF_WHITE;
            }

            v1.index = index_1;
            v2.index = index_2;

            pf_renderer3d_line_INTERNAL(
                rn, &v1, &v2, 0, mat_model, mat_normal, mat_mvp,
                vert_proc, clip_proc, proj_proc, frag_proc,
                attr);
        }
    }
}

void
pf_renderer3d_vertex_buffer_lines_thick(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, float thick, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
    pf_renderer3d_vertex_buffer_lines_thick_ex(rn, vb, thick, transform, vert_proc, NULL, NULL, frag_proc, attr);
}

void
pf_renderer3d_vertex_buffer_lines_thick_ex(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, float thick, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
    float* positions = vb->positions;
    if (positions == NULL) return;

    float* texcoords = vb->texcoords;
    pf_color_t* colors = vb->colors;
    uint16_t* indices = vb->indices;
    float* normals = vb->normals;

    /* Preparation of matrices */

    pf_mat4_t mat_model;
    pf_mat4_t mat_normal;
    if (transform == NULL) {
        pf_mat4_identity(mat_model);
        pf_mat4_identity(mat_normal);
    } else {
        pf_mat4_copy(mat_model, transform);
        pf_mat4_inverse(mat_normal, mat_model);
        pf_mat4_transpose(mat_normal, mat_normal);
    }

    pf_mat4_t mat_mvp;
    pf_mat4_mul_r(mat_mvp, mat_model, rn->mat_view);
    pf_mat4_mul(mat_mvp, mat_mvp, rn->mat_proj);

    /* Set default processors if not set */

    if (vert_proc == NULL) vert_proc = pf_proc3d_vertex_default;
    if (clip_proc == NULL) clip_proc = pf_proc3d_clip_line;
    if (proj_proc == NULL) proj_proc = pf_proc3d_screen_projection_default;
    if (frag_proc == NULL) frag_proc = pf_proc3d_fragment_default;

    /* Iterates through all vertices in the vertex buffer */

    uint8_t has_indices = (indices != NULL);
    uint32_t num = (has_indices) ? vb->num_indices : vb->num_vertices;

    for (uint32_t i = 0; i < num; i += 3) {
        uint32_t tri_indices[3];

        if (has_indices) {
            tri_indices[0] = indices[i + 0];
            tri_indices[1] = indices[i + 1];
            tri_indices[2] = indices[i + 2];
        } else {
            tri_indices[0] = i + 0;
            tri_indices[1] = i + 1;
            tri_indices[2] = i + 2;
        }

        for (uint32_t j = 0; j < 3; ++j) {
            uint32_t index_1 = tri_indices[j];
            uint32_t index_2 = tri_indices[(j + 1) % 3];

            pf_vertex3d_t v1 = { 0 };
            pf_vertex3d_t v2 = { 0 };

            pf_vec3_copy_f(v1.position, positions + 3 * index_1);
            pf_vec3_copy_f(v2.position, positions + 3 * index_2);

            if (texcoords != NULL) {
                pf_vec2_copy_f(v1.texcoord, texcoords + 2 * index_1);
                pf_vec2_copy_f(v2.texcoord, texcoords + 2 * index_2);
            }

            if (normals != NULL) {
                pf_vec3_copy_f(v1.normal, normals + 3 * index_1);
                pf_vec3_copy_f(v2.normal, normals + 3 * index_2);
            }

            if (colors != NULL) {
                v1.color = colors[index_1];
                v2.color = colors[index_2];
            } else {
                v1.color = PF_WHITE;
                v2.color = PF_WHITE;
            }

            v1.index = index_1;
            v2.index = index_2;

            pf_renderer3d_line_INTERNAL(
                rn, &v1, &v2, thick, mat_model, mat_normal, mat_mvp,
                vert_proc, clip_proc, proj_proc, frag_proc,
                attr);
        }
    }
}
