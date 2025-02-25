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

#include "pixelfactory/components/pf_color.h"
#include "pixelfactory/core/pf_renderer.h"
#include "pixelfactory/math/pf_vec2.h"

/* Macros */

#define PF_TRIANGLE_TRAVEL(PIXEL_CODE)                                                  \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        size_t y_offset = y * rn->fb.w;                                                 \
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
    for (int y = ymin; y <= ymax; ++y) {                                                \
        size_t y_offset = y * rn->fb.w;                                                 \
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
                c1_v, c2_v, c3_v, w1_norm_v, w2_norm_v, w3_norm_v);                     \
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
                c1_v, c2_v, c3_v, w1_norm_v, w2_norm_v, w3_norm_v);                     \
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
pf_renderer_triangle2d(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_color_t color)
{
    // Transformation
    if (rn->conf2d != NULL) {
        pf_vec2_transform_i(&x1, &y1, x1, y1, rn->conf2d->mat_view);
        pf_vec2_transform_i(&x2, &y2, x2, y2, rn->conf2d->mat_view);
        pf_vec2_transform_i(&x3, &y3, x3, y3, rn->conf2d->mat_view);
    }

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

    // Vector constants
    pf_simd_i_t offset = pf_simd_setr_i32(0, 1, 2, 3, 4, 5, 6, 7);
    pf_simd_i_t w1_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w1_x_step), offset);
    pf_simd_i_t w2_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w2_x_step), offset);
    pf_simd_i_t w3_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w3_x_step), offset);

    // Rasterization loop
    // Iterate through each pixel in the bounding box
    if (rn->conf2d && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
#if defined(_OPENMP)
        PF_TRIANGLE_TRAVEL_OMP({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = blend(*ptr, color);
        })
#else
        PF_TRIANGLE_TRAVEL({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = rn->blend(*ptr, color);
        })
#endif
    } else {
        PF_FAST_TRIANGLE_FILLING()
    }
}

void
pf_renderer_triangle2d_gradient(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_color_t c1,
    pf_color_t c2,
    pf_color_t c3)
{
    // Transformation
    if (rn->conf2d != NULL) {
        pf_vec2_transform_i(&x1, &y1, x1, y1, rn->conf2d->mat_view);
        pf_vec2_transform_i(&x2, &y2, x2, y2, rn->conf2d->mat_view);
        pf_vec2_transform_i(&x3, &y3, x3, y3, rn->conf2d->mat_view);
    }

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

    // Vector constants
    pf_simd_i_t offset = pf_simd_setr_i32(0, 1, 2, 3, 4, 5, 6, 7);
    pf_simd_i_t w1_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w1_x_step), offset);
    pf_simd_i_t w2_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w2_x_step), offset);
    pf_simd_i_t w3_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w3_x_step), offset);

    // Calculate the inverse of the sum of the barycentric coordinates for normalization
    // NOTE: This sum remains constant throughout the triangle
    float inv_w_sum = 1.0f / (float)(w1_row + w2_row + w3_row);
    pf_simd_t inv_w_sum_v = pf_simd_set1_ps(inv_w_sum);

    // Load colors into SIMD registers
    // In order to carry out their interpolation in a vectorized way
    pf_color_simd_t c1_v, c2_v, c3_v;
    pf_color_to_simd(c1_v, c1);
    pf_color_to_simd(c2_v, c2);
    pf_color_to_simd(c3_v, c3);

    // Rasterization loop
    // Iterate through each pixel in the bounding box
#if defined(_OPENMP)
    if (rn->conf2d && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_TRIANGLE_GRADIENT_TRAVEL_OMP({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = blend(*ptr, color);
        })
    } else {
        PF_TRIANGLE_GRADIENT_TRAVEL_OMP({
            rn->fb.buffer[offset] = color; 
        })
    }
#else
    if (rn->conf2d && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_TRIANGLE_GRADIENT_TRAVEL({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = blend(*ptr, color);
        })
    } else {
        PF_TRIANGLE_GRADIENT_TRAVEL({
            rn->fb.buffer[offset] = color; 
        })
    }
#endif
}

void
pf_renderer_triangle2d_map(
    pf_renderer_t *rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    const pf_proc2d_t* proc)
{
    // Transformation
    if (rn->conf2d != NULL) {
        pf_vec2_transform_i(&x1, &y1, x1, y1, rn->conf2d->mat_view);
        pf_vec2_transform_i(&x2, &y2, x2, y2, rn->conf2d->mat_view);
        pf_vec2_transform_i(&x3, &y3, x3, y3, rn->conf2d->mat_view);
    }

    // Setup processor
    pf_proc2d_fragment_fn fragment = pf_proc2d_fragment_default;
    const void* uniforms = NULL;
    if (proc != NULL) {
        if (proc->fragment != NULL) fragment = proc->fragment;
        if (proc->uniforms != NULL) uniforms = proc->uniforms;
    }

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

    // Vector constants
    pf_simd_i_t offset = pf_simd_setr_i32(0, 1, 2, 3, 4, 5, 6, 7);
    pf_simd_i_t w1_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w1_x_step), offset);
    pf_simd_i_t w2_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w2_x_step), offset);
    pf_simd_i_t w3_x_step_v = pf_simd_mullo_i32(pf_simd_set1_i32(w3_x_step), offset);

#if defined(_OPENMP)
    if (rn->conf2d && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_TRIANGLE_TRAVEL_OMP({
            pf_vertex_t vertex = pf_vertex_create_2d(x, y, 0, 0, PF_WHITE);
            pf_color_t *ptr = rn->fb.buffer + offset;
            pf_color_t final_color = *ptr;

            fragment(rn, &vertex, &final_color, uniforms);
            *ptr = blend(*ptr, final_color);
        })
    } else {
        PF_TRIANGLE_TRAVEL_OMP({
            pf_vertex_t vertex = pf_vertex_create_2d(x, y, 0, 0, PF_WHITE);
            pf_color_t *ptr = rn->fb.buffer + offset;
            pf_color_t final_color = *ptr;

            fragment(rn, &vertex, &final_color, uniforms);
            *ptr = final_color;
        })
    }
#else
    if (rn->conf2d && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_TRIANGLE_TRAVEL({
            pf_vertex_t vertex = pf_vertex_create_2d(x, y, 0, 0, PF_WHITE);
            pf_color_t *ptr = rn->fb.buffer + offset;
            pf_color_t final_color = *ptr;

            fragment(rn, &vertex, &final_color, uniforms);
            *ptr = blend(*ptr, final_color);
        })
    } else {
        PF_TRIANGLE_TRAVEL({
            pf_vertex_t vertex = pf_vertex_create_2d(x, y, 0, 0, PF_WHITE);
            pf_color_t *ptr = rn->fb.buffer + offset;
            pf_color_t final_color = *ptr;

            fragment(rn, &vertex, &final_color, uniforms);
            *ptr = final_color;
        })
    }
#endif
}

void
pf_renderer_triangle2d_lines(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_color_t color)
{
    pf_renderer_line2d(rn, x1, y1, x2, y2, color);
    pf_renderer_line2d(rn, x2, y2, x3, y3, color);
    pf_renderer_line2d(rn, x3, y3, x1, y1, color);
}

void
pf_renderer_triangle2d_lines_gradient(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_color_t c1,
    pf_color_t c2,
    pf_color_t c3)
{
    pf_renderer_line2d_gradient(rn, x1, y1, x2, y2, c1, c2);
    pf_renderer_line2d_gradient(rn, x2, y2, x3, y3, c2, c3);
    pf_renderer_line2d_gradient(rn, x3, y3, x1, y1, c3, c1);
}

void
pf_renderer_triangle2d_lines_map(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    const pf_proc2d_t* proc)
{
    pf_renderer_line2d_map(rn, x1, y1, x2, y2, proc);
    pf_renderer_line2d_map(rn, x2, y2, x3, y3, proc);
    pf_renderer_line2d_map(rn, x3, y3, x1, y1, proc);
}

void
pf_renderer_triangle2d_fan(
    pf_renderer_t* rn,
    int* points, int count,
    pf_color_t color)
{
    if (count >= 3) {
        for (int i = 1; i < count - 1; ++i) {
            pf_renderer_triangle2d(rn,
                points[0], points[1],
                points[2 * i], points[2 * i + 1],
                points[2 * (i + 1)], points[2 * (i + 1) + 1],
                color);
        }
    }
}

void
pf_renderer_triangle2d_fan_map(
    pf_renderer_t* rn,
    int* points, int count,
    const pf_proc2d_t* proc)
{
    if (count >= 3) {
        for (int i = 1; i < count - 1; ++i) {
            pf_renderer_triangle2d_map(rn,
                points[0], points[1],
                points[2 * i], points[2 * i + 1],
                points[2 * (i + 1)], points[2 * (i + 1) + 1],
                proc);
        }
    }
}

void
pf_renderer_triangle2d_fan_lines(
    pf_renderer_t* rn,
    int* points, int count,
    pf_color_t color)
{
    if (count >= 3) {
        for (int i = 1; i < count - 1; ++i) {
            pf_renderer_triangle2d_lines(rn,
                points[0], points[1],
                points[2 * i], points[2 * i + 1],
                points[2 * (i + 1)], points[2 * (i + 1) + 1],
                color);
        }
    }
}

void
pf_renderer_triangle2d_strip(
    pf_renderer_t* rn,
    int* points, int count,
    pf_color_t color)
{
    if (count >= 3) {
        for (int i = 2; i < count; ++i) {
            if (i % 2 == 0) {
                pf_renderer_triangle2d(rn,
                    points[2 * i], points[2 * i + 1],
                    points[2 * (i - 2)], points[2 * (i - 2) + 1],
                    points[2 * (i - 1)], points[2 * (i - 1) + 1],
                    color);
            } else {
                pf_renderer_triangle2d(rn,
                    points[2 * (i - 2)], points[2 * (i - 2) + 1],
                    points[2 * i], points[2 * i + 1],
                    points[2 * (i - 1)], points[2 * (i - 1) + 1],
                    color);
            }
        }
    }
}

void
pf_renderer_triangle2d_strip_map(
    pf_renderer_t* rn,
    int* points, int count,
    const pf_proc2d_t* proc)
{
    if (count >= 3) {
        for (int i = 2; i < count; ++i) {
            if (i % 2 == 0) {
                pf_renderer_triangle2d_map(rn,
                    points[2 * i], points[2 * i + 1],
                    points[2 * (i - 2)], points[2 * (i - 2) + 1],
                    points[2 * (i - 1)], points[2 * (i - 1) + 1],
                    proc);
            } else {
                pf_renderer_triangle2d_map(rn,
                    points[2 * (i - 2)], points[2 * (i - 2) + 1],
                    points[2 * i], points[2 * i + 1],
                    points[2 * (i - 1)], points[2 * (i - 1) + 1],
                    proc);
            }
        }
    }
}

void
pf_renderer_triangle2d_strip_lines(
    pf_renderer_t* rn,
    int* points, int count,
    pf_color_t color)
{
    if (count >= 3) {
        for (int i = 2; i < count; ++i) {
            if (i % 2 == 0) {
                pf_renderer_triangle2d_lines(rn,
                    points[2 * i], points[2 * i + 1],
                    points[2 * (i - 2)], points[2 * (i - 2) + 1],
                    points[2 * (i - 1)], points[2 * (i - 1) + 1],
                    color);
            } else {
                pf_renderer_triangle2d_lines(rn,
                    points[2 * (i - 2)], points[2 * (i - 2) + 1],
                    points[2 * i], points[2 * i + 1],
                    points[2 * (i - 1)], points[2 * (i - 1) + 1],
                    color);
            }
        }
    }
}
