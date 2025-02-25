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

#ifndef PF_MATH_VEC2_H
#define PF_MATH_VEC2_H

#include "pf_math.h"
#include "pf_vec3.h"
#include "pf_mat3.h"
#include "pf_mat4.h"

typedef PF_MATH_FLOAT pf_vec2_t[2];

static inline void
pf_vec2_zero(
    pf_vec2_t dst)
{
    memset(dst, 0, sizeof(pf_vec2_t));
}

static inline void
pf_vec2_one(
    pf_vec2_t dst,
    PF_MATH_FLOAT v)
{
    dst[0] = dst[1] = v;
}

static inline void
pf_vec2_set(
    pf_vec2_t dst,
    PF_MATH_FLOAT x,
    PF_MATH_FLOAT y)
{
    dst[0] = x, dst[1] = y;
}

static inline void
pf_vec2_copy(
    PF_MATH_FLOAT* restrict dst,
    const PF_MATH_FLOAT* restrict src)
{
    memcpy(dst, src, sizeof(pf_vec2_t));
}

static inline void
pf_vec2_copy_f(
    PF_MATH_FLOAT* restrict dst,
    const float* restrict src)
{
    for (int_fast8_t i = 0; i < 2; ++i) {
        dst[i] = src[i];
    }
}

static inline void
pf_vec2_copy_d(
    PF_MATH_FLOAT* restrict dst,
    const double* restrict src)
{
    for (int_fast8_t i = 0; i < 2; ++i) {
        dst[i] = src[i];
    }
}

static inline void
pf_vec2_swap(
    PF_MATH_FLOAT* restrict a,
    PF_MATH_FLOAT* restrict b)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        PF_MATH_FLOAT tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

static inline void
pf_vec2_neg(
    pf_vec2_t dst,
    const pf_vec2_t v)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = -v[i];
    }
}

static inline void
pf_vec2_neg_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec2_t v)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = -v[i];
    }
}

static inline void
pf_vec2_add(
    pf_vec2_t dst,
    const pf_vec2_t v1,
    const pf_vec2_t v2)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = v1[i] + v2[i];
    }
}

static inline void
pf_vec2_add_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec2_t v1,
    const pf_vec2_t v2)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = v1[i] + v2[i];
    }
}

static inline void
pf_vec2_sub(
    pf_vec2_t dst,
    const pf_vec2_t v1,
    const pf_vec2_t v2)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = v1[i] - v2[i];
    }
}

static inline void
pf_vec2_sub_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec2_t v1,
    const pf_vec2_t v2)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = v1[i] - v2[i];
    }
}

static inline void
pf_vec2_mul(
    pf_vec2_t dst,
    const pf_vec2_t v1,
    const pf_vec2_t v2)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = v1[i]*v2[i];
    }
}

static inline void
pf_vec2_mul_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec2_t v1,
    const pf_vec2_t v2)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = v1[i]*v2[i];
    }
}

static inline void
pf_vec2_div(
    pf_vec2_t dst,
    const pf_vec2_t v1,
    const pf_vec2_t v2)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = v1[i]/v2[i];
    }
}

static inline void
pf_vec2_div_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec2_t v1,
    const pf_vec2_t v2)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = v1[i]/v2[i];
    }
}

static inline void
pf_vec2_offset(pf_vec2_t dst,
    const pf_vec2_t v,
    PF_MATH_FLOAT scalar)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = v[i] + scalar;
    }
}

static inline void
pf_vec2_offset_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec2_t v,
    PF_MATH_FLOAT scalar)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = v[i] + scalar;
    }
}

static inline void
pf_vec2_scale(
    pf_vec2_t dst,
    const pf_vec2_t v,
    PF_MATH_FLOAT scalar)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = v[i]*scalar;
    }
}

static inline void
pf_vec2_scale_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec2_t v,
    PF_MATH_FLOAT scalar)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = v[i]*scalar;
    }
}

static inline void
pf_vec2_normalize(
    pf_vec2_t dst,
    const pf_vec2_t v)
{
    PF_MATH_FLOAT squaredLength = v[0]*v[0] + v[1]*v[1];
    if (squaredLength == 0.0f) return;

    PF_MATH_FLOAT invLength = pf_math_rsqrt(squaredLength);
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = v[i] * invLength;
    }
}

static inline void
pf_vec2_normalize_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec2_t v)
{
    PF_MATH_FLOAT squaredLength = v[0]*v[0] + v[1]*v[1];
    if (squaredLength == 0.0f) return;

    PF_MATH_FLOAT invLength = pf_math_rsqrt(squaredLength);
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = v[i] * invLength;
    }
}

static inline PF_MATH_FLOAT
pf_vec2_len(
    const pf_vec2_t v)
{
    return sqrtf(v[0]*v[0] + v[1]*v[1]);
}

static inline PF_MATH_FLOAT
pf_vec2_len_sq(
    const pf_vec2_t v)
{
    return v[0]*v[0] + v[1]*v[1];
}

static inline PF_MATH_FLOAT
pf_vec2_dot(
    const pf_vec2_t v1,
    const pf_vec2_t v2)
{
    return v1[0]*v2[0] + v1[1]*v2[1];
}

static inline PF_MATH_FLOAT
pf_vec2_distance(
    const pf_vec2_t v1,
    const pf_vec2_t v2)
{
    pf_vec2_t dt = {
        v1[0] - v2[0],
        v1[1] - v2[1]
    };

    PF_MATH_FLOAT distanceSq = dt[0]*dt[0] +
                           dt[1]*dt[1];

#ifdef PFM_FISR
    return distanceSq*pf_math_rsqrt(distanceSq);
#else
    return sqrtf(distanceSq);
#endif
}

static inline PF_MATH_FLOAT
pf_vec2_distance_sq(
    const pf_vec2_t v1,
    const pf_vec2_t v2)
{
    pf_vec2_t dt = { v1[0] - v2[0], v1[1] - v2[1] };
    return dt[0]*dt[0] + dt[1]*dt[1];
}

static inline void
pf_vec2_direction(
    pf_vec2_t dst,
    const pf_vec2_t v1,
    const pf_vec2_t v2)
{
    pf_vec2_t tmp;
    PF_MATH_FLOAT lengthSq = 0.0f;

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        tmp[i] = v1[i] - v2[i];
        lengthSq += tmp[i]*tmp[i];
    }

    PF_MATH_FLOAT invLength = pf_math_rsqrt(lengthSq);

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = tmp[i]*invLength;
    }
}

static inline void
pf_vec2_direction_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec2_t v1,
    const pf_vec2_t v2)
{
    PF_MATH_FLOAT lengthSq = 0.0f;

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = v1[i] - v2[i];
        lengthSq += dst[i]*dst[i];
    }

    PF_MATH_FLOAT invLength = pf_math_rsqrt(lengthSq);

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = dst[i]*invLength;
    }
}

static inline void
pf_vec2_lerp(
    pf_vec2_t dst,
    const pf_vec2_t v1,
    const pf_vec2_t v2,
    PF_MATH_FLOAT t)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = v1[i] + t*(v2[i]-v1[i]);
    }
}

static inline void
pf_vec2_lerp_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec2_t v1,
    const pf_vec2_t v2,
    PF_MATH_FLOAT t)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = v1[i] + t*(v2[i]-v1[i]);
    }
}

static inline void
pf_vec2_bary(
    pf_vec2_t dst,
    const pf_vec2_t v1,
    const pf_vec2_t v2,
    const pf_vec2_t v3,
    PF_MATH_FLOAT w1,
    PF_MATH_FLOAT w2,
    PF_MATH_FLOAT w3)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = w1*v1[i] + w2*v2[i] + w3*v3[i];
    }
}

static inline void
pf_vec2_bary_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec2_t v1,
    const pf_vec2_t v2,
    const pf_vec2_t v3,
    PF_MATH_FLOAT w1,
    PF_MATH_FLOAT w2,
    PF_MATH_FLOAT w3)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = w1*v1[i] + w2*v2[i] + w3*v3[i];
    }
}

static inline void
pf_vec2_bary_v(
    pf_vec2_t dst,
    const pf_vec2_t v1,
    const pf_vec2_t v2,
    const pf_vec2_t v3,
    const pf_vec3_t w)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = w[0]*v1[i] + w[1]*v2[i] + w[2]*v3[i];
    }
}

static inline void
pf_vec2_bary_v_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec2_t v1,
    const pf_vec2_t v2,
    const pf_vec2_t v3,
    const pf_vec3_t w)
{
    for (int_fast8_t i = 0; i < 2; ++i)
    {
        dst[i] = w[0]*v1[i] + w[1]*v2[i] + w[2]*v3[i];
    }
}

static inline void
pf_vec2_transform(
    pf_vec2_t dst,
    const pf_vec2_t v,
    const pf_mat3_t mat)
{
    pf_vec2_t tmp = {
        mat[0]*v[0] + mat[1]*v[1] + mat[2],
        mat[3]*v[0] + mat[4]*v[1] + mat[5]
    };

    memcpy(dst, tmp, sizeof(pf_vec2_t));
}

static inline void
pf_vec2_transform_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec2_t v,
    const pf_mat3_t mat)
{
    dst[0] = mat[0]*v[0] + mat[1]*v[1] + mat[2];
    dst[1] = mat[3]*v[0] + mat[4]*v[1] + mat[5];
}

static inline void
pf_vec2_transform_mat4(
    pf_vec2_t dst,
    const pf_vec2_t v,
    const pf_mat4_t mat)
{
    pf_vec2_t tmp = {
        mat[0]*v[0] + mat[4]*v[1] + mat[12],
        mat[1]*v[0] + mat[5]*v[1] + mat[13]
    };

    memcpy(dst, tmp, sizeof(pf_vec2_t));
}

static inline void
pf_vec2_transform_mat4_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec2_t v,
    const pf_mat4_t mat)
{
    dst[0] = mat[0]*v[0] + mat[4]*v[1] + mat[12];
    dst[1] = mat[1]*v[0] + mat[5]*v[1] + mat[13];
}

static inline void
pfm_vec2_transform_mat4_weighted_translation(
    pf_vec2_t dst, const pf_vec2_t v,
    PF_MATH_FLOAT weight, const pf_mat4_t mat)
{
    pf_vec2_t tmp = {
        mat[0]*v[0] + mat[4]*v[1] + weight*mat[12],
        mat[1]*v[0] + mat[5]*v[1] + weight*mat[13]
    };

    memcpy(dst, tmp, sizeof(pf_vec2_t));
}

static inline void
pfm_vec2_transform_mat4_weighted_translation_r(
    PF_MATH_FLOAT* restrict dst, const pf_vec2_t v,
    PF_MATH_FLOAT weight, const pf_mat4_t mat)
{
    dst[0] = mat[0]*v[0] + mat[4]*v[1] + weight*mat[12];
    dst[1] = mat[1]*v[0] + mat[5]*v[1] + weight*mat[13];
}

static inline void
pf_vec2_transform_i(
    int* restrict x_dst, int* restrict y_dst,
    int x_src, int y_src, const pf_mat3_t mat)
{
    *x_dst = (int)(mat[0]*x_src + mat[1]*y_src + mat[2] + 0.5f);
    *y_dst = (int)(mat[3]*x_src + mat[4]*y_src + mat[5] + 0.5f);
}

#endif //PF_MATH_VEC2
