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

#ifndef PF_MATH_MAT4_H
#define PF_MATH_MAT4_H

#include "pf_math.h"

#include "../pf_stdinc.h"
#include <string.h>

typedef PF_MATH_FLOAT pf_mat4_t[4*4];

static inline int
pf_mat4_is_zeroes(
    const pf_mat4_t mat)
{
    for (int_fast8_t i = 0; i < 16; ++i) {
        if (mat[i] != 0.0f) return 0;
    }
    return 1;
}

static inline int
pf_mat4_is_identity(
    const pf_mat4_t mat)
{
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

static inline void
pf_mat4_identity(
    pf_mat4_t dst)
{
    memset(dst, 0, sizeof(pf_mat4_t));
    dst[0] = dst[5] = dst[10] = dst[15] = 1;
}

static inline void
pf_mat4_copy(
    PF_MATH_FLOAT* restrict dst,
    const PF_MATH_FLOAT* restrict src)
{
    memcpy(dst, src, sizeof(pf_mat4_t));
}

static inline void
pf_mat4_copy_f(
    PF_MATH_FLOAT* restrict dst,
    const float* restrict src)
{
    for (int_fast8_t i = 0; i < 16; ++i) {
        dst[i] = src[i];
    }
}

static inline void
pf_mat4_copy_d(
    PF_MATH_FLOAT* restrict dst,
    const double* restrict src)
{
    for (int_fast8_t i = 0; i < 16; ++i) {
        dst[i] = src[i];
    }
}

static inline void
pf_mat4_transpose(
    pf_mat4_t dst,
    const pf_mat4_t src)
{
    pf_mat4_t result;
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        for (int_fast8_t j = 0; j < 4; ++j)
        {
            result[i * 4 + j] = src[j * 4 + i];
        }
    }

    memcpy(dst, result, sizeof(pf_mat4_t));
}

static inline void
pf_mat4_transpose_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_mat4_t src)
{
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        for (int_fast8_t j = 0; j < 4; ++j)
        {
            dst[i * 4 + j] = src[j * 4 + i];
        }
    }
}

static inline void
pf_mat4_translate(
    pf_mat4_t dst,
    PF_MATH_FLOAT x, PF_MATH_FLOAT y, PF_MATH_FLOAT z)
{
    memset(dst, 0, sizeof(pf_mat4_t));
    dst[3] = x, dst[7] = y, dst[11] = z;
    dst[0] = dst[5] = dst[10] = dst[15] = 1;
}

static inline void
pf_mat4_rotate_axis_angle(
    pf_mat4_t dst,
    PF_MATH_FLOAT x, PF_MATH_FLOAT y, PF_MATH_FLOAT z,
    PF_MATH_FLOAT angle)
{
    PF_MATH_FLOAT c = cosf(angle);
    PF_MATH_FLOAT s = sinf(angle);
    PF_MATH_FLOAT one_minus_c = 1.0f - c;
    
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

static inline void
pf_mat4_rotate_x(
    pf_mat4_t dst,
    PF_MATH_FLOAT angle)
{
    memset(dst, 0, sizeof(pf_mat4_t));

    PF_MATH_FLOAT c = cosf(angle);
    PF_MATH_FLOAT s = sinf(angle);

    dst[0] = 1;
    dst[5] = c;
    dst[6] = -s;
    dst[9] = s;
    dst[10] = c;
    dst[15] = 1;
}

static inline void
pf_mat4_rotate_y(
    pf_mat4_t dst,
    PF_MATH_FLOAT angle)
{
    memset(dst, 0, sizeof(pf_mat4_t));

    PF_MATH_FLOAT c = cosf(angle);
    PF_MATH_FLOAT s = sinf(angle);

    dst[0] = c;
    dst[2] = s;
    dst[5] = 1;
    dst[8] = -s;
    dst[10] = c;
    dst[15] = 1;
}

static inline void
pf_mat4_rotate_z(
    pf_mat4_t dst,
    PF_MATH_FLOAT angle)
{
    memset(dst, 0, sizeof(pf_mat4_t));

    PF_MATH_FLOAT c = cosf(angle);
    PF_MATH_FLOAT s = sinf(angle);

    dst[0] = c;
    dst[1] = -s;
    dst[4] = s;
    dst[5] = c;
    dst[10] = 1;
    dst[15] = 1;
}

static inline void
pf_mat4_scale(
    pf_mat4_t dst,
    PF_MATH_FLOAT sx, PF_MATH_FLOAT sy, PF_MATH_FLOAT sz)
{
    memset(dst, 0, sizeof(pf_mat4_t));

    dst[0] = sx;
    dst[5] = sy;
    dst[10] = sz;
    dst[15] = 1;
}

static inline void
pf_mat4_mul(
    pf_mat4_t dst,
    const pf_mat4_t left,
    const pf_mat4_t right)
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

static inline void
pf_mat4_mul_r(
    PF_MATH_FLOAT* restrict dst,
    const pf_mat4_t left,
    const pf_mat4_t right)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            dst[i * 4 + j] = 
                left[i * 4 + 0] * right[0 * 4 + j] +
                left[i * 4 + 1] * right[1 * 4 + j] +
                left[i * 4 + 2] * right[2 * 4 + j] +
                left[i * 4 + 3] * right[3 * 4 + j];
        }
    }
}

static inline void
pf_mat4_inverse(
    pf_mat4_t dst,
    const pf_mat4_t src)
{
    // Cache the matrix values (speed optimization)
    PF_MATH_FLOAT a00 = src[0],  a01 = src[1],  a02 = src[2],  a03 = src[3];
    PF_MATH_FLOAT a10 = src[4],  a11 = src[5],  a12 = src[6],  a13 = src[7];
    PF_MATH_FLOAT a20 = src[8],  a21 = src[9],  a22 = src[10], a23 = src[11];
    PF_MATH_FLOAT a30 = src[12], a31 = src[13], a32 = src[14], a33 = src[15];

    PF_MATH_FLOAT b00 = a00*a11 - a01*a10;
    PF_MATH_FLOAT b01 = a00*a12 - a02*a10;
    PF_MATH_FLOAT b02 = a00*a13 - a03*a10;
    PF_MATH_FLOAT b03 = a01*a12 - a02*a11;
    PF_MATH_FLOAT b04 = a01*a13 - a03*a11;
    PF_MATH_FLOAT b05 = a02*a13 - a03*a12;
    PF_MATH_FLOAT b06 = a20*a31 - a21*a30;
    PF_MATH_FLOAT b07 = a20*a32 - a22*a30;
    PF_MATH_FLOAT b08 = a20*a33 - a23*a30;
    PF_MATH_FLOAT b09 = a21*a32 - a22*a31;
    PF_MATH_FLOAT b10 = a21*a33 - a23*a31;
    PF_MATH_FLOAT b11 = a22*a33 - a23*a32;

    // Calculate the invert determinant (inlined to avoid double-caching)
    PF_MATH_FLOAT inv_det = 1.0f/(b00*b11 - b01*b10 + b02*b09 + b03*b08 - b04*b07 + b05*b06);

    dst[0] = (a11*b11 - a12*b10 + a13*b09)*inv_det;
    dst[1] = (-a01*b11 + a02*b10 - a03*b09)*inv_det;
    dst[2] = (a31*b05 - a32*b04 + a33*b03)*inv_det;
    dst[3] = (-a21*b05 + a22*b04 - a23*b03)*inv_det;
    dst[4] = (-a10*b11 + a12*b08 - a13*b07)*inv_det;
    dst[5] = (a00*b11 - a02*b08 + a03*b07)*inv_det;
    dst[6] = (-a30*b05 + a32*b02 - a33*b01)*inv_det;
    dst[7] = (a20*b05 - a22*b02 + a23*b01)*inv_det;
    dst[8] = (a10*b10 - a11*b08 + a13*b06)*inv_det;
    dst[9] = (-a00*b10 + a01*b08 - a03*b06)*inv_det;
    dst[10] = (a30*b04 - a31*b02 + a33*b00)*inv_det;
    dst[11] = (-a20*b04 + a21*b02 - a23*b00)*inv_det;
    dst[12] = (-a10*b09 + a11*b07 - a12*b06)*inv_det;
    dst[13] = (a00*b09 - a01*b07 + a02*b06)*inv_det;
    dst[14] = (-a30*b03 + a31*b01 - a32*b00)*inv_det;
    dst[15] = (a20*b03 - a21*b01 + a22*b00)*inv_det;
}

static inline void
pf_mat4_frustum(
    pf_mat4_t dst,
    PF_MATH_FLOAT left, PF_MATH_FLOAT right,
    PF_MATH_FLOAT bottom, PF_MATH_FLOAT top,
    PF_MATH_FLOAT near_plane, PF_MATH_FLOAT far_plane)
{
    memset(dst, 0, sizeof(pf_mat4_t));

    PF_MATH_FLOAT rl = right - left;
    PF_MATH_FLOAT tb = top - bottom;
    PF_MATH_FLOAT fn = far_plane - near_plane;

    dst[0] = (near_plane*2.0f)/rl;
    dst[5] = (near_plane*2.0f)/tb;

    dst[8] = (right + left)/rl;
    dst[9] = (top + bottom)/tb;
    dst[10] = -(far_plane + near_plane)/fn;
    dst[11] = -1.0f;

    dst[14] = -(far_plane*near_plane*2.0f)/fn;
}

static inline void
pf_mat4_perspective(
    pf_mat4_t dst, PF_MATH_FLOAT fovy, PF_MATH_FLOAT aspect,
    PF_MATH_FLOAT near_plane, PF_MATH_FLOAT far_plane)
{
    memset(dst, 0, sizeof(pf_mat4_t));

    PF_MATH_FLOAT top = near_plane*tan(fovy*0.5);
    PF_MATH_FLOAT bottom = -top;
    PF_MATH_FLOAT right = top*aspect;
    PF_MATH_FLOAT left = -right;

    // pf_mat4_frustum(-right, right, -top, top, near, far);
    PF_MATH_FLOAT rl = right - left;
    PF_MATH_FLOAT tb = top - bottom;
    PF_MATH_FLOAT fn = far_plane - near_plane;

    dst[0] = (near_plane*2.0f)/rl;
    dst[5] = (near_plane*2.0f)/tb;

    dst[8] = (right + left)/rl;
    dst[9] = (top + bottom)/tb;
    dst[10] = -(far_plane + near_plane)/fn;
    dst[11] = -1.0f;

    dst[14] = -(far_plane*near_plane*2.0f)/fn;
}

static inline void
pf_mat4_ortho(
    pf_mat4_t dst,
    PF_MATH_FLOAT left, PF_MATH_FLOAT right,
    PF_MATH_FLOAT bottom, PF_MATH_FLOAT top,
    PF_MATH_FLOAT near_plane, PF_MATH_FLOAT far_plane)
{
    memset(dst, 0, sizeof(pf_mat4_t));

    PF_MATH_FLOAT rl = (right - left);
    PF_MATH_FLOAT tb = (top - bottom);
    PF_MATH_FLOAT fn = (far_plane - near_plane);

    dst[0] = 2.0f/rl;
    dst[5] = 2.0f/tb;

    dst[10] = -2.0f/fn;
    dst[11] = 0.0f;
    dst[12] = -(left + right)/rl;
    dst[13] = -(top + bottom)/tb;

    dst[14] = -(far_plane + near_plane)/fn;
    dst[15] = 1.0f;
}

static inline void
pf_mat4_look_at(
    pf_mat4_t dst,
    const PF_MATH_FLOAT eye[3],
    const PF_MATH_FLOAT target[3],
    const PF_MATH_FLOAT up[3])
{
    memset(dst, 0, sizeof(pf_mat4_t));

    PF_MATH_FLOAT length = 0.0f;
    PF_MATH_FLOAT invLenght = 0.0f;

    // pf_vec3_sub(eye, target)
    PF_MATH_FLOAT vz[3] = {
        eye[0] - target[0],
        eye[1] - target[1],
        eye[2] - target[2]
    };

    // pf_vec3_normalize(vz)
    PF_MATH_FLOAT v[3] = { vz[0], vz[1], vz[2] };
    length = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (length == 0.0f) length = 1.0f;
    invLenght = 1.0f/length;
    vz[0] *= invLenght;
    vz[1] *= invLenght;
    vz[2] *= invLenght;

    // pf_vec3_cross(up, vz)
    PF_MATH_FLOAT vx[3] = {
        up[1]*vz[2] - up[2]*vz[1],
        up[2]*vz[0] - up[0]*vz[2],
        up[0]*vz[1] - up[1]*vz[0]
    };

    // pf_vec3_normalize(x)
    for (int_fast8_t i = 0; i < 3; ++i) v[i] = vx[i];
    length = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (length == 0.0f) length = 1.0f;
    invLenght = 1.0f/length;
    vx[0] *= invLenght;
    vx[1] *= invLenght;
    vx[2] *= invLenght;

    // pf_vec3_cross(vz, vx)
    PF_MATH_FLOAT vy[3] = {
        vz[1]*vx[2] - vz[2]*vx[1],
        vz[2]*vx[0] - vz[0]*vx[2],
        vz[0]*vx[1] - vz[1]*vx[0]
    };

    dst[0] = vx[0];
    dst[1] = vy[0];
    dst[2] = vz[0];

    dst[4] = vx[1];
    dst[5] = vy[1];
    dst[6] = vz[1];

    dst[8] = vx[2];
    dst[9] = vy[2];
    dst[10] = vz[2];

    dst[12] = -(vx[0]*eye[0] + vx[1]*eye[1] + vx[2]*eye[2]);   // pf_vec3_dot(vx, eye)
    dst[13] = -(vy[0]*eye[0] + vy[1]*eye[1] + vy[2]*eye[2]);   // pf_vec3_dot(vy, eye)
    dst[14] = -(vz[0]*eye[0] + vz[1]*eye[1] + vz[2]*eye[2]);   // pf_vec3_dot(vz, eye)
    dst[15] = 1.0f;
}

#endif //PF_MATH_MAT4_H
