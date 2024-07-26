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

#ifndef PF_MATH_VEC4_H
#define PF_MATH_VEC4_H

#include "pf_math.h"
#include "pf_vec3.h"
#include "pf_mat4.h"

typedef PF_MATH_FLOAT pf_vec4_t[4];

static inline void
pf_vec4_zero(
    pf_vec4_t dst)
{
    memset(dst, 0, sizeof(pf_vec4_t));
}

static inline void
pf_vec4_one(
    pf_vec4_t dst,
    PF_MATH_FLOAT v)
{
    dst[0] = dst[1] = dst[2] = dst[3] = v;
}

static inline void
pf_vec4_set(
    pf_vec4_t dst,
    PF_MATH_FLOAT x,
    PF_MATH_FLOAT y,
    PF_MATH_FLOAT z,
    PF_MATH_FLOAT w)
{
    dst[0] = x, dst[1] = y, dst[2] = z, dst[3] = w;
}

static inline void
pf_vec4_copy(
    PF_MATH_FLOAT* restrict dst,
    const PF_MATH_FLOAT* restrict src)
{
    memcpy(dst, src, sizeof(pf_vec4_t));
}

static inline void
pf_vec4_swap(
    PF_MATH_FLOAT* restrict a,
    PF_MATH_FLOAT* restrict b)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        PF_MATH_FLOAT tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

static inline void
pf_vec4_neg(
    pf_vec4_t dst,
    const pf_vec4_t v)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = -v[i];
    }
}

static inline void
pf_vec4_neg_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec4_t v)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = -v[i];
    }
}

static inline void
pf_vec4_add(
    pf_vec4_t dst,
    const pf_vec4_t v1,
    const pf_vec4_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = v1[i] + v2[i];
    }
}

static inline void
pf_vec4_add_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec4_t v1,
    const pf_vec4_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = v1[i] + v2[i];
    }
}

static inline void
pf_vec4_sub(
    pf_vec4_t dst,
    const pf_vec4_t v1,
    const pf_vec4_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = v1[i] - v2[i];
    }
}

static inline void
pf_vec4_sub_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec4_t v1,
    const pf_vec4_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = v1[i] - v2[i];
    }
}

static inline void
pf_vec4_mul(
    pf_vec4_t dst,
    const pf_vec4_t v1,
    const pf_vec4_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = v1[i]*v2[i];
    }
}

static inline void
pf_vec4_mul_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec4_t v1,
    const pf_vec4_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = v1[i]*v2[i];
    }
}

static inline void
pf_vec4_div(
    pf_vec4_t dst,
    const pf_vec4_t v1,
    const pf_vec4_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = v1[i]/v2[i];
    }
}

static inline void
pf_vec4_div_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec4_t v1,
    const pf_vec4_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = v1[i]/v2[i];
    }
}

static inline void
pf_vec4_offset(
    pf_vec4_t dst,
    const pf_vec4_t v,
    PF_MATH_FLOAT scalar)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = v[i] + scalar;
    }
}

static inline void
pf_vec4_offset_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec4_t v,
    PF_MATH_FLOAT scalar)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = v[i] + scalar;
    }
}

static inline void
pf_vec4_scale(
    pf_vec4_t dst,
    const pf_vec4_t v,
    PF_MATH_FLOAT scalar)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = v[i]*scalar;
    }
}

static inline void
pf_vec4_scale_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec4_t v,
    PF_MATH_FLOAT scalar)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = v[i]*scalar;
    }
}

static inline void
pf_vec4_normalize(
    pf_vec4_t dst,
    const pf_vec4_t v)
{
    PF_MATH_FLOAT squaredLength = v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3];
    if (squaredLength == 0.0f) return;

    PF_MATH_FLOAT invLength = pf_math_rsqrt(squaredLength);

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = v[i] * invLength;
    }
}

static inline void
pf_vec4_normalize_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec4_t v)
{
    PF_MATH_FLOAT squaredLength = v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3];
    if (squaredLength == 0.0f) return;

    PF_MATH_FLOAT invLength = pf_math_rsqrt(squaredLength);

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = v[i] * invLength;
    }
}

static inline PF_MATH_FLOAT
pf_vec4_len(
    const pf_vec4_t v)
{
    return sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3]);
}

static inline PF_MATH_FLOAT
pf_vec4_len_sq(
    const pf_vec4_t v)
{
    return v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3];
}

static inline PF_MATH_FLOAT
pf_vec4_dot(
    const pf_vec4_t v1,
    const pf_vec4_t v2)
{
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2] + v1[3]*v2[3];
}

static inline void
pf_vec4_lerp(
    pf_vec4_t dst,
    const pf_vec4_t v1,
    const pf_vec4_t v2,
    PF_MATH_FLOAT t)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = v1[i] + t*(v2[i]-v1[i]);
    }
}

static inline void
pf_vec4_lerp_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec4_t v1,
    const pf_vec4_t v2,
    PF_MATH_FLOAT t)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = v1[i] + t*(v2[i]-v1[i]);
    }
}

static inline void
pf_vec4_bary(
    pf_vec4_t dst,
    const pf_vec4_t v1,
    const pf_vec4_t v2,
    const pf_vec4_t v3,
    PF_MATH_FLOAT w1,
    PF_MATH_FLOAT w2,
    PF_MATH_FLOAT w3)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = w1*v1[i] + w2*v2[i] + w3*v3[i];
    }
}

static inline void
pf_vec4_bary_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec4_t v1,
    const pf_vec4_t v2,
    const pf_vec4_t v3,
    PF_MATH_FLOAT w1,
    PF_MATH_FLOAT w2,
    PF_MATH_FLOAT w3)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = w1*v1[i] + w2*v2[i] + w3*v3[i];
    }
}

static inline void
pf_vec4_bary_v(
    pf_vec4_t dst,
    const pf_vec4_t v1,
    const pf_vec4_t v2,
    const pf_vec4_t v3,
    const pf_vec3_t w)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = w[0]*v1[i] + w[1]*v2[i] + w[2]*v3[i];
    }
}

static inline void
pf_vec4_bary_v_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec4_t v1,
    const pf_vec4_t v2,
    const pf_vec4_t v3,
    const pf_vec3_t w)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        dst[i] = w[0]*v1[i] + w[1]*v2[i] + w[2]*v3[i];
    }
}

static inline void
pf_vec4_transform(
    pf_vec4_t dst,
    const pf_vec4_t v,
    const pf_mat4_t mat)
{
    pf_vec4_t tmp = {
        mat[0]*v[0] + mat[4]*v[1] + mat[8]*v[2] + mat[12]*v[3],
        mat[1]*v[0] + mat[5]*v[1] + mat[9]*v[2] + mat[13]*v[3],
        mat[2]*v[0] + mat[6]*v[1] + mat[10]*v[2] + mat[14]*v[3],
        mat[3]*v[0] + mat[7]*v[1] + mat[11]*v[2] + mat[15]*v[3]
    };

    memcpy(dst, tmp, sizeof(pf_vec4_t));
}

static inline void
pf_vec4_transform_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_vec4_t v,
    const pf_mat4_t mat)
{
    dst[0] = mat[0]*v[0] + mat[4]*v[1] + mat[8]*v[2] + mat[12]*v[3];
    dst[1] = mat[1]*v[0] + mat[5]*v[1] + mat[9]*v[2] + mat[13]*v[3];
    dst[2] = mat[2]*v[0] + mat[6]*v[1] + mat[10]*v[2] + mat[14]*v[3];
    dst[3] = mat[3]*v[0] + mat[7]*v[1] + mat[11]*v[2] + mat[15]*v[3];
}

#endif //PF_MATH_VEC4_H
