#include "pixelfactory/pf_processors.h"
#include "pixelfactory/pf_renderer3d.h"
#include "pixelfactory/pf_vertex.h"

/* Internal Functions Defintions */

void
pf_renderer3d_point_INTERNAL(
    pf_renderer3d_t* rn, const pf_vertex3d_t* point, float radius,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal, const pf_mat4_t mat_mvp,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr);


/* Public API Functions */

void
pf_renderer3d_point(
    pf_renderer3d_t* rn, const pf_vertex3d_t* point, const pf_mat4_t transform,
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

    pf_renderer3d_point_INTERNAL(rn, point, 0, mat_model, mat_normal, mat_mvp, vert_proc,
        pf_proc3d_clip_point, pf_proc3d_screen_projection_default, frag_proc, attr);
}

void
pf_renderer3d_point_ex(
    pf_renderer3d_t* rn, const pf_vertex3d_t* point, const pf_mat4_t transform,
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
    if (clip_proc == NULL) clip_proc = pf_proc3d_clip_point;
    if (proj_proc == NULL) proj_proc = pf_proc3d_screen_projection_default;
    if (frag_proc == NULL) frag_proc = pf_proc3d_fragment_default;

    /* Triangle rasterization */

    pf_renderer3d_point_INTERNAL(rn, point, 0,
        mat_model, mat_normal, mat_mvp, vert_proc,
        clip_proc, proj_proc, frag_proc, attr);
}

void
pf_renderer3d_point_thick(
    pf_renderer3d_t* rn, const pf_vertex3d_t* point, float radius, const pf_mat4_t transform,
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

    pf_renderer3d_point_INTERNAL(rn, point, radius, mat_model, mat_normal, mat_mvp, vert_proc,
        pf_proc3d_clip_point, pf_proc3d_screen_projection_default, frag_proc, attr);
}

void
pf_renderer3d_point_thick_ex(
    pf_renderer3d_t* rn, const pf_vertex3d_t* point, float radius, const pf_mat4_t transform,
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
    if (clip_proc == NULL) clip_proc = pf_proc3d_clip_point;
    if (proj_proc == NULL) proj_proc = pf_proc3d_screen_projection_default;
    if (frag_proc == NULL) frag_proc = pf_proc3d_fragment_default;

    /* Triangle rasterization */

    pf_renderer3d_point_INTERNAL(rn, point, 0,
        mat_model, mat_normal, mat_mvp, vert_proc,
        clip_proc, proj_proc, frag_proc, attr);
}
