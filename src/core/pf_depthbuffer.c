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

#include "pixelfactory/core/pf_depthbuffer.h"
#include "pixelfactory/components/pf_simd.h"
#include "pixelfactory/misc/pf_helper.h"

pf_depthbuffer_t
pf_depthbuffer_create(
    uint32_t w, uint32_t h,
    float def)
{
    pf_depthbuffer_t result = { 0 };
    if (w == 0 || h == 0) return result;

    size_t size = w * h;

    float* buffer = PF_MALLOC(size * sizeof(float));
    if (buffer == NULL) return result;

#if PF_SIMD_SIZE > 1
    pf_simd_t v_def = pf_simd_set1_ps(def);
    size_t i = 0;
    for (; i + PF_SIMD_SIZE - 1 < size; i += PF_SIMD_SIZE) {
        pf_simd_store_ps((pf_simd_i_t*)(buffer + i), v_def);
    }
    for (; i < size; ++i) {
        buffer[i] = def;
    }
#else
#ifdef _OPENMP
#   pragma omp parallel for \
        if (size >= PF_OMP_CLEAR_BUFFER_SIZE_THRESHOLD)
#endif //_OPENMP
    for (size_t i = 0; i < size; ++i) {
        buffer[i] = def;
    }
#endif

    result.buffer = buffer;
    result.w = w;
    result.h = h;

    return result;
}

void
pf_depthbuffer_delete(
    pf_depthbuffer_t* zb)
{
    if (zb->buffer != NULL) {
        PF_FREE(zb->buffer);
        zb->buffer = NULL;
    }
    zb->w = zb->h = 0;
}

bool
pf_depthbuffer_is_valid(
    const pf_depthbuffer_t* zb)
{
    return (zb->buffer != NULL || zb->w > 0 || zb->h > 0);
}

float
pf_depthbuffer_get(
    const pf_depthbuffer_t* zb,
    uint32_t x, uint32_t y)
{
    float result = 0;
    if (x < zb->w && y < zb->h) {
        result = zb->buffer[y * zb->w + x];
    }
    return result;
}

bool
pf_depthbuffer_test(
    const pf_depthbuffer_t* zb,
    uint32_t x, uint32_t y, float z,
    pf_depth_test_fn test)
{
    bool result = false;
    if (x < zb->w && y < zb->h) {
        result = test(zb->buffer[y * zb->w + x], z);
    }
    return result;
}

void
pf_depthbuffer_put(
    pf_depthbuffer_t* zb,
    uint32_t x, uint32_t y,
    float depth)
{
    if (x < zb->w && y < zb->h) {
        zb->buffer[y * zb->w + x] = depth;
    }
}

void
pf_depthbuffer_fill(
    pf_depthbuffer_t* zb,
    const uint32_t rect[4],
    float depth)
{
    int xmin, ymin, xmax, ymax;
    if (zb == NULL || zb->buffer == NULL) {
        return;
    }

    if (rect == NULL) {
        xmin = ymin = 0;
        xmax = zb->w - 1;
        ymax = zb->h - 1;
    } else {
        xmin = PF_MIN(rect[0], zb->w - 1);
        ymin = PF_MIN(rect[1], zb->h - 1);
        xmax = PF_MIN(rect[2], zb->w - 1);
        ymax = PF_MIN(rect[3], zb->h - 1);
        if (xmin > xmax) PF_SWAP(xmin, xmax);
        if (ymin > ymax) PF_SWAP(ymin, ymax);
    }

    float* row_ptr;
    pf_simd_t depth_vector = pf_simd_set1_ps(depth);

    for (int y = ymin; y <= ymax; ++y) {
        row_ptr = zb->buffer + y * zb->w + xmin;
        int x = xmin;
        for (; x <= xmax - PF_SIMD_SIZE + 1; x += PF_SIMD_SIZE) {
            pf_simd_store_ps(row_ptr + x, depth_vector);
        }
        for (; x <= xmax; ++x) {
            row_ptr[x] = depth;
        }
    }
}
