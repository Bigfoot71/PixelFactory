#ifndef PF_MATH_VEC3_H
#define PF_MATH_VEC3_H

#include "pf_math.h"
#include "pf_mat4.h"

typedef float pf_vec3_t[3];

void
pf_vec3_zero(pf_vec3_t dst);

void
pf_vec3_one(pf_vec3_t dst, PF_MATH_FLOAT v);

void
pf_vec3_set(pf_vec3_t dst, PF_MATH_FLOAT x, PF_MATH_FLOAT y, PF_MATH_FLOAT z);

void
pf_vec3_copy(float* restrict dst, const float* restrict src);

void
pf_vec3_swap(float* restrict a, float* restrict b);

void
pf_vec3_neg(pf_vec3_t dst, const pf_vec3_t v);

void
pf_vec3_neg_r(float* restrict dst, const pf_vec3_t v);

void
pf_vec3_add(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2);

void
pf_vec3_add_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2);

void
pf_vec3_sub(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2);

void
pf_vec3_sub_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2);

void
pf_vec3_mul(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2);

void
pf_vec3_mul_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2);

void
pf_vec3_div(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2);

void
pf_vec3_div_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2);

void
pf_vec3_offset(pf_vec3_t dst, const pf_vec3_t v, PF_MATH_FLOAT scalar);

void
pf_vec3_offset_r(float* restrict dst, const pf_vec3_t v, PF_MATH_FLOAT scalar);

void
pf_vec3_scale(pf_vec3_t dst, const pf_vec3_t v, PF_MATH_FLOAT scalar);

void
pf_vec3_scale_r(float* restrict dst, const pf_vec3_t v, PF_MATH_FLOAT scalar);

void
pf_vec3_normalize(pf_vec3_t dst, const pf_vec3_t v);

void
pf_vec3_normalize_r(float* restrict dst, const pf_vec3_t v);

PF_MATH_FLOAT
pf_vec3_len(const pf_vec3_t v);

PF_MATH_FLOAT
pf_vec3_len_sq(const pf_vec3_t v);

PF_MATH_FLOAT
pf_vec3_dot(const pf_vec3_t v1, const pf_vec3_t v2);

void
pf_vec3_cross(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2);

void
pf_vec3_cross_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2);

PF_MATH_FLOAT
pf_vec3_distance(const pf_vec3_t v1, const pf_vec3_t v2);

PF_MATH_FLOAT
pf_vec3_distance_sq(const pf_vec3_t v1, const pf_vec3_t v2);

void
pf_vec3_direction(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2);

void
pf_vec3_direction_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2);

void
pf_vec3_lerp(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2, PF_MATH_FLOAT t);

void
pf_vec3_lerp_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2, PF_MATH_FLOAT t);

void
pf_vec3_bary(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2, const pf_vec3_t v3, PF_MATH_FLOAT w1, PF_MATH_FLOAT w2, PF_MATH_FLOAT w3);

void
pf_vec3_bary_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2, const pf_vec3_t v3, PF_MATH_FLOAT w1, PF_MATH_FLOAT w2, PF_MATH_FLOAT w3);

void
pf_vec3_bary_v(pf_vec3_t dst, const pf_vec3_t v1, const pf_vec3_t v2, const pf_vec3_t v3, const pf_vec3_t w);

void
pf_vec3_bary_v_r(float* restrict dst, const pf_vec3_t v1, const pf_vec3_t v2, const pf_vec3_t v3, const pf_vec3_t w);

void
pf_vec3_transform(pf_vec3_t dst, const pf_vec3_t v, const pf_mat4_t mat);

void
pf_vec3_transform_r(float* restrict dst, const pf_vec3_t v, const pf_mat4_t mat);

void
pf_vec3_transform_weighted_translate(pf_vec3_t dst, const pf_vec3_t v, PF_MATH_FLOAT wTranslation, const pf_mat4_t mat);

void
pf_vec3_transform_weighted_translate_r(float* restrict dst, const pf_vec3_t v, PF_MATH_FLOAT wTranslation, const pf_mat4_t mat);

void
pf_vec3_reflect(pf_vec3_t dst, const pf_vec3_t incident, const pf_vec3_t normal);

void
pf_vec3_reflect_r(float* restrict dst, const pf_vec3_t incident, const pf_vec3_t normal);

#endif //PF_MATH_VEC3_H
