#ifndef PF_MATH_MAT3_H
#define PF_MATH_MAT3_H

typedef float pf_mat3_t[3*3];

int
pf_mat3_is_zeroes(const pf_mat3_t mat);

int
pf_mat3_is_identity(const pf_mat3_t mat);

void
pf_mat3_identity(pf_mat3_t dst);

void
pf_mat3_translate(pf_mat3_t dst, float x, float y);

void
pf_mat3_rotate(pf_mat3_t dst, float angle);

void
pf_mat3_scale(pf_mat3_t dst, float sx, float sy);

void
pf_mat3_mul(pf_mat3_t dst, const pf_mat3_t left, const pf_mat3_t right);

void
pf_mat3_mul_r(float* restrict dst, const pf_mat3_t left, const pf_mat3_t right);

void
pf_mat3_inverse(pf_mat3_t src, const pf_mat3_t dst);

#endif //PF_MATH_MAT3_H
