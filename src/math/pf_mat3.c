#include "pixelfactory/math/pf_mat3.h"
#include "pixelfactory/pf_stdinc.h"
#include <string.h>
#include <math.h>

int
pf_mat3_is_zeroes(const pf_mat3_t mat) {
    for (int_fast8_t i = 0; i < 9; ++i) {
        if (mat[i] != 0.0f) return 0;
    }
    return 1;
}

int
pf_mat3_is_identity(const pf_mat3_t mat) {
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

void
pf_mat3_identity(pf_mat3_t dst) {
    memset(dst, 0, sizeof(pf_mat3_t));
    dst[0] = 1.0f;
    dst[4] = 1.0f;
    dst[8] = 1.0f;
}

void
pf_mat3_copy(float* restrict dst, const float* restrict src)
{
    memcpy(dst, src, sizeof(pf_mat3_t));
}

void
pf_mat3_translate(pf_mat3_t dst, float x, float y) {
    pf_mat3_identity(dst);
    dst[2] = x;
    dst[5] = y;
}

void
pf_mat3_rotate(pf_mat3_t dst, float angle) {
    float c = cosf(angle);
    float s = sinf(angle);

    pf_mat3_identity(dst);
    dst[0] = c;
    dst[1] = -s;
    dst[3] = s;
    dst[4] = c;
}

void
pf_mat3_scale(pf_mat3_t dst, float sx, float sy) {
    pf_mat3_identity(dst);
    dst[0] = sx;
    dst[4] = sy;
}

void
pf_mat3_mul(pf_mat3_t dst, const pf_mat3_t left, const pf_mat3_t right) {
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

void
pf_mat3_mul_r(float* restrict dst, const pf_mat3_t left, const pf_mat3_t right) {
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            dst[row * 3 + col] = 
                left[row * 3 + 0] * right[0 * 3 + col] + 
                left[row * 3 + 1] * right[1 * 3 + col] + 
                left[row * 3 + 2] * right[2 * 3 + col];
        }
    }
}

void
pf_mat3_inverse(pf_mat3_t dst, const pf_mat3_t src) {
    float det = src[0] * (src[4] * src[8] - src[5] * src[7]) -
                src[1] * (src[3] * src[8] - src[5] * src[6]) +
                src[2] * (src[3] * src[7] - src[4] * src[6]);

    if (fabs(det) < 1e-6) {
        pf_mat3_identity(dst); // Non-invertible matrix case
        return;
    }

    float inv_det = 1.0f / det;

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
