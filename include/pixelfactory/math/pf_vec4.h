#ifndef PF_MATH_VEC4_H
#define PF_MATH_VEC4_H

#include "pf_math.h"
#include "pf_vec3.h"
#include "pf_mat4.h"

typedef float pf_vec4_t[4];

void
pf_vec4_zero(pf_vec4_t dst);

void
pf_vec4_one(pf_vec4_t dst, PF_MATH_FLOAT v);

void
pf_vec4_set(pf_vec4_t dst, PF_MATH_FLOAT x, PF_MATH_FLOAT y, PF_MATH_FLOAT z, PF_MATH_FLOAT w);

void
pf_vec4_copy(float* restrict dst, const float* restrict src);

void
pf_vec4_swap(float* restrict a, float* restrict b);

void
pf_vec4_neg(pf_vec4_t dst, const pf_vec4_t v);

void
pf_vec4_neg_r(float* restrict dst, const pf_vec4_t v);

void
pf_vec4_add(pf_vec4_t dst, const pf_vec4_t v1, const pf_vec4_t v2);

void
pf_vec4_add_r(float* restrict dst, const pf_vec4_t v1, const pf_vec4_t v2);

void
pf_vec4_sub(pf_vec4_t dst, const pf_vec4_t v1, const pf_vec4_t v2);

void
pf_vec4_sub_r(float* restrict dst, const pf_vec4_t v1, const pf_vec4_t v2);

void
pf_vec4_mul(pf_vec4_t dst, const pf_vec4_t v1, const pf_vec4_t v2);

void
pf_vec4_mul_r(float* restrict dst, const pf_vec4_t v1, const pf_vec4_t v2);

void
pf_vec4_div(pf_vec4_t dst, const pf_vec4_t v1, const pf_vec4_t v2);

void
pf_vec4_div_r(float* restrict dst, const pf_vec4_t v1, const pf_vec4_t v2);

void
pf_vec4_offset(pf_vec4_t dst, const pf_vec4_t v, PF_MATH_FLOAT scalar);

void
pf_vec4_offset_r(float* restrict dst, const pf_vec4_t v, PF_MATH_FLOAT scalar);

void
pf_vec4_scale(pf_vec4_t dst, const pf_vec4_t v, PF_MATH_FLOAT scalar);

void
pf_vec4_scale_r(float* restrict dst, const pf_vec4_t v, PF_MATH_FLOAT scalar);

void
pf_vec4_normalize(pf_vec4_t dst, const pf_vec4_t v);

void
pf_vec4_normalize_r(float* restrict dst, const pf_vec4_t v);

PF_MATH_FLOAT
pf_vec4_len(const pf_vec4_t v);

PF_MATH_FLOAT
pf_vec4_len_sq(const pf_vec4_t v);

PF_MATH_FLOAT
pf_vec4_dot(const pf_vec4_t v1, const pf_vec4_t v2);

void
pf_vec4_lerp(pf_vec4_t dst, const pf_vec4_t v1, const pf_vec4_t v2, PF_MATH_FLOAT t);

void
pf_vec4_lerp_r(float* restrict dst, const pf_vec4_t v1, const pf_vec4_t v2, PF_MATH_FLOAT t);

void
pf_vec4_bary(pf_vec4_t dst, const pf_vec4_t v1, const pf_vec4_t v2, const pf_vec4_t v3, PF_MATH_FLOAT w1, PF_MATH_FLOAT w2, PF_MATH_FLOAT w3);

void
pf_vec4_bary_r(float* restrict dst, const pf_vec4_t v1, const pf_vec4_t v2, const pf_vec4_t v3, PF_MATH_FLOAT w1, PF_MATH_FLOAT w2, PF_MATH_FLOAT w3);

void
pf_vec4_bary_v(pf_vec4_t dst, const pf_vec4_t v1, const pf_vec4_t v2, const pf_vec4_t v3, const pf_vec3_t w);

void
pf_vec4_bary_v_r(float* restrict dst, const pf_vec4_t v1, const pf_vec4_t v2, const pf_vec4_t v3, const pf_vec3_t w);

void
pf_vec4_transform(pf_vec4_t dst, const pf_vec4_t v, const pf_mat4_t mat);

void
pf_vec4_transform_r(float* restrict dst, const pf_vec4_t v, const pf_mat4_t mat);

#endif //PF_MATH_VEC4_H
