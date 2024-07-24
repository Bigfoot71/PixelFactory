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

typedef float pf_mat4_t[4*4];

int
pf_mat4_is_zeroes(
    const pf_mat4_t mat);

int
pf_mat4_is_identity(
    const pf_mat4_t mat);

void
pf_mat4_identity(
    pf_mat4_t dst);

void
pf_mat4_copy(
    float* restrict dst,
    const float* restrict src);

void
pf_mat4_transpose(
    pf_mat4_t dst,
    const pf_mat4_t src);

void
pf_mat4_transpose_r(
    float* restrict dst,
    const pf_mat4_t src);

void
pf_mat4_translate(
    pf_mat4_t dst,
    float x, float y, float z);

void
pf_mat4_rotate_axis_angle(
    pf_mat4_t dst,
    float x, float y, float z,
    float angle);

void
pf_mat4_rotate_x(
    pf_mat4_t dst,
    float angle);

void
pf_mat4_rotate_y(
    pf_mat4_t dst,
    float angle);

void
pf_mat4_rotate_z(
    pf_mat4_t dst,
    float angle);

void
pf_mat4_scale(
    pf_mat4_t dst,
    float sx, float sy, float sz);

void
pf_mat4_mul(
    pf_mat4_t dst,
    const pf_mat4_t left,
    const pf_mat4_t right);

void
pf_mat4_mul_r(
    float* restrict dst,
    const pf_mat4_t left,
    const pf_mat4_t right);

void
pf_mat4_inverse(
    pf_mat4_t dst,
    const pf_mat4_t src);

void
pf_mat4_frustum(
    pf_mat4_t dst,
    PF_MATH_FLOAT left, PF_MATH_FLOAT right,
    PF_MATH_FLOAT bottom, PF_MATH_FLOAT top,
    PF_MATH_FLOAT near_plane, PF_MATH_FLOAT far_plane);

void
pf_mat4_perspective(
    pf_mat4_t dst, PF_MATH_FLOAT fovy, PF_MATH_FLOAT aspect,
    PF_MATH_FLOAT near_plane, PF_MATH_FLOAT far_plane);

void
pf_mat4_ortho(
    pf_mat4_t dst,
    PF_MATH_FLOAT left, PF_MATH_FLOAT right,
    PF_MATH_FLOAT bottom, PF_MATH_FLOAT top,
    PF_MATH_FLOAT near_plane, PF_MATH_FLOAT far_plane);

void
pf_mat4_look_at(
    pf_mat4_t dst,
    const float eye[3],
    const float target[3],
    const float up[3]);

#endif //PF_MATH_MAT4_H
