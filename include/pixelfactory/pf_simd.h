#ifndef PF_SIMD_H
#define PF_SIMD_H

#include <stdint.h>
#include <stddef.h>

#if defined(__AVX2__)
#include <immintrin.h>
#define PF_SIMD_SIZE 8
typedef __m256 pf_simd_t;
typedef __m256i pf_simd_i_t;
#else
#define PF_SIMD_SIZE 1
typedef float pf_simd_t;
typedef int pf_simd_i_t;
#endif

static inline pf_simd_t
pf_simd_set1_ps(float x)
{
#if defined(__AVX2__)
    return _mm256_set1_ps(x);
#else
    return x;
#endif
}

static inline pf_simd_i_t
pf_simd_set1_i32(int32_t x)
{
#if defined(__AVX2__)
    return _mm256_set1_epi32(x);
#else
    return x;
#endif
}

static inline pf_simd_i_t
pf_simd_setr_i32(int32_t i0, int32_t i1, int32_t i2, int32_t i3, int32_t i4, int32_t i5, int32_t i6, int32_t i7)
{
#if defined(__AVX2__)
    return _mm256_setr_epi32(i0, i1, i2, i3, i4, i5, i6, i7);
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
#else
    return 0; // Fallback for scalar implementation
#endif
}

static inline pf_simd_t
pf_simd_setzero_ps(void)
{
#if defined(__AVX2__)
    return _mm256_setzero_ps();
#else
    return 0.0f; // Fallback for scalar implementation
#endif
}

static inline pf_simd_i_t
pf_simd_add_i32(pf_simd_i_t x, pf_simd_i_t y)
{
#if defined(__AVX2__)
    return _mm256_add_epi32(x, y);
#else
    return x + y;
#endif
}

static inline pf_simd_i_t
pf_simd_mullo_i32(pf_simd_i_t x, pf_simd_i_t y)
{
#if defined(__AVX2__)
    return _mm256_mullo_epi32(x, y);
#else
    return x * y;
#endif
}

static inline pf_simd_t
pf_simd_cvti32_ps(pf_simd_i_t x)
{
#if defined(__AVX2__)
    return _mm256_cvtepi32_ps(x);
#else
    return (pf_simd_t)x;
#endif
}

static inline pf_simd_t
pf_simd_mul_ps(pf_simd_t x, pf_simd_t y)
{
#if defined(__AVX2__)
    return _mm256_mul_ps(x, y);
#else
    return x * y;
#endif
}

static inline pf_simd_i_t
pf_simd_cvtf32_i32(pf_simd_t x)
{
#if defined(__AVX2__)
    return _mm256_cvtps_epi32(x);
#else
    return (pf_simd_i_t)x;
#endif
}

static inline void
pf_simd_store_ps(void* p, pf_simd_t x)
{
#if defined(__AVX2__)
    _mm256_storeu_ps((float*)p, x);
#else
    *(int32_t*)p = x;
#endif
}

static inline void
pf_simd_store_i32(void* p, pf_simd_i_t x)
{
#if defined(__AVX2__)
    _mm256_storeu_si256((__m256i*)p, x);
#else
    *(int32_t*)p = x;
#endif
}

static inline void
pf_simd_store_si32(void* p, pf_simd_i_t x)
{
#if defined(__AVX2__)
    _mm_storeu_si32((int32_t*)p, _mm256_castsi256_si128(x));
#else
    *(int32_t*)p = (int32_t)x; // Fallback for scalar implementation
#endif
}

static inline pf_simd_i_t
pf_simd_load_i32(const void* p)
{
#if defined(__AVX2__)
    return _mm256_loadu_si256((const __m256i*)p);
#else
    return *(const int32_t*)p;
#endif
}

static inline int32_t
pf_simd_extract_i32(pf_simd_i_t x, int index)
{
#if defined(__AVX2__)
    return _mm256_extract_epi32(x, index);
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
#else
    (void)y;
    return x;
#endif
}

static inline pf_simd_i_t
pf_simd_srli_i32(pf_simd_i_t x, int imm8)
{
#if defined(__AVX2__)
    return _mm256_srli_epi32(x, imm8);
#else
    return x >> imm8;
#endif
}

static inline pf_simd_i_t
pf_simd_and_i32(pf_simd_i_t x, pf_simd_i_t y)
{
#if defined(__AVX2__)
    return _mm256_and_si256(x, y);
#else
    return x & y;
#endif
}

static inline pf_simd_i_t
pf_simd_or_i32(pf_simd_i_t x, pf_simd_i_t y)
{
#if defined(__AVX2__)
    return _mm256_or_si256(x, y);
#else
    return x | y;
#endif
}

static inline pf_simd_i_t
pf_simd_slli_i32(pf_simd_i_t x, int imm8)
{
#if defined(__AVX2__)
    return _mm256_slli_epi32(x, imm8);
#else
    return x << imm8;
#endif
}

static inline int
pf_simd_movemask_ps(pf_simd_t x)
{
#if defined(__AVX2__)
    return _mm256_movemask_ps(x);
#else
    return x != 0.0f;
#endif
}

static inline int
pf_simd_movemask_i8(pf_simd_i_t x)
{
#if defined(__AVX2__)
    return _mm256_movemask_epi8(x);
#else
    return x != 0;
#endif
}

static inline pf_simd_i_t
pf_simd_blendv_i8(pf_simd_i_t a, pf_simd_i_t b, pf_simd_i_t mask)
{
#if defined(__AVX2__)
    return _mm256_blendv_epi8(a, b, mask);
#else
    return (mask ? b : a); // Fallback for scalar implementation
#endif
}

static inline pf_simd_i_t
pf_simd_cmpgt_i32(pf_simd_i_t x, pf_simd_i_t y)
{
#if defined(__AVX2__)
    return _mm256_cmpgt_epi32(x, y);
#else
    return x > y ? -1 : 0; // Fallback for scalar comparison
#endif
}

#endif // PF_SIMD_H
