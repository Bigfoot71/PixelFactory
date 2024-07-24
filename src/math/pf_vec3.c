#include "pixelfactory/math/pf_vec3.h"
#include "pixelfactory/pf_stdinc.h"
#include <string.h>

void
pf_vec3_zero(pf_vec3_t dst)
{
    memset(dst, 0, sizeof(pf_vec3_t));
}

void
pf_vec3_one(pf_vec3_t dst, PF_MATH_FLOAT v)
{
    dst[0] = dst[1] = dst[2] = v;
}

void
pf_vec3_set(pf_vec3_t dst, PF_MATH_FLOAT x, PF_MATH_FLOAT y, PF_MATH_FLOAT z)
{
    dst[0] = x, dst[1] = y, dst[2] = z;
}

void
pf_vec3_copy(float* restrict dst, const float* restrict src)
{
    memcpy(dst, src, sizeof(pf_vec3_t));
}

void
pf_vec3_swap(float* restrict a, float* restrict b)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        PF_MATH_FLOAT tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

void
pf_vec3_neg(pf_vec3_t dst, const pf_vec3_t v)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = -v[i];
    }
}

void
pf_vec3_neg_r(float* restrict dst, const pf_vec3_t v)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = -v[i];
    }
}

void
pf_vec3_add(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = v1[i] + v2[i];
    }
}

void
pf_vec3_add_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = v1[i] + v2[i];
    }
}

void
pf_vec3_sub(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = v1[i] - v2[i];
    }
}

void
pf_vec3_sub_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = v1[i] - v2[i];
    }
}

void
pf_vec3_mul(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = v1[i]*v2[i];
    }
}

void
pf_vec3_mul_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = v1[i]*v2[i];
    }
}

void
pf_vec3_div(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = v1[i]/v2[i];
    }
}

void
pf_vec3_div_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = v1[i]/v2[i];
    }
}

void
pf_vec3_offset(pf_vec3_t dst, const pf_vec3_t v, PF_MATH_FLOAT scalar)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = v[i] + scalar;
    }
}

void
pf_vec3_offset_r(float* restrict dst, const pf_vec3_t v, PF_MATH_FLOAT scalar)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = v[i] + scalar;
    }
}

void
pf_vec3_scale(pf_vec3_t dst, const pf_vec3_t v, PF_MATH_FLOAT scalar)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = v[i]*scalar;
    }
}

void
pf_vec3_scale_r(float* restrict dst, const pf_vec3_t v, PF_MATH_FLOAT scalar)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = v[i]*scalar;
    }
}

void
pf_vec3_normalize(pf_vec3_t dst, const pf_vec3_t v)
{
    PF_MATH_FLOAT squaredLength = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
    if (squaredLength == 0.0f) return;

    PF_MATH_FLOAT invLength = pf_rsqrt(squaredLength);

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = v[i] * invLength;
    }
}

void
pf_vec3_normalize_r(float* restrict dst, const pf_vec3_t v)
{
    PF_MATH_FLOAT squaredLength = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
    if (squaredLength == 0.0f) return;

    PF_MATH_FLOAT invLength = pf_rsqrt(squaredLength);

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = v[i] * invLength;
    }
}

PF_MATH_FLOAT
pf_vec3_len(const pf_vec3_t v)
{
    return sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

PF_MATH_FLOAT
pf_vec3_len_sq(const pf_vec3_t v)
{
    return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
}

PF_MATH_FLOAT
pf_vec3_dot(const pf_vec3_t v1, const pf_vec3_t v2)
{
#ifdef _OPENMP
    PF_MATH_FLOAT dotProduct = 0.0f;
#   pragma omp simd
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dotProduct += v1[i]*v2[i];
    }
    return dotProduct;
#else
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
#endif
}

void
pf_vec3_cross(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2)
{
    pf_vec3_t tmp = {
        v1[1]*v2[2] - v1[2]*v2[1],
        v1[2]*v2[0] - v1[0]*v2[2],
        v1[0]*v2[1] - v1[1]*v2[0]
    };

    memcpy(dst, tmp, sizeof(pf_vec3_t));
}

void
pf_vec3_cross_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2)
{
    dst[0] = v1[1]*v2[2] - v1[2]*v2[1];
    dst[1] = v1[2]*v2[0] - v1[0]*v2[2];
    dst[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

PF_MATH_FLOAT
pf_vec3_distance(const pf_vec3_t v1, const pf_vec3_t v2)
{
    // NOTE: Here, the loop version adds a conditional branch, in doubt we keep it simple.

    pf_vec3_t dt = {
        v1[0] - v2[0],
        v1[1] - v2[1],
        v1[2] - v2[2]
    };

    PF_MATH_FLOAT distanceSq = dt[0]*dt[0] +
                           dt[1]*dt[1] +
                           dt[2]*dt[2];

#ifdef PFM_FISR
    return distanceSq*pf_rsqrt(distanceSq);
#else
    return sqrtf(distanceSq);
#endif
}

PF_MATH_FLOAT
pf_vec3_distance_sq(const pf_vec3_t v1, const pf_vec3_t v2)
{
    // NOTE 1: The code generated by GCC 11/13 in O3 utilizes SIMD operations more efficiently than the non-loop version
    // NOTE 2: Still with GCC 13 in O3, the code generated with 'omp simd' is the same as without, but on GCC versions lower than 11.1 the code generated with 'omp simd' retains the loop...

    PF_MATH_FLOAT distanceSq = 0.0f;
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        PF_MATH_FLOAT dt = v1[i] - v2[i];
        distanceSq += dt*dt;
    }
    return distanceSq;
}

void
pf_vec3_direction(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2)
{
    pf_vec3_t tmp;
    PF_MATH_FLOAT lengthSq = 0.0f;

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        tmp[i] = v1[i] - v2[i];
        lengthSq += tmp[i]*tmp[i];
    }

    PF_MATH_FLOAT invLength = pf_rsqrt(lengthSq);

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = tmp[i]*invLength;
    }
}

void
pf_vec3_direction_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2)
{
    PF_MATH_FLOAT lengthSq = 0.0f;

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = v1[i] - v2[i];
        lengthSq += dst[i]*dst[i];
    }

    PF_MATH_FLOAT invLength = pf_rsqrt(lengthSq);

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = dst[i]*invLength;
    }
}

void
pf_vec3_lerp(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2, PF_MATH_FLOAT t)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = v1[i] + t*(v2[i]-v1[i]);
    }
}

void
pf_vec3_lerp_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2, PF_MATH_FLOAT t)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = v1[i] + t*(v2[i]-v1[i]);
    }
}

void
pf_vec3_bary(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2, const pf_vec3_t v3, PF_MATH_FLOAT w1, PF_MATH_FLOAT w2, PF_MATH_FLOAT w3)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = w1*v1[i] + w2*v2[i] + w3*v3[i];
    }
}

void
pf_vec3_bary_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2, const pf_vec3_t v3, PF_MATH_FLOAT w1, PF_MATH_FLOAT w2, PF_MATH_FLOAT w3)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = w1*v1[i] + w2*v2[i] + w3*v3[i];
    }
}

void
pf_vec3_bary_v(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2, const pf_vec3_t v3, const pf_vec3_t w)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = w[0]*v1[i] + w[1]*v2[i] + w[2]*v3[i];
    }
}

void
pf_vec3_bary_v_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2, const pf_vec3_t v3, const pf_vec3_t w)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = w[0]*v1[i] + w[1]*v2[i] + w[2]*v3[i];
    }
}

void
pf_vec3_transform(pf_vec3_t dst, const pf_vec3_t v, const pf_mat4_t mat)
{
    pf_vec3_t tmp = {
        mat[0]*v[0] + mat[4]*v[1] + mat[8]*v[2] + mat[12],
        mat[1]*v[0] + mat[5]*v[1] + mat[9]*v[2] + mat[13],
        mat[2]*v[0] + mat[6]*v[1] + mat[10]*v[2] + mat[14]
    };

    memcpy(dst, tmp, sizeof(pf_vec3_t));
}

void
pf_vec3_transform_r(float* restrict dst, const pf_vec3_t v, const pf_mat4_t mat)
{
    dst[0] = mat[0]*v[0] + mat[4]*v[1] + mat[8]*v[2] + mat[12];
    dst[1] = mat[1]*v[0] + mat[5]*v[1] + mat[9]*v[2] + mat[13];
    dst[2] = mat[2]*v[0] + mat[6]*v[1] + mat[10]*v[2] + mat[14];
}

void
pf_vec3_transform_weighted_translate(pf_vec3_t dst, const pf_vec3_t v, PF_MATH_FLOAT wTranslation, const pf_mat4_t mat)
{
    pf_vec3_t tmp = {
        mat[0]*v[0] + mat[4]*v[1] + mat[8]*v[2] + wTranslation*mat[12],
        mat[1]*v[0] + mat[5]*v[1] + mat[9]*v[2] + wTranslation*mat[13],
        mat[2]*v[0] + mat[6]*v[1] + mat[10]*v[2] + wTranslation*mat[14]
    };

    memcpy(dst, tmp, sizeof(pf_vec3_t));
}

void
pf_vec3_transform_weighted_translate_r(float* restrict dst, const pf_vec3_t v, PF_MATH_FLOAT wTranslation, const pf_mat4_t mat)
{
    dst[0] = mat[0]*v[0] + mat[4]*v[1] + mat[8]*v[2] + wTranslation*mat[12];
    dst[1] = mat[1]*v[0] + mat[5]*v[1] + mat[9]*v[2] + wTranslation*mat[13];
    dst[2] = mat[2]*v[0] + mat[6]*v[1] + mat[10]*v[2] + wTranslation*mat[14];
}

void
pf_vec3_reflect(pf_vec3_t dst, const pf_vec3_t incident, const pf_vec3_t normal)
{
    PF_MATH_FLOAT dotProduct = 0.0f;

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dotProduct += incident[i]*normal[i];
    }

    dotProduct *= 2.0f;

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = incident[i] - dotProduct*normal[i];
    }
}

void
pf_vec3_reflect_r(float* restrict dst, const pf_vec3_t incident, const pf_vec3_t normal)
{
    PF_MATH_FLOAT dotProduct = 0.0f;

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dotProduct += incident[i]*normal[i];
    }

    dotProduct *= 2.0f;

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; ++i)
    {
        dst[i] = incident[i] - dotProduct*normal[i];
    }
}

float
pf_vec3_angle(const pf_vec3_t v1, const pf_vec3_t v2)
{
    pf_vec3_t crossed;
    pf_vec3_cross_r(crossed, v1, v2);
    return atan2(pf_vec3_len(crossed), pf_vec3_dot(v1, v2));
}