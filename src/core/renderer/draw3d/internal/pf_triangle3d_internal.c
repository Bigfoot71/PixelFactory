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

#include "pixelfactory/core/pf_renderer.h"

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
                            if (test(rn->zb.buffer[offset], z)) {                       \
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
                            if (test(rn->zb.buffer[offset], z)) {                       \
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
                pf_vec3_t bary = { w1 * inv_w_sum, w2 * inv_w_sum, w3 * inv_w_sum };            \
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
                pf_vec3_t bary = { w1 * inv_w_sum, w2 * inv_w_sum, w3 * inv_w_sum };            \
                float z = 1.0f/(bary[0]*z1 + bary[1]*z2 + bary[2]*z3);                          \
                if (test(rn->zb.buffer[offset], z)) {                                           \
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
        if (((xmax - xmin) * (ymax - ymin)) >= PF_OMP_TRIANGLE_AABB_THRESHOLD)")                \
    for (uint32_t y = ymin; y <= ymax; ++y) {                                                   \
        int w1 = w1_row + (y - ymin) * w1_y_step;                                               \
        int w2 = w2_row + (y - ymin) * w2_y_step;                                               \
        int w3 = w3_row + (y - ymin) * w3_y_step;                                               \
        for (uint32_t x = xmin; x <= xmax; ++x) {                                               \
            if ((w1 | w2 | w3) >= 0) {                                                          \
                uint32_t offset = y * rn->fb.w + x;                                             \
                pf_vec3_t bary = { w1 * inv_w_sum, w2 * inv_w_sum, w3 * inv_w_sum };            \
                float z = 1.0f / (bary[0] * z1 + bary[1] * z2 + bary[2] * z3);                  \
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
        if (((xmax - xmin) * (ymax - ymin)) >= PF_OMP_TRIANGLE_AABB_THRESHOLD)")                \
    for (uint32_t y = ymin; y <= ymax; ++y) {                                                   \
        int w1 = w1_row + (y - ymin) * w1_y_step;                                               \
        int w2 = w2_row + (y - ymin) * w2_y_step;                                               \
        int w3 = w3_row + (y - ymin) * w3_y_step;                                               \
        for (uint32_t x = xmin; x <= xmax; ++x) {                                               \
            if ((w1 | w2 | w3) >= 0) {                                                          \
                uint32_t offset = y * rn->fb.w + x;                                             \
                pf_vec3_t bary = { w1 * inv_w_sum, w2 * inv_w_sum, w3 * inv_w_sum };            \
                float z = 1.0f / (bary[0] * z1 + bary[1] * z2 + bary[2] * z3);                  \
                if (test(rn->zb.buffer[offset], z)) {                                           \
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

#define PF_PIXEL_CODE_NOBLEND()                                                 \
    pf_color_t* ptr = rn->fb.buffer + offset;                                   \
    pf_color_t final_color = *ptr;                                              \
    pf_vertex_t vertex;                                                         \
    pf_renderer_triangle_interpolation_INTERNAL(&vertex, v1, v2, v3, bary, z);  \
    fragment(rn, &vertex, &final_color, uniforms);                              \
    *ptr = final_color;

#define PF_PIXEL_CODE_BLEND()                                                   \
    pf_color_t* ptr = rn->fb.buffer + offset;                                   \
    pf_color_t final_color = *ptr;                                              \
    pf_vertex_t vertex;                                                         \
    pf_renderer_triangle_interpolation_INTERNAL(&vertex, v1, v2, v3, bary, z);  \
    fragment(rn, &vertex, &final_color, uniforms);                              \
    *ptr = blend(*ptr, final_color);

/* Helper Function Declarations */

void
pf_renderer_screen_projection_INTERNAL(
    const pf_renderer_t* rn,
    pf_vec4_t* homogeneous,
    pf_vertex_t* vertices,
    size_t vertices_count,
    int screen_pos[][2]);

void
pf_renderer_triangle_interpolation_INTERNAL(
    pf_vertex_t* out_vertex,
    pf_vertex_t* v1,
    pf_vertex_t* v2,
    pf_vertex_t* v3,
    pf_vec3_t bary,
    float z_depth);

/* Internal Clipping Function */

// TODO: Fix the warping issue that occurs during near clipping
// NOTE: To avoid this problem of deformation, it is currently advisable
//       to apply the smallest "near" value possible in your projection matrix.
static void
pf_clip3d_triangle3d_INTERNAL(
    const pf_renderer_t* rn,
    pf_vertex_t* out_vertices,
    pf_vec4_t out_homogeneous[],
    size_t* out_vertices_count)
{
    (void)rn;

    pf_vec4_t input_homogen[PF_MAX_CLIPPED_POLYGON_VERTICES];
    pf_vertex_t input_vt[PF_MAX_CLIPPED_POLYGON_VERTICES];
    int_fast8_t input_count;

    // CLIP W
    memcpy(input_homogen, out_homogeneous, (*out_vertices_count) * sizeof(pf_vec4_t));
    memcpy(input_vt, out_vertices, (*out_vertices_count) * sizeof(pf_vertex_t));
    input_count = *out_vertices_count;
    *out_vertices_count = 0;

    pf_vec4_t* prev_homogen = &input_homogen[input_count - 1];
    pf_vertex_t* prev_vt = &input_vt[input_count - 1];

    int_fast8_t prevDot = ((*prev_homogen)[3] < PF_EPSILON) ? -1 : 1;

    for (int_fast8_t i = 0; i < input_count; ++i) {
        int_fast8_t currDot = (input_homogen[i][3] < PF_EPSILON) ? -1 : 1;

        if (prevDot * currDot < 0) {
            float t = (PF_EPSILON - (*prev_homogen)[3]) / (input_homogen[i][3] - (*prev_homogen)[3]);
            pf_vec4_lerp_r(out_homogeneous[*out_vertices_count], *prev_homogen, input_homogen[i], t);
            pf_vertex_lerp(&out_vertices[*out_vertices_count], prev_vt, &input_vt[i], t);
            (*out_vertices_count)++;
        }

        if (currDot > 0) {
            pf_vec4_copy(out_homogeneous[*out_vertices_count], input_homogen[i]);
            out_vertices[*out_vertices_count] = input_vt[i];
            (*out_vertices_count)++;
        }

        prev_homogen = &input_homogen[i];
        prev_vt = &input_vt[i];
        prevDot = currDot;
    }

    if (*out_vertices_count == 0) {
        return;
    }

    // CLIP XYZ
    for (int_fast8_t iAxis = 0; iAxis < 3; iAxis++) {
        pf_vec4_t input_homogen[PF_MAX_CLIPPED_POLYGON_VERTICES];
        pf_vertex_t input_vt[PF_MAX_CLIPPED_POLYGON_VERTICES];
        int_fast8_t input_count;

        memcpy(input_homogen, out_homogeneous, (*out_vertices_count) * sizeof(pf_vec4_t));
        memcpy(input_vt, out_vertices, (*out_vertices_count) * sizeof(pf_vertex_t));
        input_count = *out_vertices_count;
        *out_vertices_count = 0;

        pf_vec4_t* prev_homogen = &input_homogen[input_count - 1];
        pf_vertex_t* prev_vt = &input_vt[input_count - 1];

        int_fast8_t prevDot = ((*prev_homogen)[iAxis] <= (*prev_homogen)[3]) ? 1 : -1;

        for (int_fast8_t i = 0; i < input_count; ++i) {
            int_fast8_t currDot = (input_homogen[i][iAxis] <= input_homogen[i][3]) ? 1 : -1;

            if (prevDot * currDot <= 0) {
                float t = (*prev_homogen)[3] - (*prev_homogen)[iAxis];
                t /= t - (input_homogen[i][3] - input_homogen[i][iAxis]);
                pf_vec4_lerp_r(out_homogeneous[*out_vertices_count], *prev_homogen, input_homogen[i], t);
                pf_vertex_lerp(&out_vertices[*out_vertices_count], prev_vt, &input_vt[i], t);
                (*out_vertices_count)++;
            }

            if (currDot > 0) {
                pf_vec4_copy(out_homogeneous[*out_vertices_count], input_homogen[i]);
                out_vertices[*out_vertices_count] = input_vt[i];
                (*out_vertices_count)++;
            }

            prev_homogen = &input_homogen[i];
            prev_vt = &input_vt[i];
            prevDot = currDot;
        }

        if (*out_vertices_count == 0) {
            return;
        }

        memcpy(input_homogen, out_homogeneous, (*out_vertices_count) * sizeof(pf_vec4_t));
        memcpy(input_vt, out_vertices, (*out_vertices_count) * sizeof(pf_vertex_t));
        input_count = *out_vertices_count;
        *out_vertices_count = 0;

        prev_homogen = &input_homogen[input_count - 1];
        prev_vt = &input_vt[input_count - 1];
        prevDot = (-(*prev_homogen)[iAxis] <= (*prev_homogen)[3]) ? 1 : -1;

        for (int_fast8_t i = 0; i < input_count; ++i) {
            int_fast8_t currDot = (-input_homogen[i][iAxis] <= input_homogen[i][3]) ? 1 : -1;

            if (prevDot * currDot <= 0) {
                float t = (*prev_homogen)[3] + (*prev_homogen)[iAxis];
                t /= t - (input_homogen[i][3] + input_homogen[i][iAxis]);
                pf_vec4_lerp_r(out_homogeneous[*out_vertices_count], *prev_homogen, input_homogen[i], t);
                pf_vertex_lerp(&out_vertices[*out_vertices_count], prev_vt, &input_vt[i], t);
                (*out_vertices_count)++;
            }

            if (currDot > 0) {
                pf_vec4_copy(out_homogeneous[*out_vertices_count], input_homogen[i]);
                out_vertices[*out_vertices_count] = input_vt[i];
                (*out_vertices_count)++;
            }

            prev_homogen = &input_homogen[i];
            prev_vt = &input_vt[i];
            prevDot = currDot;
        }

        if (*out_vertices_count == 0) {
            return;
        }
    }
}

/* Internal Rendering Functions */

void
pf_renderer_triangle3d_INTERNAL(
    pf_renderer_t* rn, pf_vertex_t vertices[PF_MAX_CLIPPED_POLYGON_VERTICES],
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal,
    const pf_mat4_t mat_mvp, const pf_proc3d_t* proc,
    bool parallelize)
{
#ifndef _OPENMP
    (void)parallelize;
#endif

    /* Copy vertices, the clipping step may result in more vertex than expected */

    pf_vec4_t homogens[PF_MAX_CLIPPED_POLYGON_VERTICES] = { 0 };
    size_t vertices_count = 3;

    /* Transform vertices */

    proc->vertex(&vertices[0], homogens[0], mat_model, mat_normal, mat_mvp, proc->uniforms);
    proc->vertex(&vertices[1], homogens[1], mat_model, mat_normal, mat_mvp, proc->uniforms);
    proc->vertex(&vertices[2], homogens[2], mat_model, mat_normal, mat_mvp, proc->uniforms);

    /* Clip triangle */

    pf_clip3d_triangle3d_INTERNAL(rn, vertices, homogens, &vertices_count);
    if (vertices_count < 3) return;

    /* Projection to screen */

    int screen_pos[PF_MAX_CLIPPED_POLYGON_VERTICES][2] = { 0 };
    pf_renderer_screen_projection_INTERNAL(rn, homogens, vertices, vertices_count, screen_pos);

    /* Get often used data */

    const pf_proc3d_fragment_fn fragment = proc->fragment;
    const void* uniforms = proc->uniforms;

    const pf_color_blend_fn blend = rn->conf3d->color_blend;
    const pf_depth_test_fn test = rn->conf3d->depth_test;

    /* Rasterize triangles */

    for (size_t i = 0; i < vertices_count - 2; ++i) {
        pf_vertex_t* v1 = &vertices[0];
        pf_vertex_t* v2 = &vertices[i + 1];
        pf_vertex_t* v3 = &vertices[i + 2];

        float z1 = homogens[0][2];
        float z2 = homogens[i + 1][2];
        float z3 = homogens[i + 2][2];

        int w1_x_step, w2_x_step, w3_x_step;
        int w1_y_step, w2_y_step, w3_y_step;
        int w1_row, w2_row, w3_row;
        float inv_w_sum;

        uint32_t xmin, ymin, xmax, ymax;
        {
            /* Get integer 2D position coordinates */

            int x1 = screen_pos[0][0],     y1 = screen_pos[0][1];
            int x2 = screen_pos[i + 1][0], y2 = screen_pos[i + 1][1];
            int x3 = screen_pos[i + 2][0], y3 = screen_pos[i + 2][1];

            /* Check if the desired face can be rendered */

            int signed_area = (x2 - x1)*(y3 - y1) - (x3 - x1)*(y2 - y1);
            pf_face_e face = (signed_area <= 0); // false == PF_BACK | true == PF_FRONT

            if ((rn->conf3d->cull_mode == PF_CULL_BACK && face == PF_BACK)
            || (rn->conf3d->cull_mode == PF_CULL_FRONT && face == PF_FRONT)) {
                continue;
            }

            /* Calculate the 2D bounding box of the triangle */

            xmin = (uint32_t)PF_MIN(x1, PF_MIN(x2, x3));
            ymin = (uint32_t)PF_MIN(y1, PF_MIN(y2, y3));
            xmax = (uint32_t)PF_MAX(x1, PF_MAX(x2, x3));
            ymax = (uint32_t)PF_MAX(y1, PF_MAX(y2, y3));

            /* Barycentric interpolation */

            w1_x_step = y3 - y2, w1_y_step = x2 - x3;
            w2_x_step = y1 - y3, w2_y_step = x3 - x1;
            w3_x_step = y2 - y1, w3_y_step = x1 - x2;

            if (face == PF_BACK) {
                w1_x_step = -w1_x_step, w1_y_step = -w1_y_step;
                w2_x_step = -w2_x_step, w2_y_step = -w2_y_step;
                w3_x_step = -w3_x_step, w3_y_step = -w3_y_step;
            }

            w1_row = (xmin - x2)*w1_x_step + w1_y_step*(ymin - y2);
            w2_row = (xmin - x3)*w2_x_step + w2_y_step*(ymin - y3);
            w3_row = (xmin - x1)*w3_x_step + w3_y_step*(ymin - y1);
        }

        /*
            Finally, we calculate the inverse of the sum of
            the barycentric coordinates for the top-left point; this
            sum always remains the same, regardless of the coordinate
            within the triangle.
        */

        inv_w_sum = 1.0f/(w1_row + w2_row + w3_row);

        /* Loop rasterization */

#if defined(_OPENMP)
        if (parallelize) {
            if (test != NULL) {
                if (blend != NULL) {
                    PF_TRIANGLE_TRAVEL_DEPTH_OMP({
                        PF_PIXEL_CODE_BLEND()
                    })
                } else {
                    PF_TRIANGLE_TRAVEL_DEPTH_OMP({
                        PF_PIXEL_CODE_NOBLEND()
                    })
                }
            } else {
                if (blend != NULL) {
                    PF_TRIANGLE_TRAVEL_NODEPTH_OMP({
                        PF_PIXEL_CODE_BLEND()
                    })
                } else {
                    PF_TRIANGLE_TRAVEL_NODEPTH_OMP({
                        PF_PIXEL_CODE_NOBLEND()
                    })
                }
            }
        } else
#endif
        {
            if (test != NULL) {
                if (blend != NULL) {
                    PF_TRIANGLE_TRAVEL_DEPTH({
                        PF_PIXEL_CODE_BLEND()
                    })
                } else {
                    PF_TRIANGLE_TRAVEL_DEPTH({
                        PF_PIXEL_CODE_NOBLEND()
                    })
                }
            } else {
                if (blend != NULL) {
                    PF_TRIANGLE_TRAVEL_NODEPTH({
                        PF_PIXEL_CODE_BLEND()
                    })
                } else {
                    PF_TRIANGLE_TRAVEL_NODEPTH({
                        PF_PIXEL_CODE_NOBLEND()
                    })
                }
            }
        }
    }
}
