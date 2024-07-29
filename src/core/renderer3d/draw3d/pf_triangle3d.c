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
#include "pixelfactory/core/pf_renderer3d.h"
#include <string.h>

/* Internal Functions Declarations */

void
pf_renderer3d_triangle_INTERNAL(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, const pf_vertex3d_t* v3,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal, const pf_mat4_t mat_mvp, pf_proc3d_triangle_t* proc);


/* Triangle rasterization functions */

void
pf_renderer3d_triangle(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, const pf_vertex3d_t* v3,
    const pf_mat4_t transform, pf_proc3d_generic_t* proc)
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

    pf_proc3d_triangle_t processor = { 0 };
    processor.vertex = pf_proc3d_vertex_default;
    processor.fragment = pf_proc3d_fragment_default;
    processor.rasterizer = pf_proc3d_rasterizer_perspective_correct;
    processor.screen_projection = pf_proc3d_screen_projection_perspective_correct;

    if (proc != NULL) {
        if (proc->vertex != NULL) processor.vertex = proc->vertex;
        if (proc->fragment != NULL) processor.fragment = proc->fragment;
        if (proc->uniforms != NULL) processor.uniforms = proc->uniforms;
    }

    pf_renderer3d_triangle_INTERNAL(
        rn, v1, v2, v3, mat_model, mat_normal, mat_mvp, &processor);
}

void
pf_renderer3d_triangle_ex(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, const pf_vertex3d_t* v3,
    const pf_mat4_t transform, pf_proc3d_triangle_t* proc)
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

    pf_proc3d_triangle_t processor = { 0 };
    processor.vertex = pf_proc3d_vertex_default;
    processor.fragment = pf_proc3d_fragment_default;
    processor.rasterizer = pf_proc3d_rasterizer_perspective_correct;
    processor.screen_projection = pf_proc3d_screen_projection_perspective_correct;

    if (proc != NULL) {
        if (proc->vertex != NULL) processor.vertex = proc->vertex;
        if (proc->fragment != NULL) processor.fragment = proc->fragment;
        if (proc->rasterizer != NULL) processor.rasterizer = proc->rasterizer;
        if (proc->screen_projection != NULL) processor.screen_projection = proc->screen_projection;
        if (proc->varying != NULL) processor.varying = proc->varying;
        if (proc->uniforms != NULL) processor.uniforms = proc->uniforms;
    }

    pf_renderer3d_triangle_INTERNAL(
        rn, v1, v2, v3, mat_model, mat_normal, mat_mvp, &processor);
}
