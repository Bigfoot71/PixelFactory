#ifndef PF_MATH_VEC2_H
#define PF_MATH_VEC2_H

#include "pf_math.h"
#include "pf_vec3.h"
#include "pf_mat3.h"
#include "pf_mat4.h"

typedef float pf_vec2_t[2];

void
pf_vec2_zero(pf_vec2_t dst);

void
pf_vec2_one(pf_vec2_t dst, PF_MATH_FLOAT v);

void
pf_vec2_set(pf_vec2_t dst, PF_MATH_FLOAT x, PF_MATH_FLOAT y);

void
pf_vec2_copy(float* restrict dst, const float* restrict src);

void
pf_vec2_swap(float* restrict a, float* restrict b);

void
pf_vec2_neg(pf_vec2_t dst, const pf_vec2_t v);

void
pf_vec2_neg_r(float* restrict dst, const pf_vec2_t v);

void
pf_vec2_add(pf_vec2_t dst, const pf_vec2_t v1, const pf_vec2_t v2);

void
pf_vec2_add_r(float* restrict dst, const pf_vec2_t v1, const pf_vec2_t v2);

void
pf_vec2_sub(pf_vec2_t dst, const pf_vec2_t v1, const pf_vec2_t v2);

void
pf_vec2_sub_r(float* restrict dst, const pf_vec2_t v1, const pf_vec2_t v2);

void
pf_vec2_mul(pf_vec2_t dst, const pf_vec2_t v1, const pf_vec2_t v2);

void
pf_vec2_mul_r(float* restrict dst, const pf_vec2_t v1, const pf_vec2_t v2);

void
pf_vec2_div(pf_vec2_t dst, const pf_vec2_t v1, const pf_vec2_t v2);

void
pf_vec2_div_r(float* restrict dst, const pf_vec2_t v1, const pf_vec2_t v2);

void
pf_vec2_offset(pf_vec2_t dst, const pf_vec2_t v, PF_MATH_FLOAT scalar);

void
pf_vec2_offset_r(float* restrict dst, const pf_vec2_t v, PF_MATH_FLOAT scalar);

void
pf_vec2_scale(pf_vec2_t dst, const pf_vec2_t v, PF_MATH_FLOAT scalar);

void
pf_vec2_scale_r(float* restrict dst, const pf_vec2_t v, PF_MATH_FLOAT scalar);

void
pf_vec2_normalize(pf_vec2_t dst, const pf_vec2_t v);

void
pf_vec2_normalize_r(float* restrict dst, const pf_vec2_t v);

PF_MATH_FLOAT
pf_vec2_len(const pf_vec2_t v);

PF_MATH_FLOAT
pf_vec2_len_sq(const pf_vec2_t v);

PF_MATH_FLOAT
pf_vec2_dot(const pf_vec2_t v1, const pf_vec2_t v2);

PF_MATH_FLOAT
pf_vec2_distance(const pf_vec2_t v1, const pf_vec2_t v2);

PF_MATH_FLOAT
pf_vec2_distance_sq(const pf_vec2_t v1, const pf_vec2_t v2);

void
pf_vec2_direction(pf_vec2_t dst, const pf_vec2_t v1, const pf_vec2_t v2);

void
pf_vec2_direction_r(float* restrict dst, const pf_vec2_t v1, const pf_vec2_t v2);

void
pf_vec2_lerp(pf_vec2_t dst, const pf_vec2_t v1, const pf_vec2_t v2, PF_MATH_FLOAT t);

void
pf_vec2_lerp_r(float* restrict dst, const pf_vec2_t v1, const pf_vec2_t v2, PF_MATH_FLOAT t);

void
pf_vec2_bary(pf_vec2_t dst, const pf_vec2_t v1, const pf_vec2_t v2, const pf_vec2_t v3, PF_MATH_FLOAT w1, PF_MATH_FLOAT w2, PF_MATH_FLOAT w3);

void
pf_vec2_bary_r(float* restrict dst, const pf_vec2_t v1, const pf_vec2_t v2, const pf_vec2_t v3, PF_MATH_FLOAT w1, PF_MATH_FLOAT w2, PF_MATH_FLOAT w3);

void
pf_vec2_bary_v(pf_vec2_t dst, const pf_vec2_t v1, const pf_vec2_t v2, const pf_vec2_t v3, const pf_vec3_t w);

void
pf_vec2_bary_v_r(float* restrict dst, const pf_vec2_t v1, const pf_vec2_t v2, const pf_vec2_t v3, const pf_vec3_t w);

void
pf_vec2_transform(pf_vec2_t dst, const pf_vec2_t v, const pf_mat3_t mat);

void
pf_vec2_transform_r(float* restrict dst, const pf_vec2_t v, const pf_mat3_t mat);

void
pf_vec2_transform_mat4(pf_vec2_t dst, const pf_vec2_t v, const pf_mat4_t mat);

void
pf_vec2_transform_mat4_r(float* restrict dst, const pf_vec2_t v, const pf_mat4_t mat);

void
pfm_vec2_transform_mat4_weighted_translation(pf_vec2_t dst, const pf_vec2_t v, PF_MATH_FLOAT weight, const pf_mat4_t mat);

void
pfm_vec2_transform_mat4_weighted_translation_r(float* restrict dst, const pf_vec2_t v, PF_MATH_FLOAT weight, const pf_mat4_t mat);

void
pf_vec2_transform_i(int* restrict x_dst, int* restrict y_dst, int x_src, int y_src, const pf_mat3_t mat);

#endif //PF_MATH_VEC2
