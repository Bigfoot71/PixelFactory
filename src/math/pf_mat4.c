#include "pixelfactory/math/pf_mat4.h"
#include "pixelfactory/pf_stdinc.h"
#include <string.h>
#include <math.h>

int
pf_mat4_is_zeroes(const pf_mat4_t mat) {
    for (int_fast8_t i = 0; i < 16; ++i) {
        if (mat[i] != 0.0f) return 0;
    }
    return 1;
}

int
pf_mat4_is_identity(const pf_mat4_t mat) {
    for (int_fast8_t i = 0; i < 16; ++i) {
        if (i % 5 == 0) {
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

void pf_mat4_identity(pf_mat4_t dst)
{
    memset(dst, 0, sizeof(pf_mat4_t));
    dst[0] = dst[5] = dst[10] = dst[15] = 1;
}

void pf_mat4_translate(pf_mat4_t dst, float x, float y, float z)
{
    memset(dst, 0, sizeof(pf_mat4_t));
    dst[3] = x, dst[7] = y, dst[11] = z;
    dst[0] = dst[5] = dst[10] = dst[15] = 1;
}

void pf_mat4_rotate_axis_angle(pf_mat4_t dst, float x, float y, float z, float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);
    float one_minus_c = 1.0f - c;
    
    dst[0] = c + x * x * one_minus_c;
    dst[1] = x * y * one_minus_c - z * s;
    dst[2] = x * z * one_minus_c + y * s;
    dst[3] = 0;

    dst[4] = y * x * one_minus_c + z * s,
    dst[5] = c + y * y * one_minus_c,
    dst[6] = y * z * one_minus_c - x * s,
    dst[7] = 0,

    dst[8] = z * x * one_minus_c - y * s,
    dst[9] = z * y * one_minus_c + x * s,
    dst[10] = c + z * z * one_minus_c,
    dst[11] = 0,

    dst[12] = 0;
    dst[13] = 0;
    dst[14] = 0;
    dst[15] = 1;
}

void pf_mat4_rotate_x(pf_mat4_t dst, float angle)
{
    memset(dst, 0, sizeof(pf_mat4_t));

    float c = cosf(angle);
    float s = sinf(angle);

    dst[0] = 1;
    dst[5] = c;
    dst[6] = -s;
    dst[9] = s;
    dst[10] = c;
    dst[15] = 1;
}

void pf_mat4_rotate_y(pf_mat4_t dst, float angle)
{
    memset(dst, 0, sizeof(pf_mat4_t));

    float c = cosf(angle);
    float s = sinf(angle);

    dst[0] = c;
    dst[2] = s;
    dst[5] = 1;
    dst[8] = -s;
    dst[10] = c;
    dst[15] = 1;
}

void pf_mat4_rotate_z(pf_mat4_t dst, float angle)
{
    memset(dst, 0, sizeof(pf_mat4_t));

    float c = cosf(angle);
    float s = sinf(angle);

    dst[0] = c;
    dst[1] = -s;
    dst[4] = s;
    dst[5] = c;
    dst[10] = 1;
    dst[15] = 1;
}

void pf_mat4_scale(pf_mat4_t dst, float sx, float sy, float sz)
{
    memset(dst, 0, sizeof(pf_mat4_t));

    dst[0] = sx;
    dst[5] = sy;
    dst[10] = sz;
    dst[15] = 1;
}

void pf_mat4_mul(pf_mat4_t dst, const pf_mat4_t left, const pf_mat4_t right)
{
    pf_mat4_t result;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i * 4 + j] = 
                left[i * 4 + 0] * right[0 * 4 + j] +
                left[i * 4 + 1] * right[1 * 4 + j] +
                left[i * 4 + 2] * right[2 * 4 + j] +
                left[i * 4 + 3] * right[3 * 4 + j];
        }
    }

    memcpy(dst, result, sizeof(pf_mat4_t));
}
