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

#ifndef PF_SIMD_H
#define PF_SIMD_H

#include <stdint.h>
#include <stddef.h>

#if defined(__AVX2__)
#include <immintrin.h>
#define PF_SIMD_SIZE 8
typedef __m256 pf_simd_t;
typedef __m256i pf_simd_i_t;
#elif defined(__SSE2__)
#include <immintrin.h>
#define PF_SIMD_SIZE 4
typedef __m128 pf_simd_t;
typedef __m128i pf_simd_i_t;
#else
#define PF_SIMD_SIZE 1
typedef float pf_simd_t;
typedef int32_t pf_simd_i_t;
#endif

#ifdef __SSE2__

static inline __m128i
_mm_mullo_epi32_sse2(__m128i x, __m128i y)
{
    // From Agner Fog's Vector Class Library
    // SEE: https://github.com/vectorclass/version2/blob/master/vectori128.h#L3108

    __m128i x13 = _mm_shuffle_epi32(x, 0xF5);              // (-,x3,-,x1)
    __m128i y13 = _mm_shuffle_epi32(y, 0xF5);              // (-,y3,-,y1)
    __m128i prod02 = _mm_mul_epu32(x, y);                  // (-,x2*y2,-,x0*y0)
    __m128i prod13 = _mm_mul_epu32(x13, y13);              // (-,x3*y3,-,x1*y1)
    __m128i prod01 = _mm_unpacklo_epi32(prod02, prod13);   // (-,-,x1*y1,x0*y0)
    __m128i prod23 = _mm_unpackhi_epi32(prod02, prod13);   // (-,-,x3*y3,x2*y2)
    return           _mm_unpacklo_epi64(prod01, prod23);   // (xy3,xy2,xy1,xy0)
}

static inline __m128i
_mm_shuffle_epi8_sse2(__m128i x, __m128i y)
{
    // From Agner Fog's Vector Class Library
    // SEE: https://github.com/vectorclass/version2/blob/master/vectori128.h#L5516

    uint8_t yy[16];
    int8_t  xx[16], rr[16];
    _mm_storeu_si128((__m128i*)xx, x);
    _mm_storeu_si128((__m128i*)yy, y);
    for (int j = 0; j < 16; j++) rr[j] = xx[yy[j] & 0x0F];
    return _mm_loadu_si128((__m128i const*)rr);
}

static inline __m128i
_mm_blendv_epi8_sse2(__m128i x, __m128i y, __m128i mask)
{
    __m128i not_mask = _mm_andnot_si128(mask, x);   // _mm_andnot_si128(mask, x) : bits of x where mask is 0
    __m128i masked_y = _mm_and_si128(mask, y);      // _mm_and_si128(mask, y) : bits of y where mask is 1
    return _mm_or_si128(not_mask, masked_y);        // Combine the two results to get the final result
}

#endif

static inline pf_simd_t
pf_simd_set1_ps(float x)
{
#if defined(__AVX2__)
    return _mm256_set1_ps(x);
#elif defined(__SSE2__)
    return _mm_set1_ps(x);
#else
    return x;
#endif
}

static inline pf_simd_i_t
pf_simd_set1_i32(int32_t x)
{
#if defined(__AVX2__)
    return _mm256_set1_epi32(x);
#elif defined(__SSE2__)
    return _mm_set1_epi32(x);
#else
    return x;
#endif
}

static inline pf_simd_i_t
pf_simd_setr_i32(int32_t i0, int32_t i1, int32_t i2, int32_t i3, int32_t i4, int32_t i5, int32_t i6, int32_t i7)
{
#if defined(__AVX2__)
    return _mm256_setr_epi32(i0, i1, i2, i3, i4, i5, i6, i7);
#elif defined(__SSE2__)
    (void)i4, (void)i5, (void)i6, (void)i7;
    return _mm_setr_epi32(i0, i1, i2, i3);
#else
    (void)i1, (void)i2, (void)i3, (void)i4, (void)i5, (void)i6, (void)i7;
    return i0; // For scalar fallback, just return the first value
#endif
}

static inline pf_simd_i_t
pf_simd_setzero_i32(void)
{
#if defined(__AVX2__)
    return _mm256_setzero_si256();
#elif defined(__SSE2__)
    return _mm_setzero_si128();
#else
    return 0;
#endif
}

static inline pf_simd_t
pf_simd_setzero_ps(void)
{
#if defined(__AVX2__)
    return _mm256_setzero_ps();
#elif defined(__SSE2__)
    return _mm_setzero_ps();
#else
    return 0.0f;
#endif
}

static inline pf_simd_i_t
pf_simd_add_i32(pf_simd_i_t x, pf_simd_i_t y)
{
#if defined(__AVX2__)
    return _mm256_add_epi32(x, y);
#elif defined(__SSE2__)
    return _mm_add_epi32(x, y);
#else
    return x + y;
#endif
}

static inline pf_simd_i_t
pf_simd_mullo_i32(pf_simd_i_t x, pf_simd_i_t y)
{
#if defined(__AVX2__)
    return _mm256_mullo_epi32(x, y);
#elif defined(__SSE4_1__)
    return _mm_mullo_epi32(x, y);
#elif defined(__SSE2__)
    return _mm_mullo_epi32_sse2(x, y);
#else
    return x * y;
#endif
}

static inline pf_simd_t
pf_simd_cvti32_ps(pf_simd_i_t x)
{
#if defined(__AVX2__)
    return _mm256_cvtepi32_ps(x);
#elif defined(__SSE2__)
    return _mm_cvtepi32_ps(x);
#else
    return (pf_simd_t)x;
#endif
}

static inline pf_simd_t
pf_simd_mul_ps(pf_simd_t x, pf_simd_t y)
{
#if defined(__AVX2__)
    return _mm256_mul_ps(x, y);
#elif defined(__SSE2__)
    return _mm_mul_ps(x, y);
#else
    return x * y;
#endif
}

static inline pf_simd_i_t
pf_simd_cvtf32_i32(pf_simd_t x)
{
#if defined(__AVX2__)
    return _mm256_cvtps_epi32(x);
#elif defined(__SSE2__)
    return _mm_cvtps_epi32(x);
#else
    return (pf_simd_i_t)x;
#endif
}

static inline pf_simd_t
pf_simd_rcp_ps(pf_simd_t x)
{
#if defined(__AVX2__)
    return _mm256_rcp_ps(x);
#elif defined(__SSE2__)
    return _mm_rcp_ps(x);
#else
    return 1.0f / x;
#endif
}

static inline void
pf_simd_store_ps(void* p, pf_simd_t x)
{
#if defined(__AVX2__)
    _mm256_storeu_ps((float*)p, x);
#elif defined(__SSE2__)
    _mm_storeu_ps((float*)p, x);
#else
    *(float*)p = x;
#endif
}

static inline void
pf_simd_store_i32(void* p, pf_simd_i_t x)
{
#if defined(__AVX2__)
    _mm256_storeu_si256((__m256i*)p, x);
#elif defined(__SSE2__)
    _mm_storeu_si128((__m128i*)p, x);
#else
    *(int32_t*)p = x;
#endif
}

static inline void
pf_simd_store_si32(void* p, pf_simd_i_t x)
{
#if defined(__AVX2__)
    _mm_storeu_si32((int32_t*)p, _mm256_castsi256_si128(x));
#elif defined(__SSE2__)
    _mm_storeu_si32((int32_t*)p, x);
#else
    *(int32_t*)p = (int32_t)x;
#endif
}

static inline pf_simd_i_t
pf_simd_load_i32(const void* p)
{
#if defined(__AVX2__)
    return _mm256_loadu_si256((const __m256i*)p);
#elif defined(__SSE2__)
    return _mm_loadu_si128((const __m128i*)p);
#else
    return *(const int32_t*)p;
#endif
}

static inline int32_t
pf_simd_extract_i32(pf_simd_i_t x, int32_t index)
{
#if defined(__AVX2__)
    return _mm256_extract_epi32(x, index);
#elif defined(__SSE2__)
    return _mm_extract_epi32(x, index);
#else
    (void)index;
    return x; // Scalar fallback does not support extraction
#endif
}

static inline pf_simd_i_t
pf_simd_permute_i32(pf_simd_i_t x, pf_simd_i_t y)
{
#if defined(__AVX2__)
    return _mm256_permutevar8x32_epi32(x, y);
#elif defined(__SSE4_1__)
    y = _mm_and_si128(y, _mm_set1_epi32(0x00000003));
    y = _mm_mullo_epi32(y, _mm_set1_epi32(0x04040404));
    y = _mm_or_si128(y, _mm_set1_epi32(0x03020100));
    return _mm_shuffle_epi8(x, y);
#elif defined(__SSE2__)
    y = _mm_and_si128(y, _mm_set1_epi32(0x00000003));
    y = _mm_mullo_epi32_sse2(y, _mm_set1_epi32(0x04040404));
    y = _mm_or_si128(y, _mm_set1_epi32(0x03020100));
    return _mm_shuffle_epi8_sse2(x, y);
#else
    (void)y;
    return x;
#endif
}

static inline pf_simd_i_t
pf_simd_srli_i32(pf_simd_i_t x, int32_t imm8)
{
#if defined(__AVX2__)
    return _mm256_srli_epi32(x, imm8);
#elif defined(__SSE2__)
    return _mm_srli_epi32(x, imm8);
#else
    return x >> imm8;
#endif
}

static inline pf_simd_i_t
pf_simd_and_i32(pf_simd_i_t x, pf_simd_i_t y)
{
#if defined(__AVX2__)
    return _mm256_and_si256(x, y);
#elif defined(__SSE2__)
    return _mm_and_si128(x, y);
#else
    return x & y;
#endif
}

static inline pf_simd_i_t
pf_simd_or_i32(pf_simd_i_t x, pf_simd_i_t y)
{
#if defined(__AVX2__)
    return _mm256_or_si256(x, y);
#elif defined(__SSE2__)
    return _mm_or_si128(x, y);
#else
    return x | y;
#endif
}

static inline pf_simd_i_t
pf_simd_slli_i32(pf_simd_i_t x, int32_t imm8)
{
#if defined(__AVX2__)
    return _mm256_slli_epi32(x, imm8);
#elif defined(__SSE2__)
    return _mm_slli_epi32(x, imm8);
#else
    return x << imm8;
#endif
}

static inline int32_t
pf_simd_movemask_ps(pf_simd_t x)
{
#if defined(__AVX2__)
    return _mm256_movemask_ps(x);
#elif defined(__SSE2__)
    return _mm_movemask_ps(x);
#else
    return x != 0.0f;
#endif
}

static inline int32_t
pf_simd_movemask_i8(pf_simd_i_t x)
{
#if defined(__AVX2__)
    return _mm256_movemask_epi8(x);
#elif defined(__SSE2__)
    return _mm_movemask_epi8(x);
#else
    return x != 0;
#endif
}

static inline pf_simd_i_t
pf_simd_blendv_i8(pf_simd_i_t a, pf_simd_i_t b, pf_simd_i_t mask)
{
#if defined(__AVX2__)
    return _mm256_blendv_epi8(a, b, mask);
#elif defined(__SSE4_1__)
    return _mm_blendv_epi8(a, b, mask);
#elif defined(__SSE2__)
    return _mm_blendv_epi8_sse2(a, b, mask);
#else
    return (mask ? b : a);
#endif
}

static inline pf_simd_i_t
pf_simd_cmpeq_i32(pf_simd_i_t x, pf_simd_i_t y)
{
#if defined(__AVX2__)
    return _mm256_cmpeq_epi32(x, y);
#elif defined(__SSE2__)
    return _mm_cmpeq_epi32(x, y);
#else
    return x == y;
#endif
}

static inline pf_simd_i_t
pf_simd_cmplt_i32(pf_simd_i_t x, pf_simd_i_t y)
{
#if defined(__AVX2__)
    return _mm256_cmpgt_epi32(y, x);
#elif defined(__SSE2__)
    return _mm_cmplt_epi32(x, y);
#else
    return x < y;
#endif
}

static inline pf_simd_i_t
pf_simd_cmpgt_i32(pf_simd_i_t x, pf_simd_i_t y)
{
#if defined(__AVX2__)
    return _mm256_cmpgt_epi32(x, y);
#elif defined(__SSE2__)
    return _mm_cmpgt_epi32(x, y);
#else
    return x > y;
#endif
}

static inline pf_simd_i_t
pf_simd_cmple_i32(pf_simd_i_t x, pf_simd_i_t y)
{
#if defined(__AVX2__)
    return _mm256_cmpgt_epi32(y, x);
#elif defined(__SSE2__)
    return _mm_cmplt_epi32(x, y);
#else
    return x <= y;
#endif
}

static inline pf_simd_i_t
pf_simd_cmpge_i32(pf_simd_i_t x, pf_simd_i_t y)
{
#if defined(__AVX2__)
    return _mm256_cmpgt_epi32(y, x);
#elif defined(__SSE2__)
    return _mm_cmpgt_epi32(y, x);
#else
    return x >= y;
#endif
}

#endif // PF_SIMD_H
