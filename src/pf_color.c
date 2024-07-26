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

#include "pixelfactory/pf_color.h"
#include "pixelfactory/pf_stdinc.h"
#include <stdint.h>

/* General Functions */

pf_color_t
pf_color_lerpf(
    pf_color_t a,
    pf_color_t b,
    float t)
{
    pf_color_t result;
    result.c.r = a.c.r + t * (b.c.r - a.c.r);
    result.c.g = a.c.g + t * (b.c.g - a.c.g);
    result.c.b = a.c.b + t * (b.c.b - a.c.b);
    result.c.a = a.c.a + t * (b.c.a - a.c.a);
    return result;
}

pf_color_t
pf_color_lerpi(
    pf_color_t a,
    pf_color_t b,
    int i, int max)
{
    pf_color_t result;
    result.c.r = (a.c.r * (max - i) + b.c.r * i) / max;
    result.c.g = (a.c.g * (max - i) + b.c.g * i) / max;
    result.c.b = (a.c.b * (max - i) + b.c.b * i) / max;
    result.c.a = (a.c.a * (max - i) + b.c.a * i) / max;
    return result;
}

pf_color_t
pf_color_bary(
    pf_color_t c1,
    pf_color_t c2,
    pf_color_t c3,
    float w1,
    float w2,
    float w3)
{
    uint8_t uw1 = 255 * w1;
    uint8_t uw2 = 255 * w2;
    uint8_t uw3 = 255 * w3;

    pf_color_t result;
    result.c.r = ((uw1 * c1.c.r) + (uw2 * c2.c.r) + (uw3 * c3.c.r)) / 255;
    result.c.g = ((uw1 * c1.c.g) + (uw2 * c2.c.g) + (uw3 * c3.c.g)) / 255;
    result.c.b = ((uw1 * c1.c.b) + (uw2 * c2.c.b) + (uw3 * c3.c.b)) / 255;
    result.c.a = ((uw1 * c1.c.a) + (uw2 * c2.c.a) + (uw3 * c3.c.a)) / 255;

    return result;
}

pf_color_t
pf_color_bary_v(
    pf_color_t c1,
    pf_color_t c2,
    pf_color_t c3,
    const pf_vec3_t bary)
{
    uint8_t uw1 = 255 * bary[0];
    uint8_t uw2 = 255 * bary[1];
    uint8_t uw3 = 255 * bary[2];

    pf_color_t result;
    result.c.r = ((uw1 * c1.c.r) + (uw2 * c2.c.r) + (uw3 * c3.c.r)) / 255;
    result.c.g = ((uw1 * c1.c.g) + (uw2 * c2.c.g) + (uw3 * c3.c.g)) / 255;
    result.c.b = ((uw1 * c1.c.b) + (uw2 * c2.c.b) + (uw3 * c3.c.b)) / 255;
    result.c.a = ((uw1 * c1.c.a) + (uw2 * c2.c.a) + (uw3 * c3.c.a)) / 255;

    return result;
}

pf_simd_i_t
pf_color_bary_simd(
    pf_simd_i_t c1_r, pf_simd_i_t c1_g, pf_simd_i_t c1_b, pf_simd_i_t c1_a,
    pf_simd_i_t c2_r, pf_simd_i_t c2_g, pf_simd_i_t c2_b, pf_simd_i_t c2_a,
    pf_simd_i_t c3_r, pf_simd_i_t c3_g, pf_simd_i_t c3_b, pf_simd_i_t c3_a,
    pf_simd_t w1, pf_simd_t w2, pf_simd_t w3)
{
    // Multiply weights by 255 to convert them to integers
    pf_simd_t scale = pf_simd_set1_ps(255.0f);
    pf_simd_t uw1_vec = pf_simd_mul_ps(w1, scale);
    pf_simd_t uw2_vec = pf_simd_mul_ps(w2, scale);
    pf_simd_t uw3_vec = pf_simd_mul_ps(w3, scale);

    // Convert weights to integers
    pf_simd_i_t uw1_vec_int = pf_simd_cvtf32_i32(uw1_vec);
    pf_simd_i_t uw2_vec_int = pf_simd_cvtf32_i32(uw2_vec);
    pf_simd_i_t uw3_vec_int = pf_simd_cvtf32_i32(uw3_vec);

    // Perform multiplications and additions for each channel
    pf_simd_i_t sum_r = pf_simd_add_i32(
        pf_simd_add_i32(pf_simd_mullo_i32(uw1_vec_int, c1_r), 
                         pf_simd_mullo_i32(uw2_vec_int, c2_r)),
        pf_simd_mullo_i32(uw3_vec_int, c3_r));

    pf_simd_i_t sum_g = pf_simd_add_i32(
        pf_simd_add_i32(pf_simd_mullo_i32(uw1_vec_int, c1_g), 
                         pf_simd_mullo_i32(uw2_vec_int, c2_g)),
        pf_simd_mullo_i32(uw3_vec_int, c3_g));

    pf_simd_i_t sum_b = pf_simd_add_i32(
        pf_simd_add_i32(pf_simd_mullo_i32(uw1_vec_int, c1_b), 
                         pf_simd_mullo_i32(uw2_vec_int, c2_b)),
        pf_simd_mullo_i32(uw3_vec_int, c3_b));

    pf_simd_i_t sum_a = pf_simd_add_i32(
        pf_simd_add_i32(pf_simd_mullo_i32(uw1_vec_int, c1_a), 
                         pf_simd_mullo_i32(uw2_vec_int, c2_a)),
        pf_simd_mullo_i32(uw3_vec_int, c3_a));

    // Approximate division by 255
    pf_simd_i_t factor = pf_simd_set1_i32(257);
    sum_r = pf_simd_srli_i32(pf_simd_mullo_i32(sum_r, factor), 16);
    sum_g = pf_simd_srli_i32(pf_simd_mullo_i32(sum_g, factor), 16);
    sum_b = pf_simd_srli_i32(pf_simd_mullo_i32(sum_b, factor), 16);
    sum_a = pf_simd_srli_i32(pf_simd_mullo_i32(sum_a, factor), 16);

    // Combine results into a single vector pf_simd_i_t
    pf_simd_i_t result = pf_simd_or_i32(
        pf_simd_or_i32(
            pf_simd_slli_i32(sum_a, 24), 
            pf_simd_slli_i32(sum_b, 16)),
        pf_simd_or_i32(
            pf_simd_slli_i32(sum_g, 8), 
            sum_r));

    return result;
}

pf_color_t
pf_color_scale(
    pf_color_t col,
    float scale)
{
    int s = scale * 255;
    col.c.r = (col.c.r * s) / 255;
    col.c.g = (col.c.g * s) / 255;
    col.c.b = (col.c.b * s) / 255;
    col.c.a = (col.c.a * s) / 255;
    return col;
}

pf_color_t
pf_color_scale_u8(
    pf_color_t col,
    uint8_t scale)
{
    col.c.r = (col.c.r * scale) / 255;
    col.c.g = (col.c.g * scale) / 255;
    col.c.b = (col.c.b * scale) / 255;
    col.c.a = (col.c.a * scale) / 255;
    return col;
}

void
pf_color_from_hsv(
    pf_color_t* color,
    float h, float s, float v)
{
    color->c.r = 0;
    color->c.g = 0;
    color->c.b = 0;
    color->c.a = 255;

    float k = fmodf((5.0f + h/60.0f), 6);
    float t = 4.0f - k;
    k = (t < k)? t : k;
    k = (k < 1)? k : 1;
    k = (k > 0)? k : 0;
    color->c.r = (unsigned char)((v - v * s * k) * 255);

    k = fmodf((3.0f + h/60.0f), 6);
    t = 4.0f - k;
    k = (t < k)? t : k;
    k = (k < 1)? k : 1;
    k = (k > 0)? k : 0;
    color->c.g = (unsigned char)((v - v * s * k)*255);

    k = fmodf((1.0f + h/60.0f), 6);
    t = 4.0f - k;
    k = (t < k)? t : k;
    k = (k < 1)? k : 1;
    k = (k > 0)? k : 0;
    color->c.b = (unsigned char)((v - v * s * k)*255);
}

void
pf_color_to_hsv(
    pf_color_t color,
    float* h, float* s, float* v)
{
    if (h == NULL && s == NULL && v == NULL) return;

    float r = color.c.r/255.0f;
    float g = color.c.g/255.0f;
    float b = color.c.b/255.0f;

    float min = (r < g) ? r : g;
    min = (min < b) ? min : b;

    float max = (r > g) ? r : g;
    max = (max > b) ? max : b;

    if (v) *v = max;
    if (h == NULL && s == NULL) return;

    float delta = max - min;

    if (delta < 0.00001f) {
        *s = 0.0f;
        *h = 0.0f;
        return;
    }

    if (max > 0.0f) {
        *s = (delta/max);
    } else {
        *s = 0.0f;
        *h = NAN;
        return;
    }

    if (r >= max) {
        *h = (g - b)/delta;
    } else {
        if (g >= max) {
            *h = 2.0f + (b - r)/delta;
        } else {
            *h = 4.0f + (r - g)/delta;
        }
    }

    *h *= 60.0f;
    if (*h < 0.0f) {
        *h += 360.0f;
    }
}

/* Blending Functions */

pf_color_t
pf_color_blend_avg(
    pf_color_t dst,
    pf_color_t src)
{
    pf_color_t result;
    result.c.r = (uint8_t)((src.c.r + dst.c.r) >> 1);
    result.c.g = (uint8_t)((src.c.g + dst.c.g) >> 1);
    result.c.b = (uint8_t)((src.c.b + dst.c.b) >> 1);
    result.c.a = (uint8_t)((src.c.a + dst.c.a) >> 1);
    return result;
}

pf_color_t
pf_color_blend_add(
    pf_color_t dst,
    pf_color_t src)
{
    pf_color_t result;
    result.c.r = (uint8_t)PF_MIN_255((int)(dst.c.r + src.c.r));
    result.c.g = (uint8_t)PF_MIN_255((int)(dst.c.g + src.c.g));
    result.c.b = (uint8_t)PF_MIN_255((int)(dst.c.b + src.c.b));
    result.c.a = (uint8_t)PF_MIN_255((int)(dst.c.a + src.c.a));
    return result;
}

pf_color_t
pf_color_blend_sub(
    pf_color_t dst,
    pf_color_t src)
{
    pf_color_t result;
    result.c.r = (uint8_t)PF_MAX_0((int)(dst.c.r - src.c.r));
    result.c.g = (uint8_t)PF_MAX_0((int)(dst.c.g - src.c.g));
    result.c.b = (uint8_t)PF_MAX_0((int)(dst.c.b - src.c.b));
    result.c.a = (uint8_t)PF_MAX_0((int)(dst.c.a - src.c.a));
    return result;
}

pf_color_t
pf_color_blend_mul(
    pf_color_t dst,
    pf_color_t src)
{
    pf_color_t result;
    result.c.r = (uint8_t)((dst.c.r * src.c.r) / 255);
    result.c.g = (uint8_t)((dst.c.g * src.c.g) / 255);
    result.c.b = (uint8_t)((dst.c.b * src.c.b) / 255);
    result.c.a = (uint8_t)((dst.c.a * src.c.a) / 255);
    return result;
}

pf_color_t
pf_color_blend_alpha(
    pf_color_t dst,
    pf_color_t src)
{
    uint32_t alpha = src.c.a + 1;
    uint32_t invAlpha = 256 - alpha;

    pf_color_t result;
    result.c.r = (uint8_t)((alpha * src.c.r + invAlpha * dst.c.r) >> 8);
    result.c.g = (uint8_t)((alpha * src.c.g + invAlpha * dst.c.g) >> 8);
    result.c.b = (uint8_t)((alpha * src.c.b + invAlpha * dst.c.b) >> 8);
    result.c.a = (uint8_t)((alpha * 255 + invAlpha * dst.c.a) >> 8);
    return result;
}

pf_color_t
pf_color_blend_screen(
    pf_color_t dst,
    pf_color_t src)
{
    pf_color_t result;
    result.c.r = (uint8_t)PF_MIN_255((int)((dst.c.r * (255 - src.c.r) >> 8) + src.c.r));
    result.c.g = (uint8_t)PF_MIN_255((int)((dst.c.g * (255 - src.c.g) >> 8) + src.c.g));
    result.c.b = (uint8_t)PF_MIN_255((int)((dst.c.b * (255 - src.c.b) >> 8) + src.c.b));
    result.c.a = (uint8_t)PF_MIN_255((int)((dst.c.a * (255 - src.c.a) >> 8) + src.c.a));
    return result;
}

pf_color_t
pf_color_blend_lighten(
    pf_color_t dst,
    pf_color_t src)
{
    pf_color_t result;
    result.c.r = (uint8_t)(PF_MAX(dst.c.r, src.c.r));
    result.c.g = (uint8_t)(PF_MAX(dst.c.g, src.c.g));
    result.c.b = (uint8_t)(PF_MAX(dst.c.b, src.c.b));
    result.c.a = (uint8_t)(PF_MAX(dst.c.a, src.c.a));
    return result;
}

pf_color_t
pf_color_blend_darken(
    pf_color_t dst,
    pf_color_t src)
{
    pf_color_t result;
    result.c.r = (uint8_t)(PF_MIN(dst.c.r, src.c.r));
    result.c.g = (uint8_t)(PF_MIN(dst.c.g, src.c.g));
    result.c.b = (uint8_t)(PF_MIN(dst.c.b, src.c.b));
    result.c.a = (uint8_t)(PF_MIN(dst.c.a, src.c.a));
    return result;
}
