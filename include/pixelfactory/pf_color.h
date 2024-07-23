#ifndef PF_COLOR_H
#define PF_COLOR_H

#include "math/pf_vec3.h"
#include "pf_stdinc.h"
#include "pf_helper.h"
#include <math.h>

typedef union {
    uint32_t v;
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } c;
} pf_color_t;

#define PF_WHITE        ((pf_color_t) { .c = { 255, 255, 255, 255 } })
#define PF_BLACK        ((pf_color_t) { .c = {   0,   0,   0, 255 } })
#define PF_BLANK        ((pf_color_t) { .c = {   0,   0,   0,   0 } })

#define PF_RED          ((pf_color_t) { .c = { 255,   0,   0, 255 } })
#define PF_GREEN        ((pf_color_t) { .c = {   0, 255,   0, 255 } })
#define PF_BLUE         ((pf_color_t) { .c = {   0,   0, 255, 255 } })
#define PF_YELLOW       ((pf_color_t) { .c = { 255, 255,   0, 255 } })
#define PF_MAGENTA      ((pf_color_t) { .c = { 255,   0, 255, 255 } })
#define PF_CYAN         ((pf_color_t) { .c = {   0, 255, 255, 255 } })

#define PF_ORANGE       ((pf_color_t) { .c = { 255, 165,   0, 255 } })
#define PF_PURPLE       ((pf_color_t) { .c = { 128,   0, 128, 255 } })
#define PF_BROWN        ((pf_color_t) { .c = { 165,  42,  42, 255 } })
#define PF_PINK         ((pf_color_t) { .c = { 255, 192, 203, 255 } })
#define PF_LIME         ((pf_color_t) { .c = { 191, 255,   0, 255 } })
#define PF_TURQUOISE    ((pf_color_t) { .c = {  64, 224, 208, 255 } })
#define PF_SILVER       ((pf_color_t) { .c = { 192, 192, 192, 255 } })
#define PF_GOLD         ((pf_color_t) { .c = { 255, 215,   0, 255 } })
#define PF_NAVY         ((pf_color_t) { .c = {   0,   0, 128, 255 } })
#define PF_OLIVE        ((pf_color_t) { .c = { 128, 128,   0, 255 } })
#define PF_MAROON       ((pf_color_t) { .c = { 128,   0,   0, 255 } })
#define PF_TEAL         ((pf_color_t) { .c = {   0, 128, 128, 255 } })
#define PF_CORAL        ((pf_color_t) { .c = { 255, 127,  80, 255 } })
#define PF_SALMON       ((pf_color_t) { .c = { 250, 128, 114, 255 } })
#define PF_INDIGO       ((pf_color_t) { .c = {  75,   0, 130, 255 } })
#define PF_VIOLET       ((pf_color_t) { .c = { 238, 130, 238, 255 } })
#define PF_KHAKI        ((pf_color_t) { .c = { 240, 230, 140, 255 } })
#define PF_BEIGE        ((pf_color_t) { .c = { 245, 245, 220, 255 } })

/* Functions Types */

typedef pf_color_t(*pf_color_blend_fn)(pf_color_t, pf_color_t);

/* General Functions */

pf_color_t
pf_color_lerpf(pf_color_t a, pf_color_t b, float t);

pf_color_t
pf_color_lerpi(pf_color_t a, pf_color_t b, int i, int max);

pf_color_t
pf_color_bary(pf_color_t c1, pf_color_t c2, pf_color_t c3, float w1, float w2, float w3);

pf_color_t
pf_color_bary_v(pf_color_t c1, pf_color_t c2, pf_color_t c3, const pf_vec3_t bary);

#ifdef __AVX2__

__m256i
pf_color_bary_avx(
    __m256i c1_r, __m256i c1_g, __m256i c1_b, __m256i c1_a,
    __m256i c2_r, __m256i c2_g, __m256i c2_b, __m256i c2_a,
    __m256i c3_r, __m256i c3_g, __m256i c3_b, __m256i c3_a,
    __m256 w1, __m256 w2, __m256 w3);

#endif //__AVX2__

pf_color_t
pf_color_scale(pf_color_t col, float scale);

pf_color_t
pf_color_scale_u(pf_color_t col, uint8_t scale);

void
pf_color_from_hsv(pf_color_t* color, float h, float s, float v);

void
pf_color_to_hsv(pf_color_t color, float* h, float* s, float* v);

/* Blending Functions */

pf_color_t
pf_color_blend_avg(pf_color_t dst, pf_color_t src);

pf_color_t
pf_color_blend_add(pf_color_t dst, pf_color_t src);

pf_color_t
pf_color_blend_sub(pf_color_t dst, pf_color_t src);

pf_color_t
pf_color_blend_mul(pf_color_t dst, pf_color_t src);

pf_color_t
pf_color_blend_alpha(pf_color_t dst, pf_color_t src);

pf_color_t
pf_color_blend_screen(pf_color_t dst, pf_color_t src);

pf_color_t
pf_color_blend_lighten(pf_color_t dst, pf_color_t src);

pf_color_t
pf_color_blend_darken(pf_color_t dst, pf_color_t src);

#endif //PF_COLOR_H
