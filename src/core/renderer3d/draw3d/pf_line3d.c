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

#include "pixelfactory/core/pf_renderer3d.h"

/* Internal Functions Defintions */

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
pf_renderer3d_line(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
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

    if (vert_proc == NULL) vert_proc = pf_proc3d_vertex_default;
    if (frag_proc == NULL) frag_proc = pf_proc3d_fragment_default;

    pf_renderer3d_line_INTERNAL(rn, v1, v2, 0, mat_model, mat_normal, mat_mvp, vert_proc,
        pf_proc3d_clip_line, pf_proc3d_screen_projection_default, frag_proc, attr);
}

void
pf_renderer3d_line_ex(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
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

    /* Triangle rasterization */

    pf_renderer3d_line_INTERNAL(rn, v1, v2, 0,
        mat_model, mat_normal, mat_mvp, vert_proc,
        clip_proc, proj_proc, frag_proc, attr);
}

void
pf_renderer3d_line_thick(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, float thick, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
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

    if (vert_proc == NULL) vert_proc = pf_proc3d_vertex_default;
    if (frag_proc == NULL) frag_proc = pf_proc3d_fragment_default;

    pf_renderer3d_line_INTERNAL(rn, v1, v2, thick, mat_model, mat_normal, mat_mvp, vert_proc,
        pf_proc3d_clip_line, pf_proc3d_screen_projection_default, frag_proc, attr);
}

void
pf_renderer3d_line_thick_ex(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, float thick, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
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

    /* Triangle rasterization */

    pf_renderer3d_line_INTERNAL(rn, v1, v2, thick,
        mat_model, mat_normal, mat_mvp, vert_proc,
        clip_proc, proj_proc, frag_proc, attr);
}
