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
pf_mat3_inverse(pf_mat3_t inv, const pf_mat3_t mat) {
    float det = mat[0] * (mat[4] * mat[8] - mat[5] * mat[7]) -
                mat[1] * (mat[3] * mat[8] - mat[5] * mat[6]) +
                mat[2] * (mat[3] * mat[7] - mat[4] * mat[6]);

    if (fabs(det) < 1e-6) {
        pf_mat3_identity(inv); // Non-invertible matrix case
        return;
    }

    float invDet = 1.0f / det;

    inv[0] = (mat[4] * mat[8] - mat[5] * mat[7]) * invDet;
    inv[1] = (mat[2] * mat[7] - mat[1] * mat[8]) * invDet;
    inv[2] = (mat[1] * mat[5] - mat[2] * mat[4]) * invDet;
    inv[3] = (mat[5] * mat[6] - mat[3] * mat[8]) * invDet;
    inv[4] = (mat[0] * mat[8] - mat[2] * mat[6]) * invDet;
    inv[5] = (mat[2] * mat[3] - mat[0] * mat[5]) * invDet;
    inv[6] = (mat[3] * mat[7] - mat[4] * mat[6]) * invDet;
    inv[7] = (mat[1] * mat[6] - mat[0] * mat[7]) * invDet;
    inv[8] = (mat[0] * mat[4] - mat[1] * mat[3]) * invDet;
}
