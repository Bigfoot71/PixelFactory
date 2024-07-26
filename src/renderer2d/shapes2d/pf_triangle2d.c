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

#include "pixelfactory/pf_renderer2d.h"
#include "pixelfactory/pf_config.h"

/* Macros */

#define PF_TRIANGLE_TRAVEL(PIXEL_CODE)                                                  \
    pf_simd_i_t offset = pf_simd_setr_i32(0, 1, 2, 3, 4, 5, 6, 7);                      \
    pf_simd_i_t w1_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w1_x_step), offset);   \
    pf_simd_i_t w2_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w2_x_step), offset);   \
    pf_simd_i_t w3_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w3_x_step), offset);   \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        int w1 = w1_row;                                                                \
        int w2 = w2_row;                                                                \
        int w3 = w3_row;                                                                \
        for (int x = xmin; x <= xmax; x += PF_SIMD_SIZE) {                              \
            /*
                Load the current barycentric coordinates into SIMD registers
            */                                                                          \
            pf_simd_i_t w1_v = pf_simd_add_i32(pf_simd_set1_i32(w1), w1_x_step_v);      \
            pf_simd_i_t w2_v = pf_simd_add_i32(pf_simd_set1_i32(w2), w2_x_step_v);      \
            pf_simd_i_t w3_v = pf_simd_add_i32(pf_simd_set1_i32(w3), w3_x_step_v);      \
            /*
                Test if pixels are inside the triangle
            */                                                                          \
            pf_simd_i_t mask = pf_simd_or_i32(pf_simd_or_i32(w1_v, w2_v), w3_v);        \
            pf_simd_i_t mask_ge_zero = pf_simd_cmpgt_i32(mask, pf_simd_setzero_i32());  \
            /*
                Store the results if the pixels are inside the triangle
            */                                                                          \
            int mask_int = pf_simd_movemask_ps((pf_simd_t)mask_ge_zero);                \
            if (mask_int != 0) {                                                        \
                /*
                    Determine which pixels are inside
                    the triangle and update framebuffer
                */                                                                      \
                int mask_bit = 1;                                                       \
                for (int i = 0; i < PF_SIMD_SIZE; ++i) {                                \
                    if (mask_int & mask_bit) {                                          \
                        int px = x + i;                                                 \
                        /*
                            Ensure not to go out of bounds
                        */                                                              \
                        if (px <= xmax)  {                                              \
                            size_t offset = y_offset + px;                              \
                            PIXEL_CODE                                                  \
                        }                                                               \
                    }                                                                   \
                    mask_bit <<= 1;                                                     \
                }                                                                       \
            }                                                                           \
            /*
                Increment the barycentric coordinates for the next pixels
            */                                                                          \
            w1 += PF_SIMD_SIZE * w1_x_step;                                             \
            w2 += PF_SIMD_SIZE * w2_x_step;                                             \
            w3 += PF_SIMD_SIZE * w3_x_step;                                             \
        }                                                                               \
        /*
            Move to the next row in the bounding box
        */                                                                              \
        w1_row += w1_y_step;                                                            \
        w2_row += w2_y_step;                                                            \
        w3_row += w3_y_step;                                                            \
    }



#define PF_TRIANGLE_TRAVEL_OMP(PIXEL_CODE)                                              \
    /*
        Calculate the 2D bounding box of the triangle
    */                                                                                  \
    int xmin = PF_MAX(PF_MIN(x1, PF_MIN(x2, x3)), 0);                                   \
    int ymin = PF_MAX(PF_MIN(y1, PF_MIN(y2, y3)), 0);                                   \
    int xmax = PF_MIN(PF_MAX(x1, PF_MAX(x2, x3)), (int)rn->fb.w - 1);                   \
    int ymax = PF_MIN(PF_MAX(y1, PF_MAX(y2, y3)), (int)rn->fb.h - 1);                   \
    /*
        Check the order of the vertices to determine if it's a front or back face
    */                                                                                  \
    float signed_area = (x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1);                  \
    int_fast8_t is_back_face = (signed_area > 0);                                       \
    /*
        Barycentric interpolation setup
    */                                                                                  \
    int w1_x_step = y3 - y2, w1_y_step = x2 - x3;                                       \
    int w2_x_step = y1 - y3, w2_y_step = x3 - x1;                                       \
    int w3_x_step = y2 - y1, w3_y_step = x1 - x2;                                       \
    if (is_back_face) {                                                                 \
        w1_x_step = -w1_x_step, w1_y_step = -w1_y_step;                                 \
        w2_x_step = -w2_x_step, w2_y_step = -w2_y_step;                                 \
        w3_x_step = -w3_x_step, w3_y_step = -w3_y_step;                                 \
    }                                                                                   \
    int w1_row = (xmin - x2) * w1_x_step + w1_y_step * (ymin - y2);                     \
    int w2_row = (xmin - x3) * w2_x_step + w2_y_step * (ymin - y3);                     \
    int w3_row = (xmin - x1) * w3_x_step + w3_y_step * (ymin - y1);                     \
    /*
        Vector constants
    */                                                                                  \
    pf_simd_i_t offset = pf_simd_setr_i32(0, 1, 2, 3, 4, 5, 6, 7);                      \
    pf_simd_i_t w1_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w1_x_step), offset);   \
    pf_simd_i_t w2_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w2_x_step), offset);   \
    pf_simd_i_t w3_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w3_x_step), offset);   \
    _Pragma("omp parallel for schedule(dynamic)                                         \
        if ((xmax - xmin) * (ymax - ymin) >= PF_OMP_TRIANGLE_AABB_THRESHOLD)")          \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        size_t y_offset = y * rn->fb.w;                                                 \
        int iy = y - ymin;                                                              \
        int w1 = w1_row + iy * w1_y_step;                                               \
        int w2 = w2_row + iy * w2_y_step;                                               \
        int w3 = w3_row + iy * w3_y_step;                                               \
        for (int x = xmin; x <= xmax; x += PF_SIMD_SIZE) {                              \
            /*
                Load the current barycentric coordinates into AVX registers
            */                                                                          \
            pf_simd_i_t w1_v = pf_simd_add_i32(pf_simd_set1_i32(w1), w1_x_step_v);      \
            pf_simd_i_t w2_v = pf_simd_add_i32(pf_simd_set1_i32(w2), w2_x_step_v);      \
            pf_simd_i_t w3_v = pf_simd_add_i32(pf_simd_set1_i32(w3), w3_x_step_v);      \
            /*
                Test if pixels are inside the triangle
            */                                                                          \
            pf_simd_i_t mask = pf_simd_or_i32(pf_simd_or_i32(w1_v, w2_v), w3_v);        \
            pf_simd_i_t mask_ge_zero = pf_simd_cmpgt_i32(mask, pf_simd_setzero_i32());  \
            /*
                Store the results if the pixels are inside the triangle
            */                                                                          \
            int mask_int = pf_simd_movemask_ps((pf_simd_t)mask_ge_zero);                \
            if (mask_int != 0) {                                                        \
                /*
                    Determine which pixels are inside
                    the triangle and update framebuffer
                */                                                                      \
                int mask_bit = 1;                                                       \
                for (int i = 0; i < PF_SIMD_SIZE; ++i) {                                \
                    if (mask_int & mask_bit) {                                          \
                        int px = x + i;                                                 \
                        /*
                            Ensure not to go out of bounds
                        */                                                              \
                        if (px <= xmax)  {                                              \
                            size_t offset = y_offset + px;                              \
                            PIXEL_CODE                                                  \
                        }                                                               \
                    }                                                                   \
                    mask_bit <<= 1;                                                     \
                }                                                                       \
            }                                                                           \
            /*
                Increment the barycentric coordinates for the next pixels
            */                                                                          \
            w1 += PF_SIMD_SIZE * w1_x_step;                                             \
            w2 += PF_SIMD_SIZE * w2_x_step;                                             \
            w3 += PF_SIMD_SIZE * w3_x_step;                                             \
        }                                                                               \
    }

#define PF_FAST_TRIANGLE_FILLING()                                                      \
    /*
        Vector constants
    */                                                                                  \
    pf_simd_i_t offset = pf_simd_setr_i32(0, 1, 2, 3, 4, 5, 6, 7);                      \
    pf_simd_i_t w1_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w1_x_step), offset);   \
    pf_simd_i_t w2_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w2_x_step), offset);   \
    pf_simd_i_t w3_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w3_x_step), offset);   \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        size_t y_offset = y * rn->fb.w;                                                 \
        int w1 = w1_row;                                                                \
        int w2 = w2_row;                                                                \
        int w3 = w3_row;                                                                \
        for (int x = xmin; x <= xmax; x += PF_SIMD_SIZE) {                              \
            /*
                Load the current barycentric coordinates into AVX registers
            */                                                                          \
            pf_simd_i_t w1_v = pf_simd_add_i32(pf_simd_set1_i32(w1), w1_x_step_v);      \
            pf_simd_i_t w2_v = pf_simd_add_i32(pf_simd_set1_i32(w2), w2_x_step_v);      \
            pf_simd_i_t w3_v = pf_simd_add_i32(pf_simd_set1_i32(w3), w3_x_step_v);      \
            /*
                Test if pixels are inside the triangle
            */                                                                          \
            pf_simd_i_t mask = pf_simd_or_i32(pf_simd_or_i32(w1_v, w2_v), w3_v);        \
            pf_simd_i_t mask_ge_zero = pf_simd_cmpgt_i32(mask, pf_simd_setzero_i32());  \
            /*
                Load current framebuffer pixels
            */                                                                          \
            pf_simd_i_t framebuffer_colors = pf_simd_load_i32(                          \
                (pf_simd_i_t*)(rn->fb.buffer + y_offset + x));                          \
            /*
                Apply mask to update framebuffer pixels
            */                                                                          \
            pf_simd_i_t masked_colors = pf_simd_blendv_i8(framebuffer_colors, pf_simd_set1_i32(color.v), mask_ge_zero);\
            pf_simd_store_i32((pf_simd_i_t*)(rn->fb.buffer + y_offset + x), masked_colors);\
            /*
                Increment the barycentric coordinates for the next pixels
            */                                                                          \
            w1 += PF_SIMD_SIZE * w1_x_step;                                             \
            w2 += PF_SIMD_SIZE * w2_x_step;                                             \
            w3 += PF_SIMD_SIZE * w3_x_step;                                             \
        }                                                                               \
        /*
            Move to the next row in the bounding box
        */                                                                              \
        w1_row += w1_y_step;                                                            \
        w2_row += w2_y_step;                                                            \
        w3_row += w3_y_step;                                                            \
    }

#define PF_TRIANGLE_GRADIENT_TRAVEL(PIXEL_CODE)                                         \
    pf_simd_t inv_w_sum_v = pf_simd_set1_ps(inv_w_sum);                                 \
    pf_simd_i_t c1_r = pf_simd_set1_i32(c1.c.r),                                        \
            c1_g = pf_simd_set1_i32(c1.c.g),                                            \
            c1_b = pf_simd_set1_i32(c1.c.b),                                            \
            c1_a = pf_simd_set1_i32(c1.c.a);                                            \
    pf_simd_i_t c2_r = pf_simd_set1_i32(c2.c.r),                                        \
            c2_g = pf_simd_set1_i32(c2.c.g),                                            \
            c2_b = pf_simd_set1_i32(c2.c.b),                                            \
            c2_a = pf_simd_set1_i32(c2.c.a);                                            \
    pf_simd_i_t c3_r = pf_simd_set1_i32(c3.c.r),                                        \
            c3_g = pf_simd_set1_i32(c3.c.g),                                            \
            c3_b = pf_simd_set1_i32(c3.c.b),                                            \
            c3_a = pf_simd_set1_i32(c3.c.a);                                            \
    pf_simd_i_t offset = pf_simd_setr_i32(0, 1, 2, 3, 4, 5, 6, 7);                      \
    pf_simd_i_t w1_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w1_x_step), offset);   \
    pf_simd_i_t w2_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w2_x_step), offset);   \
    pf_simd_i_t w3_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w3_x_step), offset);   \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        int w1 = w1_row;                                                                \
        int w2 = w2_row;                                                                \
        int w3 = w3_row;                                                                \
        for (int x = xmin; x <= xmax; x += PF_SIMD_SIZE) {                              \
            pf_simd_i_t w1_v = pf_simd_add_i32(pf_simd_set1_i32(w1), w1_x_step_v);      \
            pf_simd_i_t w2_v = pf_simd_add_i32(pf_simd_set1_i32(w2), w2_x_step_v);      \
            pf_simd_i_t w3_v = pf_simd_add_i32(pf_simd_set1_i32(w3), w3_x_step_v);      \
            pf_simd_i_t mask = pf_simd_and_i32(pf_simd_and_i32(                         \
                pf_simd_cmpgt_i32(w1_v, pf_simd_setzero_i32()),                         \
                pf_simd_cmpgt_i32(w2_v, pf_simd_setzero_i32())),                        \
                pf_simd_cmpgt_i32(w3_v, pf_simd_setzero_i32()));                        \
            int mask_int = pf_simd_movemask_i8(mask);                                   \
            pf_simd_t w1_norm_v = pf_simd_mul_ps(pf_simd_cvti32_ps(w1_v), inv_w_sum_v); \
            pf_simd_t w2_norm_v = pf_simd_mul_ps(pf_simd_cvti32_ps(w2_v), inv_w_sum_v); \
            pf_simd_t w3_norm_v = pf_simd_mul_ps(pf_simd_cvti32_ps(w3_v), inv_w_sum_v); \
            pf_simd_i_t colors = pf_color_bary_simd(                                    \
                c1_r, c1_g, c1_b, c1_a,                                                 \
                c2_r, c2_g, c2_b, c2_a,                                                 \
                c3_r, c3_g, c3_b, c3_a,                                                 \
                w1_norm_v, w2_norm_v, w3_norm_v);                                       \
            for (int i = 0; i < PF_SIMD_SIZE; ++i) {                                    \
                if (mask_int & (1 << (i * 4))) {                                        \
                    int px = x + i;                                                     \
                    if (px <= xmax) {                                                   \
                        pf_color_t color;                                               \
                        size_t offset = y_offset + px;                                  \
                        pf_simd_store_si32(&color.v, pf_simd_permute_i32(colors, pf_simd_set1_i32(i)));\
                        PIXEL_CODE                                                      \
                    }                                                                   \
                }                                                                       \
            }                                                                           \
            w1 += PF_SIMD_SIZE * w1_x_step;                                             \
            w2 += PF_SIMD_SIZE * w2_x_step;                                             \
            w3 += PF_SIMD_SIZE * w3_x_step;                                             \
        }                                                                               \
        w1_row += w1_y_step;                                                            \
        w2_row += w2_y_step;                                                            \
        w3_row += w3_y_step;                                                            \
    }

#define PF_TRIANGLE_GRADIENT_TRAVEL_OMP(PIXEL_CODE)                                     \
    pf_simd_t inv_w_sum_v = pf_simd_set1_ps(inv_w_sum);                                 \
    pf_simd_i_t c1_r = pf_simd_set1_i32(c1.c.r),                                        \
            c1_g = pf_simd_set1_i32(c1.c.g),                                            \
            c1_b = pf_simd_set1_i32(c1.c.b),                                            \
            c1_a = pf_simd_set1_i32(c1.c.a);                                            \
    pf_simd_i_t c2_r = pf_simd_set1_i32(c2.c.r),                                        \
            c2_g = pf_simd_set1_i32(c2.c.g),                                            \
            c2_b = pf_simd_set1_i32(c2.c.b),                                            \
            c2_a = pf_simd_set1_i32(c2.c.a);                                            \
    pf_simd_i_t c3_r = pf_simd_set1_i32(c3.c.r),                                        \
            c3_g = pf_simd_set1_i32(c3.c.g),                                            \
            c3_b = pf_simd_set1_i32(c3.c.b),                                            \
            c3_a = pf_simd_set1_i32(c3.c.a);                                            \
    pf_simd_i_t offset = pf_simd_setr_i32(0, 1, 2, 3, 4, 5, 6, 7);                      \
    pf_simd_i_t w1_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w1_x_step), offset);   \
    pf_simd_i_t w2_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w2_x_step), offset);   \
    pf_simd_i_t w3_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w3_x_step), offset);   \
    _Pragma("omp parallel for schedule(dynamic)                                         \
        if ((xmax - xmin) * (ymax - ymin) >= PF_OMP_TRIANGLE_AABB_THRESHOLD)")          \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        size_t y_offset = y * rn->fb.w;                                                 \
        int iy = y - ymin;                                                              \
        int w1 = w1_row + iy * w1_y_step;                                               \
        int w2 = w2_row + iy * w2_y_step;                                               \
        int w3 = w3_row + iy * w3_y_step;                                               \
        for (int x = xmin; x <= xmax; x += PF_SIMD_SIZE) {                              \
            pf_simd_i_t w1_v = pf_simd_add_i32(pf_simd_set1_i32(w1), w1_x_step_v);      \
            pf_simd_i_t w2_v = pf_simd_add_i32(pf_simd_set1_i32(w2), w2_x_step_v);      \
            pf_simd_i_t w3_v = pf_simd_add_i32(pf_simd_set1_i32(w3), w3_x_step_v);      \
            pf_simd_i_t mask = pf_simd_and_i32(pf_simd_and_i32(                         \
                pf_simd_cmpgt_i32(w1_v, pf_simd_setzero_i32()),                         \
                pf_simd_cmpgt_i32(w2_v, pf_simd_setzero_i32())),                        \
                pf_simd_cmpgt_i32(w3_v, pf_simd_setzero_i32()));                        \
            int mask_int = pf_simd_movemask_i8(mask);                                   \
            pf_simd_t w1_norm_v = pf_simd_mul_ps(pf_simd_cvti32_ps(w1_v), inv_w_sum_v); \
            pf_simd_t w2_norm_v = pf_simd_mul_ps(pf_simd_cvti32_ps(w2_v), inv_w_sum_v); \
            pf_simd_t w3_norm_v = pf_simd_mul_ps(pf_simd_cvti32_ps(w3_v), inv_w_sum_v); \
            pf_simd_i_t colors = pf_color_bary_simd(                                    \
                c1_r, c1_g, c1_b, c1_a,                                                 \
                c2_r, c2_g, c2_b, c2_a,                                                 \
                c3_r, c3_g, c3_b, c3_a,                                                 \
                w1_norm_v, w2_norm_v, w3_norm_v);                                       \
            for (int i = 0; i < PF_SIMD_SIZE; ++i) {                                    \
                if (mask_int & (1 << (i * 4))) {                                        \
                    int px = x + i;                                                     \
                    if (px <= xmax) {                                                   \
                        pf_color_t color;                                               \
                        size_t offset = y_offset + px;                                  \
                        pf_simd_store_si32(&color.v, pf_simd_permute_i32(colors, pf_simd_set1_i32(i)));\
                        PIXEL_CODE                                                      \
                    }                                                                   \
                }                                                                       \
            }                                                                           \
            w1 += PF_SIMD_SIZE * w1_x_step;                                             \
            w2 += PF_SIMD_SIZE * w2_x_step;                                             \
            w3 += PF_SIMD_SIZE * w3_x_step;                                             \
        }                                                                               \
    }

/* Public API */

void
pf_renderer2d_triangle(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_color_t color)
{
    pf_vec2_transform_i(&x1, &y1, x1, y1, rn->mat_view);
    pf_vec2_transform_i(&x2, &y2, x2, y2, rn->mat_view);
    pf_vec2_transform_i(&x3, &y3, x3, y3, rn->mat_view);

    // Calculate the 2D bounding box of the triangle
    // Determine the minimum and maximum x and y coordinates of the triangle vertices
    int xmin = PF_MAX(PF_MIN(x1, PF_MIN(x2, x3)), 0);
    int ymin = PF_MAX(PF_MIN(y1, PF_MIN(y2, y3)), 0);
    int xmax = PF_MIN(PF_MAX(x1, PF_MAX(x2, x3)), (int)rn->fb.w - 1);
    int ymax = PF_MIN(PF_MAX(y1, PF_MAX(y2, y3)), (int)rn->fb.h - 1);

    // Check the order of the vertices to determine if it's a front or back face
    // NOTE: if signed_area is equal to 0, the face is degenerate
    float signed_area = (x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1);
    int_fast8_t is_back_face = (signed_area > 0);

    // Barycentric interpolation setup
    // Calculate the step increments for the barycentric coordinates
    int w1_x_step = y3 - y2, w1_y_step = x2 - x3;
    int w2_x_step = y1 - y3, w2_y_step = x3 - x1;
    int w3_x_step = y2 - y1, w3_y_step = x1 - x2;

    // If the triangle is a back face, invert the steps
    if (is_back_face) {
        w1_x_step = -w1_x_step, w1_y_step = -w1_y_step;
        w2_x_step = -w2_x_step, w2_y_step = -w2_y_step;
        w3_x_step = -w3_x_step, w3_y_step = -w3_y_step;
    }

    // Calculate the initial barycentric coordinates
    // for the top-left point of the bounding box
    int w1_row = (xmin - x2) * w1_x_step + w1_y_step * (ymin - y2);
    int w2_row = (xmin - x3) * w2_x_step + w2_y_step * (ymin - y3);
    int w3_row = (xmin - x1) * w3_x_step + w3_y_step * (ymin - y1);

    // Rasterization loop
    // Iterate through each pixel in the bounding box
    if (rn->blend == NULL) {
        PF_FAST_TRIANGLE_FILLING()
    } else {
#if defined(_OPENMP)
        PF_TRIANGLE_TRAVEL_OMP({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = rn->blend(*ptr, color);
        })
#else
        PF_TRIANGLE_TRAVEL({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = rn->blend(*ptr, color);
        })
#endif
    }
}

void
pf_renderer2d_triangle_gradient(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_color_t c1,
    pf_color_t c2,
    pf_color_t c3)
{
    pf_vec2_transform_i(&x1, &y1, x1, y1, rn->mat_view);
    pf_vec2_transform_i(&x2, &y2, x2, y2, rn->mat_view);
    pf_vec2_transform_i(&x3, &y3, x3, y3, rn->mat_view);

    // Calculate the 2D bounding box of the triangle
    // Determine the minimum and maximum x and y coordinates of the triangle vertices
    int xmin = PF_MAX(PF_MIN(x1, PF_MIN(x2, x3)), 0);
    int ymin = PF_MAX(PF_MIN(y1, PF_MIN(y2, y3)), 0);
    int xmax = PF_MIN(PF_MAX(x1, PF_MAX(x2, x3)), (int)rn->fb.w - 1);
    int ymax = PF_MIN(PF_MAX(y1, PF_MAX(y2, y3)), (int)rn->fb.h - 1);

    // Check the order of the vertices to determine if it's a front or back face
    // NOTE: if signed_area is equal to 0, the face is degenerate
    float signed_area = (x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1);
    int_fast8_t is_back_face = (signed_area > 0);

    // Barycentric interpolation setup
    // Calculate the step increments for the barycentric coordinates
    int w1_x_step = y3 - y2, w1_y_step = x2 - x3;
    int w2_x_step = y1 - y3, w2_y_step = x3 - x1;
    int w3_x_step = y2 - y1, w3_y_step = x1 - x2;

    // If the triangle is a back face, invert the steps
    if (is_back_face) {
        w1_x_step = -w1_x_step, w1_y_step = -w1_y_step;
        w2_x_step = -w2_x_step, w2_y_step = -w2_y_step;
        w3_x_step = -w3_x_step, w3_y_step = -w3_y_step;
    }

    // Calculate the initial barycentric coordinates
    // for the top-left point of the bounding box
    int w1_row = (xmin - x2) * w1_x_step + w1_y_step * (ymin - y2);
    int w2_row = (xmin - x3) * w2_x_step + w2_y_step * (ymin - y3);
    int w3_row = (xmin - x1) * w3_x_step + w3_y_step * (ymin - y1);

    // Calculate the inverse of the sum of the barycentric coordinates for normalization
    // NOTE: This sum remains constant throughout the triangle
    float inv_w_sum = 1.0f / (float)(w1_row + w2_row + w3_row);

    // Rasterization loop
    // Iterate through each pixel in the bounding box
#if defined(_OPENMP)
    if (rn->blend == NULL) {
        PF_TRIANGLE_GRADIENT_TRAVEL_OMP({
            rn->fb.buffer[offset] = color; 
        })
    } else {
        PF_TRIANGLE_GRADIENT_TRAVEL_OMP({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = rn->blend(*ptr, color);
        })
    }
#else
    if (rn->blend == NULL) {
        PF_TRIANGLE_GRADIENT_TRAVEL({
            rn->fb.buffer[offset] = color; 
        })
    } else {
        PF_TRIANGLE_GRADIENT_TRAVEL({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = rn->blend(*ptr, color);
        })
    }
#endif
}

void
pf_renderer2d_triangle_map(
    pf_renderer2d_t *rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_proc2d_fragment_fn frag_proc,
    const void* attr)
{
    pf_vec2_transform_i(&x1, &y1, x1, y1, rn->mat_view);
    pf_vec2_transform_i(&x2, &y2, x2, y2, rn->mat_view);
    pf_vec2_transform_i(&x3, &y3, x3, y3, rn->mat_view);

#if defined(_OPENMP)
    if (rn->blend != NULL) {
        PF_TRIANGLE_TRAVEL_OMP({
            pf_vertex2d_t vertex;
            vertex.position[0] = x;
            vertex.position[1] = y;
            vertex.texcoord[0] = 0;
            vertex.texcoord[1] = 0;
            vertex.color = PF_WHITE;

            pf_color_t *ptr = rn->fb.buffer + offset;
            pf_color_t final_color = *ptr;

            frag_proc(rn, &vertex, &final_color, attr);
            *ptr = rn->blend(*ptr, final_color);
        })
    } else {
        PF_TRIANGLE_TRAVEL_OMP({
            pf_vertex2d_t vertex;
            vertex.position[0] = x;
            vertex.position[1] = y;
            vertex.texcoord[0] = 0;
            vertex.texcoord[1] = 0;
            vertex.color = PF_WHITE;

            pf_color_t *ptr = rn->fb.buffer + offset;
            pf_color_t final_color = *ptr;

            frag_proc(rn, &vertex, &final_color, attr);
            *ptr = final_color;
        })
    }
#else
    if (rn->blend != NULL) {
        PF_TRIANGLE_TRAVEL({
            pf_vertex2d_t vertex;
            vertex.position[0] = x;
            vertex.position[1] = y;
            vertex.texcoord[0] = 0;
            vertex.texcoord[1] = 0;
            vertex.color = PF_WHITE;

            pf_color_t *ptr = rn->fb.buffer + offset;
            pf_color_t final_color = *ptr;

            frag_proc(rn, &vertex, &final_color, attr);
            *ptr = rn->blend(*ptr, final_color);
        })
    } else {
        PF_TRIANGLE_TRAVEL({
            pf_vertex2d_t vertex;
            vertex.position[0] = x;
            vertex.position[1] = y;
            vertex.texcoord[0] = 0;
            vertex.texcoord[1] = 0;
            vertex.color = PF_WHITE;

            pf_color_t *ptr = rn->fb.buffer + offset;
            pf_color_t final_color = *ptr;

            frag_proc(rn, &vertex, &final_color, attr);
            *ptr = final_color;
        })
    }
#endif
}

void
pf_renderer2d_triangle_lines(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_color_t color)
{
    pf_renderer2d_line(rn, x1, y1, x2, y2, color);
    pf_renderer2d_line(rn, x2, y2, x3, y3, color);
    pf_renderer2d_line(rn, x3, y3, x1, y1, color);
}

void
pf_renderer2d_triangle_lines_gradient(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_color_t c1,
    pf_color_t c2,
    pf_color_t c3)
{
    pf_renderer2d_line_gradient(rn, x1, y1, x2, y2, c1, c2);
    pf_renderer2d_line_gradient(rn, x2, y2, x3, y3, c2, c3);
    pf_renderer2d_line_gradient(rn, x3, y3, x1, y1, c3, c1);
}

void
pf_renderer2d_triangle_lines_map(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_proc2d_fragment_fn frag_proc,
    const void* attr)
{
    pf_renderer2d_line_map(rn, x1, y1, x2, y2, frag_proc, attr);
    pf_renderer2d_line_map(rn, x2, y2, x3, y3, frag_proc, attr);
    pf_renderer2d_line_map(rn, x3, y3, x1, y1, frag_proc, attr);
}

void
pf_renderer2d_triangle_fan(
    pf_renderer2d_t* rn,
    int* points, int count,
    pf_color_t color)
{
    if (count >= 3) {
        for (int i = 1; i < count - 1; ++i) {
            pf_renderer2d_triangle(rn,
                points[0], points[1],
                points[2 * i], points[2 * i + 1],
                points[2 * (i + 1)], points[2 * (i + 1) + 1],
                color);
        }
    }
}

void
pf_renderer2d_triangle_fan_map(
    pf_renderer2d_t* rn,
    int* points, int count,
    pf_proc2d_fragment_fn frag_proc,
    const void* attr)
{
    if (count >= 3) {
        for (int i = 1; i < count - 1; ++i) {
            pf_renderer2d_triangle_map(rn,
                points[0], points[1],
                points[2 * i], points[2 * i + 1],
                points[2 * (i + 1)], points[2 * (i + 1) + 1],
                frag_proc, attr);
        }
    }
}

void
pf_renderer2d_triangle_fan_lines(
    pf_renderer2d_t* rn,
    int* points, int count,
    pf_color_t color)
{
    if (count >= 3) {
        for (int i = 1; i < count - 1; ++i) {
            pf_renderer2d_triangle_lines(rn,
                points[0], points[1],
                points[2 * i], points[2 * i + 1],
                points[2 * (i + 1)], points[2 * (i + 1) + 1],
                color);
        }
    }
}

void
pf_renderer2d_triangle_strip(
    pf_renderer2d_t* rn,
    int* points, int count,
    pf_color_t color)
{
    if (count >= 3) {
        for (int i = 2; i < count; ++i) {
            if (i % 2 == 0) {
                pf_renderer2d_triangle(rn,
                    points[2 * i], points[2 * i + 1],
                    points[2 * (i - 2)], points[2 * (i - 2) + 1],
                    points[2 * (i - 1)], points[2 * (i - 1) + 1],
                    color);
            } else {
                pf_renderer2d_triangle(rn,
                    points[2 * (i - 2)], points[2 * (i - 2) + 1],
                    points[2 * i], points[2 * i + 1],
                    points[2 * (i - 1)], points[2 * (i - 1) + 1],
                    color);
            }
        }
    }
}

void
pf_renderer2d_triangle_strip_map(
    pf_renderer2d_t* rn,
    int* points, int count,
    pf_proc2d_fragment_fn frag_proc,
    const void* attr)
{
    if (count >= 3) {
        for (int i = 2; i < count; ++i) {
            if (i % 2 == 0) {
                pf_renderer2d_triangle_map(rn,
                    points[2 * i], points[2 * i + 1],
                    points[2 * (i - 2)], points[2 * (i - 2) + 1],
                    points[2 * (i - 1)], points[2 * (i - 1) + 1],
                    frag_proc, attr);
            } else {
                pf_renderer2d_triangle_map(rn,
                    points[2 * (i - 2)], points[2 * (i - 2) + 1],
                    points[2 * i], points[2 * i + 1],
                    points[2 * (i - 1)], points[2 * (i - 1) + 1],
                    frag_proc, attr);
            }
        }
    }
}

void
pf_renderer2d_triangle_strip_lines(
    pf_renderer2d_t* rn,
    int* points, int count,
    pf_color_t color)
{
    if (count >= 3) {
        for (int i = 2; i < count; ++i) {
            if (i % 2 == 0) {
                pf_renderer2d_triangle_lines(rn,
                    points[2 * i], points[2 * i + 1],
                    points[2 * (i - 2)], points[2 * (i - 2) + 1],
                    points[2 * (i - 1)], points[2 * (i - 1) + 1],
                    color);
            } else {
                pf_renderer2d_triangle_lines(rn,
                    points[2 * (i - 2)], points[2 * (i - 2) + 1],
                    points[2 * i], points[2 * i + 1],
                    points[2 * (i - 1)], points[2 * (i - 1) + 1],
                    color);
            }
        }
    }
}
