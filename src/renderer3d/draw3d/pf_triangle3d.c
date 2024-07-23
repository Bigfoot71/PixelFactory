#include "pixelfactory/math/pf_vec4.h"
#include "pixelfactory/pf_renderer3d.h"
#include "pixelfactory/pf_stdinc.h"
#include "pixelfactory/pf_vertex.h"
#include <string.h>

/* Internal Macros */

#define PF_TRIANGLE_TRAVEL_NODEPTH(PIXEL_CODE)                                                  \
    for (uint32_t y = ymin, y_offset = ymin*rn->fb.w; y <= ymax; y++, y_offset += rn->fb.w) {   \
        int w1 = w1_row;                                                                        \
        int w2 = w2_row;                                                                        \
        int w3 = w3_row;                                                                        \
        for (uint32_t x = xmin; x <= xmax; x++) {                                               \
            if ((w1 | w2 | w3) >= 0) {                                                          \
                uint32_t offset = y_offset + x;                                                 \
                pf_vec3_t bary = {                                                              \
                    w1 * inv_w_sum,                                                             \
                    w2 * inv_w_sum,                                                             \
                    w3 * inv_w_sum                                                              \
                };                                                                              \
                float z = 1.0f/(bary[0]*z1 + bary[1]*z2 + bary[2]*z3);                          \
                PIXEL_CODE                                                                      \
            }                                                                                   \
            w1 += w1_x_step;                                                                    \
            w2 += w2_x_step;                                                                    \
            w3 += w3_x_step;                                                                    \
        }                                                                                       \
        w1_row += w1_y_step;                                                                    \
        w2_row += w2_y_step;                                                                    \
        w3_row += w3_y_step;                                                                    \
    }

#define PF_TRIANGLE_TRAVEL_DEPTH(PIXEL_CODE)                                                    \
    for (uint32_t y = ymin, y_offset = ymin*rn->fb.w; y <= ymax; y++, y_offset += rn->fb.w) {   \
        int w1 = w1_row;                                                                        \
        int w2 = w2_row;                                                                        \
        int w3 = w3_row;                                                                        \
        for (uint32_t x = xmin; x <= xmax; x++) {                                               \
            if ((w1 | w2 | w3) >= 0) {                                                          \
                uint32_t offset = y_offset + x;                                                 \
                pf_vec3_t bary = {                                                              \
                    w1 * inv_w_sum,                                                             \
                    w2 * inv_w_sum,                                                             \
                    w3 * inv_w_sum                                                              \
                };                                                                              \
                float z = 1.0f/(bary[0]*z1 + bary[1]*z2 + bary[2]*z3);                          \
                if (rn->test(z, rn->zb.buffer[offset])) {                                       \
                    rn->zb.buffer[offset] = z;                                                  \
                    PIXEL_CODE                                                                  \
                }                                                                               \
            }                                                                                   \
            w1 += w1_x_step;                                                                    \
            w2 += w2_x_step;                                                                    \
            w3 += w3_x_step;                                                                    \
        }                                                                                       \
        w1_row += w1_y_step;                                                                    \
        w2_row += w2_y_step;                                                                    \
        w3_row += w3_y_step;                                                                    \
    }

#define PF_TRIANGLE_TRAVEL_NODEPTH_OMP(PIXEL_CODE)                                              \
    _Pragma("omp parallel for schedule(dynamic)")                                               \
    for (uint32_t y = ymin; y <= ymax; y++) {                                                   \
        int i = y - ymin;                                                                       \
        int w1 = w1_row + i*w1_y_step;                                                          \
        int w2 = w2_row + i*w2_y_step;                                                          \
        int w3 = w3_row + i*w3_y_step;                                                          \
        const uint32_t y_offset = y*rn->fb.w;                                                   \
        for (uint32_t x = xmin; x <= xmax; x++) {                                               \
            if ((w1 | w2 | w3) >= 0) {                                                          \
                uint32_t offset = y_offset + x;                                                 \
                pf_vec3_t bary = {                                                              \
                    w1 * inv_w_sum,                                                             \
                    w2 * inv_w_sum,                                                             \
                    w3 * inv_w_sum                                                              \
                };                                                                              \
                float z = 1.0f/(bary[0]*z1 + bary[1]*z2 + bary[2]*z3);                          \
                PIXEL_CODE                                                                      \
            }                                                                                   \
            w1 += w1_x_step;                                                                    \
            w2 += w2_x_step;                                                                    \
            w3 += w3_x_step;                                                                    \
        }                                                                                       \
    }

#define PF_TRIANGLE_TRAVEL_DEPTH_OMP(PIXEL_CODE)                                                \
    _Pragma("omp parallel for schedule(dynamic)")                                               \
    for (uint32_t y = ymin; y <= ymax; y++) {                                                   \
        int i = y - ymin;                                                                       \
        int w1 = w1_row + i*w1_y_step;                                                          \
        int w2 = w2_row + i*w2_y_step;                                                          \
        int w3 = w3_row + i*w3_y_step;                                                          \
        const uint32_t y_offset = y*rn->fb.w;                                                   \
        for (uint32_t x = xmin; x <= xmax; x++) {                                               \
            if ((w1 | w2 | w3) >= 0) {                                                          \
                uint32_t offset = y_offset + x;                                                 \
                pf_vec3_t bary = {                                                              \
                    w1 * inv_w_sum,                                                             \
                    w2 * inv_w_sum,                                                             \
                    w3 * inv_w_sum                                                              \
                };                                                                              \
                float z = 1.0f/(bary[0]*z1 + bary[1]*z2 + bary[2]*z3);                          \
                if (rn->test(z, rn->zb.buffer[offset])) {                                       \
                    rn->zb.buffer[offset] = z;                                                  \
                    PIXEL_CODE                                                                  \
                }                                                                               \
            }                                                                                   \
            w1 += w1_x_step;                                                                    \
            w2 += w2_x_step;                                                                    \
            w3 += w3_x_step;                                                                    \
        }                                                                                       \
    }


/* Triangle rasterization functions */

void
pf_renderer3d_triangle(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, const pf_vertex3d_t* v3,
    const pf_mat4_t transform, pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
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

    pf_renderer3d_triangle_ex(rn, v1, v2, v3, model, normal, mvp, vert_proc, NULL, NULL, NULL, frag_proc, attr);
}

void
pf_renderer3d_triangle_ex(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, const pf_vertex3d_t* v3,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal, const pf_mat4_t mat_mvp,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_rasterizer_fn rast_proc,
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
    if (clip_proc == NULL) clip_proc = pf_proc3d_clip_triangle;
    if (proj_proc == NULL) proj_proc = pf_proc3d_screen_projection_perspective_correct;
    if (rast_proc == NULL) rast_proc = pf_proc3d_rasterizer_perspective_correct;
    if (frag_proc == NULL) frag_proc = pf_proc3d_fragment_default;

    /* Copy vertices, the clipping step may result in more vertex than expected */

    pf_vertex3d_t vertices[12] = { 0 };
    pf_vec4_t homogens[12] = { 0 };
    size_t vertices_count = 3;

    vertices[0] = *v1;
    vertices[1] = *v2;
    vertices[2] = *v3;

    /* Transform vertices */

    vert_proc(&vertices[0], homogens[0], model, normal, mvp, attr);
    vert_proc(&vertices[1], homogens[1], model, normal, mvp, attr);
    vert_proc(&vertices[2], homogens[2], model, normal, mvp, attr);

    /* Clip triangle */

    clip_proc(rn, vertices, homogens, &vertices_count);

    if (vertices_count < 3) {
        return;
    }

    /* Projection to screen */

    int screen_pos[12][2] = { 0 };
    proj_proc(rn, vertices, homogens, vertices_count, screen_pos);

    /* Rasterize triangles */

    for (size_t i = 0; i < vertices_count - 2; i++)
    {
        pf_vertex3d_t* v1 = &vertices[0];
        pf_vertex3d_t* v2 = &vertices[i + 1];
        pf_vertex3d_t* v3 = &vertices[i + 2];

        pf_vec4_t* h1 = &homogens[0];
        pf_vec4_t* h2 = &homogens[i + 1];
        pf_vec4_t* h3 = &homogens[i + 2];

        /* Get integer 2D position coordinates */

        int x1 = screen_pos[0][0], y1 = screen_pos[0][1];
        int x2 = screen_pos[i + 1][0], y2 = screen_pos[i + 1][1];
        int x3 = screen_pos[i + 2][0], y3 = screen_pos[i + 2][1];

        /* Check if the desired face can be rendered */

        float signed_area = (x2 - x1)*(y3 - y1) - (x3 - x1)*(y2 - y1);
        pf_face_e face = (signed_area <= 0); // false == PF_BACK | true == PF_FRONT

        if ((rn->cull_mode == PF_CULL_BACK && face == PF_BACK)
        || (rn->cull_mode == PF_CULL_FRONT && face == PF_FRONT))
        {
            continue;
        }

        /* Calculate the 2D bounding box of the triangle */

        size_t xmin = (size_t)PF_MIN(x1, PF_MIN(x2, x3));
        size_t ymin = (size_t)PF_MIN(y1, PF_MIN(y2, y3));
        size_t xmax = (size_t)PF_MAX(x1, PF_MAX(x2, x3));
        size_t ymax = (size_t)PF_MAX(y1, PF_MAX(y2, y3));

        /* Barycentric interpolation */

        int w1_x_step = y3 - y2, w1_y_step = x2 - x3;
        int w2_x_step = y1 - y3, w2_y_step = x3 - x1;
        int w3_x_step = y2 - y1, w3_y_step = x1 - x2;

        if (face == PF_BACK)
        {
            w1_x_step = -w1_x_step, w1_y_step = -w1_y_step;
            w2_x_step = -w2_x_step, w2_y_step = -w2_y_step;
            w3_x_step = -w3_x_step, w3_y_step = -w3_y_step;
        }

        int w1_row = (xmin - x2)*w1_x_step + w1_y_step*(ymin - y2);
        int w2_row = (xmin - x3)*w2_x_step + w2_y_step*(ymin - y3);
        int w3_row = (xmin - x1)*w3_x_step + w3_y_step*(ymin - y1);

        /*
            Finally, we calculate the inverse of the sum of
            the barycentric coordinates for the top-left point; this
            sum always remains the same, regardless of the coordinate
            within the triangle.
        */

        float inv_w_sum = 1.0f/(w1_row + w2_row + w3_row);

        /* Get some contextual values */

        float z1 = (*h1)[2];
        float z2 = (*h2)[2];
        float z3 = (*h3)[2];

        /* Loop rasterization */

        if (rn->test != NULL) {
#ifdef _OPENMP
            PF_TRIANGLE_TRAVEL_DEPTH_OMP({
                pf_vertex3d_t vertex;
                rast_proc(&vertex, v1, v2, v3, bary, z, attr);
                frag_proc(rn, &vertex, rn->fb.buffer + offset, attr);
            })
#else
            PF_TRIANGLE_TRAVEL_DEPTH({
                pf_vertex3d_t vertex;
                rast_proc(&vertex, v1, v2, v3, bary, z, attr);
                frag_proc(rn, &vertex, rn->fb.buffer + offset, attr);
            })
#endif
        } else {
#ifdef _OPENMP
            PF_TRIANGLE_TRAVEL_NODEPTH_OMP({
                pf_vertex3d_t vertex;
                rast_proc(&vertex, v1, v2, v3, bary, z, attr);
                frag_proc(rn, &vertex, rn->fb.buffer + offset, attr);
            })
#else
            PF_TRIANGLE_TRAVEL_NODEPTH({
                pf_vertex3d_t vertex;
                rast_proc(&vertex, v1, v2, v3, bary, z, attr);
                frag_proc(rn, &vertex, rn->fb.buffer + offset, attr);
            })
#endif
        }
    }
}
