#include "pixelfactory/pf_renderer2d.h"

/* Macros */

#define PF_TRIANGLE_TRAVEL(PIXEL_CODE)                                                  \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        size_t y_offset = y * rn->fb.w;                                                 \
        int w1 = w1_row;                                                                \
        int w2 = w2_row;                                                                \
        int w3 = w3_row;                                                                \
        for (int x = xmin; x <= xmax; ++x) {                                            \
            /*
                Check if the pixel is inside the triangle using barycentric coordinates
                If it is then we can draw the pixel with the given color
            */                                                                          \
            if ((w1 | w2 | w3) >= 0) {                                                  \
                size_t offset = y_offset + x;                                           \
                PIXEL_CODE                                                              \
            }                                                                           \
            /*
                Increment the barycentric coordinates for the next pixel
            */                                                                          \
            w1 += w1_x_step;                                                            \
            w2 += w2_x_step;                                                            \
            w3 += w3_x_step;                                                            \
        }                                                                               \
        /*
            Move to the next row in the bounding box
        */                                                                              \
        w1_row += w1_y_step;                                                            \
        w2_row += w2_y_step;                                                            \
        w3_row += w3_y_step;                                                            \
    }

#define PF_TRIANGLE_TRAVEL_OMP(PIXEL_CODE)                                              \
    _Pragma("omp parallel for")                                                         \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        size_t y_offset = y * rn->fb.w;                                                 \
        int iy = y - ymin;                                                              \
        int w1 = w1_row + iy * w1_y_step;                                               \
        int w2 = w2_row + iy * w2_y_step;                                               \
        int w3 = w3_row + iy * w3_y_step;                                               \
        for (int x = xmin; x <= xmax; ++x) {                                            \
            /*
                Check if the pixel is inside the triangle using barycentric coordinates
                If it is then we can draw the pixel with the given color
            */                                                                          \
            if ((w1 | w2 | w3) >= 0) {                                                  \
                size_t offset = y_offset + x;                                           \
                PIXEL_CODE                                                              \
            }                                                                           \
            /*
                Increment the barycentric coordinates for the next pixel
            */                                                                          \
            w1 += w1_x_step;                                                            \
            w2 += w2_x_step;                                                            \
            w3 += w3_x_step;                                                            \
        }                                                                               \
    }

#define PF_TRIANGLE_TRAVEL_AVX(PIXEL_CODE)                                              \
    __m256i offset = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);                         \
    __m256i w1_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w1_x_step), offset);     \
    __m256i w2_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w2_x_step), offset);     \
    __m256i w3_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w3_x_step), offset);     \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        int w1 = w1_row;                                                                \
        int w2 = w2_row;                                                                \
        int w3 = w3_row;                                                                \
        for (int x = xmin; x <= xmax; x += 8) {                                         \
            /*
                Load the current barycentric coordinates into AVX registers
            */                                                                          \
            __m256i w1_v = _mm256_add_epi32(_mm256_set1_epi32(w1), w1_x_step_v);        \
            __m256i w2_v = _mm256_add_epi32(_mm256_set1_epi32(w2), w2_x_step_v);        \
            __m256i w3_v = _mm256_add_epi32(_mm256_set1_epi32(w3), w3_x_step_v);        \
            /*
                Test if pixels are inside the triangle
            */                                                                          \
            __m256i mask = _mm256_or_si256(_mm256_or_si256(w1_v, w2_v), w3_v);          \
            __m256i mask_ge_zero = _mm256_cmpgt_epi32(mask, _mm256_setzero_si256());    \
            /*
                Store the results if the pixels are inside the triangle
            */                                                                          \
            int mask_int = _mm256_movemask_ps((__m256)mask_ge_zero);                    \
            if (mask_int != 0) {                                                        \
                /*
                    Determine which pixels are inside
                    the triangle and update framebuffer
                */                                                                      \
                int mask_bit = 1;                                                       \
                for (int i = 0; i < 8; i++) {                                           \
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
            w1 += 8 * w1_x_step;                                                        \
            w2 += 8 * w2_x_step;                                                        \
            w3 += 8 * w3_x_step;                                                        \
        }                                                                               \
        /*
            Move to the next row in the bounding box
        */                                                                              \
        w1_row += w1_y_step;                                                            \
        w2_row += w2_y_step;                                                            \
        w3_row += w3_y_step;                                                            \
    }

#define PF_TRIANGLE_TRAVEL_OMP_AVX(PIXEL_CODE)                                          \
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
    __m256i offset = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);                         \
    __m256i w1_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w1_x_step), offset);     \
    __m256i w2_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w2_x_step), offset);     \
    __m256i w3_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w3_x_step), offset);     \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        size_t y_offset = y * rn->fb.w;                                                 \
        int iy = y - ymin;                                                              \
        int w1 = w1_row + iy * w1_y_step;                                               \
        int w2 = w2_row + iy * w2_y_step;                                               \
        int w3 = w3_row + iy * w3_y_step;                                               \
        for (int x = xmin; x <= xmax; x += 8) {                                         \
            /*
                Load the current barycentric coordinates into AVX registers
            */                                                                          \
            __m256i w1_v = _mm256_add_epi32(_mm256_set1_epi32(w1), w1_x_step_v);        \
            __m256i w2_v = _mm256_add_epi32(_mm256_set1_epi32(w2), w2_x_step_v);        \
            __m256i w3_v = _mm256_add_epi32(_mm256_set1_epi32(w3), w3_x_step_v);        \
            /*
                Test if pixels are inside the triangle
            */                                                                          \
            __m256i mask = _mm256_or_si256(_mm256_or_si256(w1_v, w2_v), w3_v);          \
            __m256i mask_ge_zero = _mm256_cmpgt_epi32(mask, _mm256_setzero_si256());    \
            /*
                Store the results if the pixels are inside the triangle
            */                                                                          \
            int mask_int = _mm256_movemask_ps((__m256)mask_ge_zero);                    \
            if (mask_int != 0) {                                                        \
                /*
                    Determine which pixels are inside
                    the triangle and update framebuffer
                */                                                                      \
                int mask_bit = 1;                                                       \
                for (int i = 0; i < 8; i++) {                                           \
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
            w1 += 8 * w1_x_step;                                                        \
            w2 += 8 * w2_x_step;                                                        \
            w3 += 8 * w3_x_step;                                                        \
        }                                                                               \
    }

// NOTE: This "fast filling" macro directly fills the
//       elements of the 8 by 8 buffer using AVX2
#define PF_FAST_TRIANGLE_FILLING_AVX()                                                  \
    /*
        Vector constants
    */                                                                                  \
    __m256i offset = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);                         \
    __m256i w1_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w1_x_step), offset);     \
    __m256i w2_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w2_x_step), offset);     \
    __m256i w3_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w3_x_step), offset);     \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        size_t y_offset = y * rn->fb.w;                                                 \
        int w1 = w1_row;                                                                \
        int w2 = w2_row;                                                                \
        int w3 = w3_row;                                                                \
        for (int x = xmin; x <= xmax; x += 8) {                                         \
            /*
                Load the current barycentric coordinates into AVX registers
            */                                                                          \
            __m256i w1_v = _mm256_add_epi32(_mm256_set1_epi32(w1), w1_x_step_v);        \
            __m256i w2_v = _mm256_add_epi32(_mm256_set1_epi32(w2), w2_x_step_v);        \
            __m256i w3_v = _mm256_add_epi32(_mm256_set1_epi32(w3), w3_x_step_v);        \
            /*
                Test if pixels are inside the triangle
            */                                                                          \
            __m256i mask = _mm256_or_si256(_mm256_or_si256(w1_v, w2_v), w3_v);          \
            __m256i mask_ge_zero = _mm256_cmpgt_epi32(mask, _mm256_setzero_si256());    \
            /*
                Load current framebuffer pixels
            */                                                                          \
            __m256i framebuffer_colors = _mm256_loadu_si256(                            \
                (__m256i*)(rn->fb.buffer + y_offset + x));                              \
            /*
                Apply mask to update framebuffer pixels
            */                                                                          \
            __m256i masked_colors = _mm256_blendv_epi8(framebuffer_colors, _mm256_set1_epi32(color.v), mask_ge_zero);\
            _mm256_storeu_si256((__m256i*)(rn->fb.buffer + y_offset + x), masked_colors);\
            /*
                Increment the barycentric coordinates for the next pixels
            */                                                                          \
            w1 += 8 * w1_x_step;                                                        \
            w2 += 8 * w2_x_step;                                                        \
            w3 += 8 * w3_x_step;                                                        \
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
        for (int x = xmin; x <= xmax; ++x) {                                            \
            if ((w1 | w2 | w3) >= 0) {                                                  \
                size_t offset = y_offset + x;                                           \
                float aw1 = (float)w1*inv_w_sum;                                        \
                float aw2 = (float)w2*inv_w_sum;                                        \
                float aw3 = (float)w3*inv_w_sum;                                        \
                pf_color_t finalColor = pf_color_bary(c1, c2, c3, aw1, aw2, aw3);       \
                PIXEL_CODE                                                              \
            }                                                                           \
            w1 += w1_x_step;                                                            \
            w2 += w2_x_step;                                                            \
            w3 += w3_x_step;                                                            \
        }                                                                               \
        w1_row += w1_y_step;                                                            \
        w2_row += w2_y_step;                                                            \
        w3_row += w3_y_step;                                                            \
    }

#define PF_TRIANGLE_GRADIENT_TRAVEL_OMP(PIXEL_CODE)                                     \
    _Pragma("omp parallel for")                                                         \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        size_t y_offset = y * rn->fb.w;                                                 \
        int iy = y - ymin;                                                              \
        int w1 = w1_row + iy * w1_y_step;                                               \
        int w2 = w2_row + iy * w2_y_step;                                               \
        int w3 = w3_row + iy * w3_y_step;                                               \
        for (int x = xmin; x <= xmax; ++x) {                                            \
            if ((w1 | w2 | w3) >= 0) {                                                  \
                size_t offset = y_offset + x;                                           \
                float aw1 = (float)w1*inv_w_sum;                                        \
                float aw2 = (float)w2*inv_w_sum;                                        \
                float aw3 = (float)w3*inv_w_sum;                                        \
                pf_color_t finalColor = pf_color_bary(c1, c2, c3, aw1, aw2, aw3);       \
                PIXEL_CODE                                                              \
            }                                                                           \
            w1 += w1_x_step;                                                            \
            w2 += w2_x_step;                                                            \
            w3 += w3_x_step;                                                            \
        }                                                                               \
    }

#define PF_TRIANGLE_GRADIENT_TRAVEL_AVX(PIXEL_CODE)                                     \
    __m256 inv_w_sum_v = _mm256_set1_ps(inv_w_sum);                                     \
    __m256i c1_r = _mm256_set1_epi32(c1.c.r),                                           \
            c1_g = _mm256_set1_epi32(c1.c.g),                                           \
            c1_b = _mm256_set1_epi32(c1.c.b),                                           \
            c1_a = _mm256_set1_epi32(c1.c.a);                                           \
    __m256i c2_r = _mm256_set1_epi32(c2.c.r),                                           \
            c2_g = _mm256_set1_epi32(c2.c.g),                                           \
            c2_b = _mm256_set1_epi32(c2.c.b),                                           \
            c2_a = _mm256_set1_epi32(c2.c.a);                                           \
    __m256i c3_r = _mm256_set1_epi32(c3.c.r),                                           \
            c3_g = _mm256_set1_epi32(c3.c.g),                                           \
            c3_b = _mm256_set1_epi32(c3.c.b),                                           \
            c3_a = _mm256_set1_epi32(c3.c.a);                                           \
    __m256i offset = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);                         \
    __m256i w1_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w1_x_step), offset);     \
    __m256i w2_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w2_x_step), offset);     \
    __m256i w3_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w3_x_step), offset);     \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        int w1 = w1_row;                                                                \
        int w2 = w2_row;                                                                \
        int w3 = w3_row;                                                                \
        for (int x = xmin; x <= xmax; x += 8) {                                         \
            __m256i w1_v = _mm256_add_epi32(_mm256_set1_epi32(w1), w1_x_step_v);        \
            __m256i w2_v = _mm256_add_epi32(_mm256_set1_epi32(w2), w2_x_step_v);        \
            __m256i w3_v = _mm256_add_epi32(_mm256_set1_epi32(w3), w3_x_step_v);        \
            __m256i mask = _mm256_and_si256(_mm256_and_si256(                           \
                _mm256_cmpgt_epi32(w1_v, _mm256_setzero_si256()),                       \
                _mm256_cmpgt_epi32(w2_v, _mm256_setzero_si256())),                      \
                _mm256_cmpgt_epi32(w3_v, _mm256_setzero_si256()));                      \
            int mask_int = _mm256_movemask_epi8(mask);                                  \
            __m256 w1_norm_v = _mm256_mul_ps(_mm256_cvtepi32_ps(w1_v), inv_w_sum_v);    \
            __m256 w2_norm_v = _mm256_mul_ps(_mm256_cvtepi32_ps(w2_v), inv_w_sum_v);    \
            __m256 w3_norm_v = _mm256_mul_ps(_mm256_cvtepi32_ps(w3_v), inv_w_sum_v);    \
            __m256i colors = pf_color_bary_avx(                                         \
                c1_r, c1_g, c1_b, c1_a,                                                 \
                c2_r, c2_g, c2_b, c2_a,                                                 \
                c3_r, c3_g, c3_b, c3_a,                                                 \
                w1_norm_v, w2_norm_v, w3_norm_v);                                       \
            for (int i = 0; i < 8; i++) {                                               \
                if (mask_int & (1 << (i * 4))) {                                        \
                    int px = x + i;                                                     \
                    if (px <= xmax) {                                                   \
                        pf_color_t color;                                               \
                        size_t offset = y_offset + px;                                  \
                        _mm_storeu_si32(&color.v, _mm256_castsi256_si128(               \
                            _mm256_permutevar8x32_epi32(colors, _mm256_set1_epi32(i))));\
                        PIXEL_CODE                                                      \
                    }                                                                   \
                }                                                                       \
            }                                                                           \
            w1 += 8 * w1_x_step;                                                        \
            w2 += 8 * w2_x_step;                                                        \
            w3 += 8 * w3_x_step;                                                        \
        }                                                                               \
        w1_row += w1_y_step;                                                            \
        w2_row += w2_y_step;                                                            \
        w3_row += w3_y_step;                                                            \
    }                                                                                   \

#define PF_TRIANGLE_GRADIENT_TRAVEL_OMP_AVX(PIXEL_CODE)                                 \
    __m256 inv_w_sum_v = _mm256_set1_ps(inv_w_sum);                                     \
    __m256i c1_r = _mm256_set1_epi32(c1.c.r),                                           \
            c1_g = _mm256_set1_epi32(c1.c.g),                                           \
            c1_b = _mm256_set1_epi32(c1.c.b),                                           \
            c1_a = _mm256_set1_epi32(c1.c.a);                                           \
    __m256i c2_r = _mm256_set1_epi32(c2.c.r),                                           \
            c2_g = _mm256_set1_epi32(c2.c.g),                                           \
            c2_b = _mm256_set1_epi32(c2.c.b),                                           \
            c2_a = _mm256_set1_epi32(c2.c.a);                                           \
    __m256i c3_r = _mm256_set1_epi32(c3.c.r),                                           \
            c3_g = _mm256_set1_epi32(c3.c.g),                                           \
            c3_b = _mm256_set1_epi32(c3.c.b),                                           \
            c3_a = _mm256_set1_epi32(c3.c.a);                                           \
    __m256i offset = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);                         \
    __m256i w1_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w1_x_step), offset);     \
    __m256i w2_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w2_x_step), offset);     \
    __m256i w3_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w3_x_step), offset);     \
    _Pragma("omp parallel for")                                                         \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        size_t y_offset = y * rn->fb.w;                                                 \
        int iy = y - ymin;                                                              \
        int w1 = w1_row + iy * w1_y_step;                                               \
        int w2 = w2_row + iy * w2_y_step;                                               \
        int w3 = w3_row + iy * w3_y_step;                                               \
        for (int x = xmin; x <= xmax; x += 8) {                                         \
            __m256i w1_v = _mm256_add_epi32(_mm256_set1_epi32(w1), w1_x_step_v);        \
            __m256i w2_v = _mm256_add_epi32(_mm256_set1_epi32(w2), w2_x_step_v);        \
            __m256i w3_v = _mm256_add_epi32(_mm256_set1_epi32(w3), w3_x_step_v);        \
            __m256i mask = _mm256_and_si256(_mm256_and_si256(                           \
                _mm256_cmpgt_epi32(w1_v, _mm256_setzero_si256()),                       \
                _mm256_cmpgt_epi32(w2_v, _mm256_setzero_si256())),                      \
                _mm256_cmpgt_epi32(w3_v, _mm256_setzero_si256()));                      \
            int mask_int = _mm256_movemask_epi8(mask);                                  \
            __m256 w1_norm_v = _mm256_mul_ps(_mm256_cvtepi32_ps(w1_v), inv_w_sum_v);    \
            __m256 w2_norm_v = _mm256_mul_ps(_mm256_cvtepi32_ps(w2_v), inv_w_sum_v);    \
            __m256 w3_norm_v = _mm256_mul_ps(_mm256_cvtepi32_ps(w3_v), inv_w_sum_v);    \
            __m256i colors = pf_color_bary_avx(                                         \
                c1_r, c1_g, c1_b, c1_a,                                                 \
                c2_r, c2_g, c2_b, c2_a,                                                 \
                c3_r, c3_g, c3_b, c3_a,                                                 \
                w1_norm_v, w2_norm_v, w3_norm_v);                                       \
            for (int i = 0; i < 8; i++) {                                               \
                if (mask_int & (1 << (i * 4))) {                                        \
                    int px = x + i;                                                     \
                    if (px <= xmax) {                                                   \
                        pf_color_t color;                                               \
                        size_t offset = y_offset + px;                                  \
                        _mm_storeu_si32(&color.v, _mm256_castsi256_si128(               \
                            _mm256_permutevar8x32_epi32(colors, _mm256_set1_epi32(i))));\
                        PIXEL_CODE                                                      \
                    }                                                                   \
                }                                                                       \
            }                                                                           \
            w1 += 8 * w1_x_step;                                                        \
            w2 += 8 * w2_x_step;                                                        \
            w3 += 8 * w3_x_step;                                                        \
        }                                                                               \
    }

/* Public API */

void
pf_renderer2d_triangle(pf_renderer2d_t* rn, int x1, int y1, int x2, int y2, int x3, int y3, pf_color_t color)
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
#if defined(__AVX2__) && defined(_OPENMP)
    if (rn->blend == NULL) {
        PF_FAST_TRIANGLE_FILLING_AVX()
    } else {
        PF_TRIANGLE_TRAVEL_OMP_AVX({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = rn->blend(*ptr, color);
        })
    }
#elif defined(__AVX2__)
    if (rn->blend == NULL) {
        PF_FAST_TRIANGLE_FILLING_AVX()
    } else {
        PF_TRIANGLE_TRAVEL_AVX({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = rn->blend(*ptr, color);
        })
    }
#elif defined(_OPENMP)
    if (rn->blend == NULL) {
        PF_TRIANGLE_TRAVEL_OMP({
            rn->fb.buffer[offset] = color;
        })
    } else {
        PF_TRIANGLE_TRAVEL_OMP({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = rn->blend(*ptr, color);
        })
    }
#else
    if (rn->blend == NULL) {
        PF_TRIANGLE_TRAVEL({
            rn->fb.buffer[offset] = color;
        })
    } else {
        PF_TRIANGLE_TRAVEL({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = rn->blend(*ptr, color);
        })
    }
#endif
}

void pf_renderer2d_triangle_gradient(pf_renderer2d_t* rn, int x1, int y1, int x2, int y2, int x3, int y3, pf_color_t c1, pf_color_t c2, pf_color_t c3)
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
#if defined(__AVX2__) && defined(_OPENMP)
    if (rn->blend == NULL) {
        PF_TRIANGLE_GRADIENT_TRAVEL_OMP_AVX({
            rn->fb.buffer[offset] = color; 
        })
    } else {
        PF_TRIANGLE_GRADIENT_TRAVEL_OMP_AVX({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = rn->blend(*ptr, color);
        })
    }
#elif defined(_OPENMP)
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
#elif defined(__AVX2__)
    if (rn->blend == NULL) {
        PF_TRIANGLE_GRADIENT_TRAVEL_AVX({
            rn->fb.buffer[offset] = color; 
        })
    } else {
        PF_TRIANGLE_GRADIENT_TRAVEL_AVX({
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
pf_renderer2d_triangle_map(pf_renderer2d_t *rn, int x1, int y1, int x2, int y2, int x3, int y3, pf_proc2d_fragment_fn frag_proc, const void* attr)
{
    pf_vec2_transform_i(&x1, &y1, x1, y1, rn->mat_view);
    pf_vec2_transform_i(&x2, &y2, x2, y2, rn->mat_view);
    pf_vec2_transform_i(&x3, &y3, x3, y3, rn->mat_view);

#if defined(__AVX2__) && defined(_OPENMP)
    if (rn->blend != NULL) {
        PF_TRIANGLE_TRAVEL_OMP_AVX({
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
        PF_TRIANGLE_TRAVEL_OMP_AVX({
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
#elif defined(_OPENMP)
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
#elif defined(__AVX2__)
    if (rn->blend != NULL) {
        PF_TRIANGLE_TRAVEL_AVX({
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
        PF_TRIANGLE_TRAVEL_AVX({
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
pf_renderer2d_triangle_lines(pf_renderer2d_t* rn, int x1, int y1, int x2, int y2, int x3, int y3, pf_color_t color)
{
    pf_renderer2d_line(rn, x1, y1, x2, y2, color);
    pf_renderer2d_line(rn, x2, y2, x3, y3, color);
    pf_renderer2d_line(rn, x3, y3, x1, y1, color);
}

void
pf_renderer2d_triangle_lines_gradient(pf_renderer2d_t* rn, int x1, int y1, int x2, int y2, int x3, int y3, pf_color_t c1, pf_color_t c2, pf_color_t c3)
{
    pf_renderer2d_line_gradient(rn, x1, y1, x2, y2, c1, c2);
    pf_renderer2d_line_gradient(rn, x2, y2, x3, y3, c2, c3);
    pf_renderer2d_line_gradient(rn, x3, y3, x1, y1, c3, c1);
}

void
pf_renderer2d_triangle_lines_map(pf_renderer2d_t* rn, int x1, int y1, int x2, int y2, int x3, int y3, pf_proc2d_fragment_fn frag_proc, const void* attr)
{
    pf_renderer2d_line_map(rn, x1, y1, x2, y2, frag_proc, attr);
    pf_renderer2d_line_map(rn, x2, y2, x3, y3, frag_proc, attr);
    pf_renderer2d_line_map(rn, x3, y3, x1, y1, frag_proc, attr);
}

void
pf_renderer2d_triangle_fan(pf_renderer2d_t* rn, int* points, int count, pf_color_t color)
{
    if (count >= 3) {
        for (int i = 1; i < count - 1; i++) {
            pf_renderer2d_triangle(rn,
                points[0], points[1],
                points[2 * i], points[2 * i + 1],
                points[2 * (i + 1)], points[2 * (i + 1) + 1],
                color);
        }
    }
}

void
pf_renderer2d_triangle_fan_map(pf_renderer2d_t* rn, int* points, int count, pf_proc2d_fragment_fn frag_proc, const void* attr)
{
    if (count >= 3) {
        for (int i = 1; i < count - 1; i++) {
            pf_renderer2d_triangle_map(rn,
                points[0], points[1],
                points[2 * i], points[2 * i + 1],
                points[2 * (i + 1)], points[2 * (i + 1) + 1],
                frag_proc, attr);
        }
    }
}

void
pf_renderer2d_triangle_fan_lines(pf_renderer2d_t* rn, int* points, int count, pf_color_t color)
{
    if (count >= 3) {
        for (int i = 1; i < count - 1; i++) {
            pf_renderer2d_triangle_lines(rn,
                points[0], points[1],
                points[2 * i], points[2 * i + 1],
                points[2 * (i + 1)], points[2 * (i + 1) + 1],
                color);
        }
    }
}

void
pf_renderer2d_triangle_strip(pf_renderer2d_t* rn, int* points, int count, pf_color_t color)
{
    if (count >= 3) {
        for (int i = 2; i < count; i++) {
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
pf_renderer2d_triangle_strip_map(pf_renderer2d_t* rn, int* points, int count, pf_proc2d_fragment_fn frag_proc, const void* attr)
{
    if (count >= 3) {
        for (int i = 2; i < count; i++) {
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
pf_renderer2d_triangle_strip_lines(pf_renderer2d_t* rn, int* points, int count, pf_color_t color)
{
    if (count >= 3) {
        for (int i = 2; i < count; i++) {
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
