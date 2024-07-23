#include "pixelfactory/pf_renderer3d.h"
#include "pixelfactory/pf_vertex.h"

/* Internal Macros */

#define PF_POINT_THICK_TRAVEL(PIXEL_CODE)                       \
    float rSq = radius*radius;                                  \
    for (int y = -radius; y <= radius; y++) {                   \
        for (int x = -radius; x <= radius; x++) {               \
            if (y*y + x*x <= rSq){                              \
                uint32_t px = screen_pos[0] + x;                \
                uint32_t py = screen_pos[1] + y;                \
                if (px < rn->fb.w && py < rn->fb.h) {           \
                    PIXEL_CODE                                  \
                }                                               \
            }                                                   \
        }                                                       \
    }


/* Public API Functions */

void
pf_renderer3d_point(
    pf_renderer3d_t* rn, const pf_vertex3d_t* point, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
    pf_mat4_t model;
    if (transform == NULL) {
        pf_mat4_identity(model);
    } else {
        pf_mat4_copy(model, transform);
    }

    pf_mat4_t mvp;
    pf_mat4_mul_r(mvp, model, rn->mat_view);
    pf_mat4_mul(mvp, mvp, rn->mat_proj);

    pf_mat4_t normal;
    pf_mat4_inverse(normal, model);
    pf_mat4_transpose(normal, normal);

    pf_renderer3d_point_ex(rn, point, model, normal, mvp, vert_proc, NULL, NULL, frag_proc, attr);
}

void
pf_renderer3d_point_ex(
    pf_renderer3d_t* rn, const pf_vertex3d_t* point,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal, const pf_mat4_t mat_mvp,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
    /* Preparation of matrices */

    pf_mat4_t mvp;
    pf_mat4_t model;
    pf_mat4_t normal;

    if (mat_model == NULL) {
        pf_mat4_identity(model);
    } else {
        pf_mat4_copy(model, mat_model);
    }

    if (mat_mvp == NULL) {
        pf_mat4_mul_r(mvp, model, rn->mat_view);
        pf_mat4_mul(mvp, mvp, rn->mat_proj);
    } else {
        pf_mat4_copy(mvp, mat_mvp);
    }

    if (mat_normal == NULL) {
        pf_mat4_inverse(normal, model);
        pf_mat4_transpose(normal, normal);
    } else {
        pf_mat4_copy(normal, mat_normal);
    }

    if (vert_proc == NULL) vert_proc = pf_proc3d_vertex_default;
    if (clip_proc == NULL) clip_proc = pf_proc3d_clip_point;
    if (proj_proc == NULL) proj_proc = pf_proc3d_screen_projection_default;
    if (frag_proc == NULL) frag_proc = pf_proc3d_fragment_default;

    /* Process Vertex */

    pf_vertex3d_t vertex = *point;
    pf_vec4_t homogen = { 0 };
    int screen_pos[2] = { 0 };
    size_t num = 1;

    vert_proc(&vertex, homogen, mat_model, mat_normal, mat_mvp, attr);
    clip_proc(rn, &vertex, &homogen, &num);
    proj_proc(rn, &vertex, &homogen, num, &screen_pos);

    /* Point rendering */

    size_t offset = screen_pos[1] * rn->fb.w + screen_pos[0];

    if (rn->test != NULL && rn->test(rn->zb.buffer[offset], homogen[2])) {
        frag_proc(rn, &vertex, rn->fb.buffer + offset, attr);
        rn->zb.buffer[offset] = homogen[2];
    } else {
        frag_proc(rn, &vertex, rn->fb.buffer + offset, attr);
    }
}

void
pf_renderer3d_point_thick(
    pf_renderer3d_t* rn, const pf_vertex3d_t* point, float radius, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
    pf_mat4_t model;
    if (transform == NULL) {
        pf_mat4_identity(model);
    } else {
        pf_mat4_copy(model, transform);
    }

    pf_mat4_t mvp;
    pf_mat4_mul_r(mvp, model, rn->mat_view);
    pf_mat4_mul(mvp, mvp, rn->mat_proj);

    pf_mat4_t normal;
    pf_mat4_inverse(normal, model);
    pf_mat4_transpose(normal, normal);

    pf_renderer3d_point_thick_ex(rn, point, radius, model, normal, mvp, vert_proc, NULL, NULL, frag_proc, attr);
}

void
pf_renderer3d_point_thick_ex(
    pf_renderer3d_t* rn, const pf_vertex3d_t* point, float radius,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal, const pf_mat4_t mat_mvp,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
    /* Preparation of matrices */

    pf_mat4_t mvp;
    pf_mat4_t model;
    pf_mat4_t normal;

    if (mat_model == NULL) {
        pf_mat4_identity(model);
    } else {
        pf_mat4_copy(model, mat_model);
    }

    if (mat_mvp == NULL) {
        pf_mat4_mul_r(mvp, model, rn->mat_view);
        pf_mat4_mul(mvp, mvp, rn->mat_proj);
    } else {
        pf_mat4_copy(mvp, mat_mvp);
    }

    if (mat_normal == NULL) {
        pf_mat4_inverse(normal, model);
        pf_mat4_transpose(normal, normal);
    } else {
        pf_mat4_copy(normal, mat_normal);
    }

    if (vert_proc == NULL) vert_proc = pf_proc3d_vertex_default;
    if (clip_proc == NULL) clip_proc = pf_proc3d_clip_point;
    if (proj_proc == NULL) proj_proc = pf_proc3d_screen_projection_default;
    if (frag_proc == NULL) frag_proc = pf_proc3d_fragment_default;

    /* Process Vertex */

    pf_vertex3d_t vertex = *point;
    pf_vec4_t homogen = { 0 };
    int screen_pos[2] = { 0 };
    size_t num = 1;

    vert_proc(&vertex, homogen, mat_model, mat_normal, mat_mvp, attr);
    clip_proc(rn, &vertex, &homogen, &num);
    proj_proc(rn, &vertex, &homogen, num, &screen_pos);

    /* Point rendering */

    if (rn->test != NULL) {
        PF_POINT_THICK_TRAVEL({
            size_t offset = py * rn->fb.w + px;
            if (rn->test(rn->zb.buffer[offset], homogen[2])) {
                frag_proc(rn, &vertex, rn->fb.buffer + offset, attr);
                rn->zb.buffer[offset] = homogen[2];
            }
        })
    } else {
        PF_POINT_THICK_TRAVEL({
            frag_proc(rn, &vertex, rn->fb.buffer + py * rn->fb.w + px, attr);
        })
    }
}
