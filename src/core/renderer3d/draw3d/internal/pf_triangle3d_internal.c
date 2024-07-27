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

/* Internal Rasterization Macros */

// NOTE: This SIMD version is slightly less efficient than the "SISD" version
// TODO: Review the logic in a more optimized way
/*
#define PF_TRIANGLE_TRAVEL_NODEPTH(PIXEL_CODE)                                          \
    pf_simd_i_t offset = pf_simd_setr_i32(0, 1, 2, 3, 4, 5, 6, 7);                      \
    pf_simd_i_t w1_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w1_x_step), offset);   \
    pf_simd_i_t w2_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w2_x_step), offset);   \
    pf_simd_i_t w3_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w3_x_step), offset);   \
    for (uint32_t y = ymin; y <= ymax; ++y) {                                           \
        int w1 = w1_row;                                                                \
        int w2 = w2_row;                                                                \
        int w3 = w3_row;                                                                \
        for (uint32_t x = xmin; x <= xmax; x += PF_SIMD_SIZE) {                         \
            pf_simd_i_t w1_v = pf_simd_add_i32(pf_simd_set1_i32(w1), w1_x_step_v);      \
            pf_simd_i_t w2_v = pf_simd_add_i32(pf_simd_set1_i32(w2), w2_x_step_v);      \
            pf_simd_i_t w3_v = pf_simd_add_i32(pf_simd_set1_i32(w3), w3_x_step_v);      \
            pf_simd_i_t mask = pf_simd_or_i32(pf_simd_or_i32(w1_v, w2_v), w3_v);        \
            pf_simd_i_t mask_ge_zero = pf_simd_cmpgt_i32(mask, pf_simd_setzero_i32());  \
            int mask_int = pf_simd_movemask_ps((pf_simd_t)mask_ge_zero);                \
            if (mask_int != 0) {                                                        \
                int mask_bit = 1;                                                       \
                for (int i = 0; i < PF_SIMD_SIZE; ++i) {                                \
                    if (mask_int & mask_bit) {                                          \
                        uint32_t px = x + i;                                            \
                        if (px <= xmax)  {                                              \
                            size_t offset = y_offset + px;                              \
                            pf_vec3_t bary = {                                          \
                                w1 * inv_w_sum,                                         \
                                w2 * inv_w_sum,                                         \
                                w3 * inv_w_sum                                          \
                            };                                                          \
                            float z = 1.0f/(bary[0]*z1 + bary[1]*z2 + bary[2]*z3);      \
                            rn->zb.buffer[offset] = z;                                  \
                            PIXEL_CODE                                                  \
                        }                                                               \
                    }                                                                   \
                    mask_bit <<= 1;                                                     \
                    w1 += w1_x_step;                                                    \
                    w2 += w2_x_step;                                                    \
                    w3 += w3_x_step;                                                    \
                }                                                                       \
            } else {                                                                    \
                w1 += PF_SIMD_SIZE * w1_x_step;                                         \
                w2 += PF_SIMD_SIZE * w2_x_step;                                         \
                w3 += PF_SIMD_SIZE * w3_x_step;                                         \
            }                                                                           \
        }                                                                               \
        w1_row += w1_y_step;                                                            \
        w2_row += w2_y_step;                                                            \
        w3_row += w3_y_step;                                                            \
    }

#define PF_TRIANGLE_TRAVEL_DEPTH(PIXEL_CODE)                                            \
    pf_simd_i_t offset = pf_simd_setr_i32(0, 1, 2, 3, 4, 5, 6, 7);                      \
    pf_simd_i_t w1_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w1_x_step), offset);   \
    pf_simd_i_t w2_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w2_x_step), offset);   \
    pf_simd_i_t w3_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w3_x_step), offset);   \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        int w1 = w1_row;                                                                \
        int w2 = w2_row;                                                                \
        int w3 = w3_row;                                                                \
        for (uint32_t x = xmin; x <= xmax; x += PF_SIMD_SIZE) {                         \
            pf_simd_i_t w1_v = pf_simd_add_i32(pf_simd_set1_i32(w1), w1_x_step_v);      \
            pf_simd_i_t w2_v = pf_simd_add_i32(pf_simd_set1_i32(w2), w2_x_step_v);      \
            pf_simd_i_t w3_v = pf_simd_add_i32(pf_simd_set1_i32(w3), w3_x_step_v);      \
            pf_simd_i_t mask = pf_simd_or_i32(pf_simd_or_i32(w1_v, w2_v), w3_v);        \
            pf_simd_i_t mask_ge_zero = pf_simd_cmpgt_i32(mask, pf_simd_setzero_i32());  \
            int mask_int = pf_simd_movemask_ps((pf_simd_t)mask_ge_zero);                \
            if (mask_int != 0) {                                                        \
                int mask_bit = 1;                                                       \
                for (uint32_t i = 0; i < PF_SIMD_SIZE; ++i) {                           \
                    if (mask_int & mask_bit) {                                          \
                        uint32_t px = x + i;                                            \
                        if (px <= xmax)  {                                              \
                            size_t offset = y_offset + px;                              \
                            pf_vec3_t bary = {                                          \
                                w1 * inv_w_sum,                                         \
                                w2 * inv_w_sum,                                         \
                                w3 * inv_w_sum                                          \
                            };                                                          \
                            float z = 1.0f/(bary[0]*z1 + bary[1]*z2 + bary[2]*z3);      \
                            if (rn->test(rn->zb.buffer[offset], z)) {                   \
                                rn->zb.buffer[offset] = z;                              \
                                PIXEL_CODE                                              \
                            }                                                           \
                        }                                                               \
                    }                                                                   \
                    mask_bit <<= 1;                                                     \
                    w1 += w1_x_step;                                                    \
                    w2 += w2_x_step;                                                    \
                    w3 += w3_x_step;                                                    \
                }                                                                       \
            } else {                                                                    \
                w1 += PF_SIMD_SIZE * w1_x_step;                                         \
                w2 += PF_SIMD_SIZE * w2_x_step;                                         \
                w3 += PF_SIMD_SIZE * w3_x_step;                                         \
            }                                                                           \
        }                                                                               \
        w1_row += w1_y_step;                                                            \
        w2_row += w2_y_step;                                                            \
        w3_row += w3_y_step;                                                            \
    }

#define PF_TRIANGLE_TRAVEL_NODEPTH_OMP(PIXEL_CODE)                                      \
    pf_simd_i_t offset = pf_simd_setr_i32(0, 1, 2, 3, 4, 5, 6, 7);                      \
    pf_simd_i_t w1_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w1_x_step), offset);   \
    pf_simd_i_t w2_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w2_x_step), offset);   \
    pf_simd_i_t w3_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w3_x_step), offset);   \
    _Pragma("omp parallel for schedule(dynamic)                                         \
        if ((xmax - xmin) * (ymax - ymin) >= PF_OMP_TRIANGLE_AABB_THRESHOLD)")          \
    for (uint32_t y = ymin; y <= ymax; ++y) {                                           \
        size_t y_offset = y * rn->fb.w;                                                 \
        int iy = y - ymin;                                                              \
        int w1 = w1_row + iy * w1_y_step;                                               \
        int w2 = w2_row + iy * w2_y_step;                                               \
        int w3 = w3_row + iy * w3_y_step;                                               \
        for (uint32_t x = xmin; x <= xmax; x += PF_SIMD_SIZE) {                         \
            pf_simd_i_t w1_v = pf_simd_add_i32(pf_simd_set1_i32(w1), w1_x_step_v);      \
            pf_simd_i_t w2_v = pf_simd_add_i32(pf_simd_set1_i32(w2), w2_x_step_v);      \
            pf_simd_i_t w3_v = pf_simd_add_i32(pf_simd_set1_i32(w3), w3_x_step_v);      \
            pf_simd_i_t mask = pf_simd_or_i32(pf_simd_or_i32(w1_v, w2_v), w3_v);        \
            pf_simd_i_t mask_ge_zero = pf_simd_cmpgt_i32(mask, pf_simd_setzero_i32());  \
            int mask_int = pf_simd_movemask_ps((pf_simd_t)mask_ge_zero);                \
            if (mask_int != 0) {                                                        \
                int mask_bit = 1;                                                       \
                for (uint32_t i = 0; i < PF_SIMD_SIZE; ++i) {                           \
                    if (mask_int & mask_bit) {                                          \
                        uint32_t px = x + i;                                            \
                        if (px <= xmax)  {                                              \
                            size_t offset = y_offset + px;                              \
                            pf_vec3_t bary = {                                          \
                                w1 * inv_w_sum,                                         \
                                w2 * inv_w_sum,                                         \
                                w3 * inv_w_sum                                          \
                            };                                                          \
                            float z = 1.0f/(bary[0]*z1 + bary[1]*z2 + bary[2]*z3);      \
                            rn->zb.buffer[offset] = z;                                  \
                            PIXEL_CODE                                                  \
                        }                                                               \
                    }                                                                   \
                    mask_bit <<= 1;                                                     \
                    w1 += w1_x_step;                                                    \
                    w2 += w2_x_step;                                                    \
                    w3 += w3_x_step;                                                    \
                }                                                                       \
            } else {                                                                    \
                w1 += PF_SIMD_SIZE * w1_x_step;                                         \
                w2 += PF_SIMD_SIZE * w2_x_step;                                         \
                w3 += PF_SIMD_SIZE * w3_x_step;                                         \
            }                                                                           \
        }                                                                               \
    }

#define PF_TRIANGLE_TRAVEL_DEPTH_OMP(PIXEL_CODE)                                        \
    pf_simd_i_t offset = pf_simd_setr_i32(0, 1, 2, 3, 4, 5, 6, 7);                      \
    pf_simd_i_t w1_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w1_x_step), offset);   \
    pf_simd_i_t w2_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w2_x_step), offset);   \
    pf_simd_i_t w3_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w3_x_step), offset);   \
    _Pragma("omp parallel for schedule(dynamic)                                         \
        if ((xmax - xmin) * (ymax - ymin) >= PF_OMP_TRIANGLE_AABB_THRESHOLD)")          \
    for (uint32_t y = ymin; y <= ymax; ++y) {                                           \
        size_t y_offset = y * rn->fb.w;                                                 \
        int iy = y - ymin;                                                              \
        int w1 = w1_row + iy * w1_y_step;                                               \
        int w2 = w2_row + iy * w2_y_step;                                               \
        int w3 = w3_row + iy * w3_y_step;                                               \
        for (uint32_t x = xmin; x <= xmax; x += PF_SIMD_SIZE) {                         \
            pf_simd_i_t w1_v = pf_simd_add_i32(pf_simd_set1_i32(w1), w1_x_step_v);      \
            pf_simd_i_t w2_v = pf_simd_add_i32(pf_simd_set1_i32(w2), w2_x_step_v);      \
            pf_simd_i_t w3_v = pf_simd_add_i32(pf_simd_set1_i32(w3), w3_x_step_v);      \
            pf_simd_i_t mask = pf_simd_or_i32(pf_simd_or_i32(w1_v, w2_v), w3_v);        \
            pf_simd_i_t mask_ge_zero = pf_simd_cmpgt_i32(mask, pf_simd_setzero_i32());  \
            int mask_int = pf_simd_movemask_ps((pf_simd_t)mask_ge_zero);                \
            if (mask_int != 0) {                                                        \
                int mask_bit = 1;                                                       \
                for (uint32_t i = 0; i < PF_SIMD_SIZE; ++i) {                           \
                    if (mask_int & mask_bit) {                                          \
                        uint32_t px = x + i;                                            \
                        if (px <= xmax)  {                                              \
                            size_t offset = y_offset + px;                              \
                            pf_vec3_t bary = {                                          \
                                w1 * inv_w_sum,                                         \
                                w2 * inv_w_sum,                                         \
                                w3 * inv_w_sum                                          \
                            };                                                          \
                            float z = 1.0f/(bary[0]*z1 + bary[1]*z2 + bary[2]*z3);      \
                            if (rn->test(rn->zb.buffer[offset], z)) {                   \
                                rn->zb.buffer[offset] = z;                              \
                                PIXEL_CODE                                              \
                            }                                                           \
                        }                                                               \
                    }                                                                   \
                    mask_bit <<= 1;                                                     \
                    w1 += w1_x_step;                                                    \
                    w2 += w2_x_step;                                                    \
                    w3 += w3_x_step;                                                    \
                }                                                                       \
            } else {                                                                    \
                w1 += PF_SIMD_SIZE * w1_x_step;                                         \
                w2 += PF_SIMD_SIZE * w2_x_step;                                         \
                w3 += PF_SIMD_SIZE * w3_x_step;                                         \
            }                                                                           \
        }                                                                               \
    }
*/

#define PF_TRIANGLE_TRAVEL_NODEPTH(PIXEL_CODE)                                                  \
    for (uint32_t y = ymin, y_offset = ymin*rn->fb.w; y <= ymax; ++y, y_offset += rn->fb.w) {   \
        int w1 = w1_row;                                                                        \
        int w2 = w2_row;                                                                        \
        int w3 = w3_row;                                                                        \
        for (uint32_t x = xmin; x <= xmax; ++x) {                                               \
            if ((w1 | w2 | w3) >= 0) {                                                          \
                uint32_t offset = y_offset + x;                                                 \
                pf_vec3_t bary = {                                                              \
                    w1 * inv_w_sum,                                                             \
                    w2 * inv_w_sum,                                                             \
                    w3 * inv_w_sum                                                              \
                };                                                                              \
                float z = 1.0f/(bary[0]*z1 + bary[1]*z2 + bary[2]*z3);                          \
                rn->zb.buffer[offset] = z;                                                      \
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
    for (uint32_t y = ymin, y_offset = ymin*rn->fb.w; y <= ymax; ++y, y_offset += rn->fb.w) {   \
        int w1 = w1_row;                                                                        \
        int w2 = w2_row;                                                                        \
        int w3 = w3_row;                                                                        \
        for (uint32_t x = xmin; x <= xmax; ++x) {                                               \
            if ((w1 | w2 | w3) >= 0) {                                                          \
                uint32_t offset = y_offset + x;                                                 \
                pf_vec3_t bary = {                                                              \
                    w1 * inv_w_sum,                                                             \
                    w2 * inv_w_sum,                                                             \
                    w3 * inv_w_sum                                                              \
                };                                                                              \
                float z = 1.0f/(bary[0]*z1 + bary[1]*z2 + bary[2]*z3);                          \
                if (rn->test(rn->zb.buffer[offset], z)) {                                       \
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
    _Pragma("omp parallel for schedule(dynamic)                                                 \
        if ((xmax - xmin) * (ymax - ymin) >= PF_OMP_TRIANGLE_AABB_THRESHOLD)")                  \
    for (uint32_t y = ymin; y <= ymax; ++y) {                                                   \
        int i = y - ymin;                                                                       \
        int w1 = w1_row + i*w1_y_step;                                                          \
        int w2 = w2_row + i*w2_y_step;                                                          \
        int w3 = w3_row + i*w3_y_step;                                                          \
        const uint32_t y_offset = y*rn->fb.w;                                                   \
        for (uint32_t x = xmin; x <= xmax; ++x) {                                               \
            if ((w1 | w2 | w3) >= 0) {                                                          \
                uint32_t offset = y_offset + x;                                                 \
                pf_vec3_t bary = {                                                              \
                    w1 * inv_w_sum,                                                             \
                    w2 * inv_w_sum,                                                             \
                    w3 * inv_w_sum                                                              \
                };                                                                              \
                float z = 1.0f/(bary[0]*z1 + bary[1]*z2 + bary[2]*z3);                          \
                rn->zb.buffer[offset] = z;                                                      \
                PIXEL_CODE                                                                      \
            }                                                                                   \
            w1 += w1_x_step;                                                                    \
            w2 += w2_x_step;                                                                    \
            w3 += w3_x_step;                                                                    \
        }                                                                                       \
    }

#define PF_TRIANGLE_TRAVEL_DEPTH_OMP(PIXEL_CODE)                                                \
    _Pragma("omp parallel for schedule(dynamic)                                                 \
        if ((xmax - xmin) * (ymax - ymin) >= PF_OMP_TRIANGLE_AABB_THRESHOLD)")                  \
    for (uint32_t y = ymin; y <= ymax; ++y) {                                                   \
        int i = y - ymin;                                                                       \
        int w1 = w1_row + i*w1_y_step;                                                          \
        int w2 = w2_row + i*w2_y_step;                                                          \
        int w3 = w3_row + i*w3_y_step;                                                          \
        const uint32_t y_offset = y*rn->fb.w;                                                   \
        for (uint32_t x = xmin; x <= xmax; ++x) {                                               \
            if ((w1 | w2 | w3) >= 0) {                                                          \
                uint32_t offset = y_offset + x;                                                 \
                pf_vec3_t bary = {                                                              \
                    w1 * inv_w_sum,                                                             \
                    w2 * inv_w_sum,                                                             \
                    w3 * inv_w_sum                                                              \
                };                                                                              \
                float z = 1.0f/(bary[0]*z1 + bary[1]*z2 + bary[2]*z3);                          \
                if (rn->test(rn->zb.buffer[offset], z)) {                                       \
                    rn->zb.buffer[offset] = z;                                                  \
                    PIXEL_CODE                                                                  \
                }                                                                               \
            }                                                                                   \
            w1 += w1_x_step;                                                                    \
            w2 += w2_x_step;                                                                    \
            w3 += w3_x_step;                                                                    \
        }                                                                                       \
    }

/* Internal Pixel Code Macros */

#define PF_PIXEL_CODE_NOBLEND()                     \
    pf_vertex3d_t vertex;                           \
    rast_proc(&vertex, v1, v2, v3, bary, z, attr);  \
    pf_color_t* ptr = rn->fb.buffer + offset;       \
    pf_color_t final_color = *ptr;                  \
    frag_proc(rn, &vertex, &final_color, attr);     \
    *ptr = final_color;

#define PF_PIXEL_CODE_BLEND()                       \
    pf_vertex3d_t vertex;                           \
    rast_proc(&vertex, v1, v2, v3, bary, z, attr);  \
    pf_color_t* ptr = rn->fb.buffer + offset;       \
    pf_color_t final_color = *ptr;                  \
    frag_proc(rn, &vertex, &final_color, attr);     \
    *ptr = rn->blend(*ptr, final_color);


/* Internal Rendering Functions */

void
pf_renderer3d_triangle_INTERNAL(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, const pf_vertex3d_t* v3,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal, const pf_mat4_t mat_mvp,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_rasterizer_fn rast_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
    /* Copy vertices, the clipping step may result in more vertex than expected */

    pf_vertex3d_t vertices[PF_MAX_CLIPPED_POLYGON_VERTICES] = { 0 };
    pf_vec4_t homogens[PF_MAX_CLIPPED_POLYGON_VERTICES] = { 0 };
    size_t vertices_count = 3;

    vertices[0] = *v1;
    vertices[1] = *v2;
    vertices[2] = *v3;

    /* Transform vertices */

    vert_proc(&vertices[0], homogens[0], mat_model, mat_normal, mat_mvp, attr);
    vert_proc(&vertices[1], homogens[1], mat_model, mat_normal, mat_mvp, attr);
    vert_proc(&vertices[2], homogens[2], mat_model, mat_normal, mat_mvp, attr);

    /* Clip triangle */

    clip_proc(rn, vertices, homogens, &vertices_count);

    if (vertices_count < 3) {
        return;
    }

    /* Projection to screen */

    int screen_pos[PF_MAX_CLIPPED_POLYGON_VERTICES][2] = { 0 };
    proj_proc(rn, vertices, homogens, vertices_count, screen_pos);

    /* Rasterize triangles */

    for (size_t i = 0; i < vertices_count - 2; ++i)
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

#ifdef _OPENMP
        if (rn->test != NULL) {
            if (rn->blend != NULL) {
                PF_TRIANGLE_TRAVEL_DEPTH_OMP({
                    PF_PIXEL_CODE_BLEND()
                })
            } else {
                PF_TRIANGLE_TRAVEL_DEPTH_OMP({
                    PF_PIXEL_CODE_NOBLEND()
                })
            }
        } else {
            if (rn->blend != NULL) {
                PF_TRIANGLE_TRAVEL_NODEPTH_OMP({
                    PF_PIXEL_CODE_BLEND()
                })
            } else {
                PF_TRIANGLE_TRAVEL_NODEPTH_OMP({
                    PF_PIXEL_CODE_NOBLEND()
                })
            }
        }
#else
        if (rn->test != NULL) {
            if (rn->blend != NULL) {
                PF_TRIANGLE_TRAVEL_DEPTH({
                    PF_PIXEL_CODE_BLEND()
                })
            } else {
                PF_TRIANGLE_TRAVEL_DEPTH({
                    PF_PIXEL_CODE_NOBLEND()
                })
            }
        } else {
            if (rn->blend != NULL) {
                PF_TRIANGLE_TRAVEL_NODEPTH({
                    PF_PIXEL_CODE_BLEND()
                })
            } else {
                PF_TRIANGLE_TRAVEL_NODEPTH({
                    PF_PIXEL_CODE_NOBLEND()
                })
            }
        }
#endif
    }
}
