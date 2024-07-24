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

#ifndef PF_MATH_MAT3_H
#define PF_MATH_MAT3_H

typedef float pf_mat3_t[3*3];

int
pf_mat3_is_zeroes(
    const pf_mat3_t mat);

int
pf_mat3_is_identity(
    const pf_mat3_t mat);

void
pf_mat3_identity(
    pf_mat3_t dst);

void
pf_mat3_copy(
    float* restrict dst,
    const float* restrict src);

void
pf_mat3_translate(
    pf_mat3_t dst, float x, float y);

void
pf_mat3_rotate(
    pf_mat3_t dst, float angle);

void
pf_mat3_scale(
    pf_mat3_t dst, float sx, float sy);

void
pf_mat3_mul(
    pf_mat3_t dst,
    const pf_mat3_t left,
    const pf_mat3_t right);

void
pf_mat3_mul_r(
    float* restrict dst,
    const pf_mat3_t left,
    const pf_mat3_t right);

void
pf_mat3_inverse(
    pf_mat3_t dst,
    const pf_mat3_t src);

#endif //PF_MATH_MAT3_H
