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

#ifndef PF_MATH_MAT3_H
#define PF_MATH_MAT3_H

#include "pf_math.h"

#include "../misc/pf_stdinc.h"
#include <string.h>

typedef PF_MATH_FLOAT pf_mat3_t[3*3];

static inline bool
pf_mat3_is_zeroes(
    const pf_mat3_t mat)
{
    for (int_fast8_t i = 0; i < 9; ++i) {
        if (mat[i] != 0.0f) return 0;
    }
    return 1;
}

static inline bool
pf_mat3_is_identity(
    const pf_mat3_t mat)
{
    for (int_fast8_t i = 0; i < 9; ++i) {
        if (i % 4 == 0) {
            if (mat[i] != 1.0f) {
                return 0;
            }
        } else {
            if (mat[i] != 0.0f) {
                return 0;
            }
        }
    }
    return 1;
}

static inline void
pf_mat3_identity(
    pf_mat3_t dst)
{
    memset(dst, 0, sizeof(pf_mat3_t));
    dst[0] = 1.0f;
    dst[4] = 1.0f;
    dst[8] = 1.0f;
}

static inline void
pf_mat3_copy(
    PF_MATH_FLOAT* restrict dst,
    const PF_MATH_FLOAT* restrict src)
{
    memcpy(dst, src, sizeof(pf_mat3_t));
}

static inline void
pf_mat3_copy_f(
    PF_MATH_FLOAT* restrict dst,
    const float* restrict src)
{
    for (int_fast8_t i = 0; i < 9; ++i) {
        dst[i] = src[i];
    }
}

static inline void
pf_mat3_copy_d(
    PF_MATH_FLOAT* restrict dst,
    const double* restrict src)
{
    for (int_fast8_t i = 0; i < 9; ++i) {
        dst[i] = src[i];
    }
}

static inline void
pf_mat3_translate(
    pf_mat3_t dst, PF_MATH_FLOAT x, PF_MATH_FLOAT y)
{
    pf_mat3_identity(dst);
    dst[2] = x;
    dst[5] = y;
}

static inline void
pf_mat3_rotate(
    pf_mat3_t dst, PF_MATH_FLOAT angle)
{
    PF_MATH_FLOAT c = cosf(angle);
    PF_MATH_FLOAT s = sinf(angle);

    pf_mat3_identity(dst);
    dst[0] = c;
    dst[1] = -s;
    dst[3] = s;
    dst[4] = c;
}

static inline void
pf_mat3_scale(
    pf_mat3_t dst, PF_MATH_FLOAT sx, PF_MATH_FLOAT sy)
{
    pf_mat3_identity(dst);
    dst[0] = sx;
    dst[4] = sy;
}

static inline void
pf_mat3_mul(
    pf_mat3_t dst,
    const pf_mat3_t left,
    const pf_mat3_t right)
{
    pf_mat3_t temp;

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            temp[row * 3 + col] = 
                left[row * 3 + 0] * right[0 * 3 + col] + 
                left[row * 3 + 1] * right[1 * 3 + col] + 
                left[row * 3 + 2] * right[2 * 3 + col];
        }
    }

    memcpy(dst, temp, sizeof(pf_mat3_t));
}

static inline void
pf_mat3_mul_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_mat3_t left,
    const pf_mat3_t right)
{
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            dst[row * 3 + col] = 
                left[row * 3 + 0] * right[0 * 3 + col] + 
                left[row * 3 + 1] * right[1 * 3 + col] + 
                left[row * 3 + 2] * right[2 * 3 + col];
        }
    }
}

static inline void
pf_mat3_inverse(
    pf_mat3_t dst,
    const pf_mat3_t src)
{
    PF_MATH_FLOAT det = src[0] * (src[4] * src[8] - src[5] * src[7]) -
                src[1] * (src[3] * src[8] - src[5] * src[6]) +
                src[2] * (src[3] * src[7] - src[4] * src[6]);

    if (fabs(det) < 1e-6) {
        pf_mat3_identity(dst); // Non-invertible matrix case
        return;
    }

    PF_MATH_FLOAT inv_det = 1.0f / det;

    dst[0] = (src[4] * src[8] - src[5] * src[7]) * inv_det;
    dst[1] = (src[2] * src[7] - src[1] * src[8]) * inv_det;
    dst[2] = (src[1] * src[5] - src[2] * src[4]) * inv_det;
    dst[3] = (src[5] * src[6] - src[3] * src[8]) * inv_det;
    dst[4] = (src[0] * src[8] - src[2] * src[6]) * inv_det;
    dst[5] = (src[2] * src[3] - src[0] * src[5]) * inv_det;
    dst[6] = (src[3] * src[7] - src[4] * src[6]) * inv_det;
    dst[7] = (src[1] * src[6] - src[0] * src[7]) * inv_det;
    dst[8] = (src[0] * src[4] - src[1] * src[3]) * inv_det;
}

#endif //PF_MATH_MAT3_H
