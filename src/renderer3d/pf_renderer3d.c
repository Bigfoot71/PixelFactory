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

#include "pixelfactory/pf_renderer3d.h"
#include <float.h>

/* Public API */

pf_renderer3d_t
pf_renderer3d_create(
    uint32_t w, uint32_t h,
    pf_color_blend_fn blend,
    pf_depth_test_fn test)
{
    pf_renderer3d_t rn = { 0 };

    pf_mat4_identity(rn.mat_view);
    pf_mat4_perspective(rn.mat_proj, 45.0, (w > h) ? (float)w / h : (float)h / w, 0.01f, 1000.0f);

    rn.fb = pf_framebuffer_create(w, h, PF_BLANK);
    rn.zb = pf_depthbuffer_create(w, h, FLT_MAX);

    rn.blend = blend;
    rn.test = test;

    rn.viewport_position[0] = 0;
    rn.viewport_position[1] = 0;

    rn.viewport_dimension[0] = w - 1;
    rn.viewport_dimension[1] = h - 1;

    rn.cull_mode = PF_CULL_NONE;

    return rn;
}

void
pf_renderer3d_delete(
    pf_renderer3d_t* rn)
{
    pf_framebuffer_delete(&rn->fb);
    pf_depthbuffer_delete(&rn->zb);
    *rn = (pf_renderer3d_t) { 0 };
}

void
pf_renderer3d_clear(pf_renderer3d_t* rn, pf_color_t clear_color, float clear_depth)
{
#   if PF_SIMD_SIZE > 1

    size_t size = rn->fb.w * rn->fb.h;
    pf_color_t* fb = rn->fb.buffer;
    float* zb = rn->zb.buffer;

    // Load clear color/depth into an SIMD register
    pf_simd_i_t clear_color_vec = pf_simd_set1_i32(clear_color.v);
    pf_simd_t clear_depth_vec = pf_simd_set1_ps(clear_depth);

    // Fill the buffer using SIMD as much as possible
    size_t i = 0;
    for (; i + PF_SIMD_SIZE - 1 < size; i += PF_SIMD_SIZE) {
        pf_simd_store_i32((pf_simd_i_t*)(fb + i), clear_color_vec);
        pf_simd_store_ps(zb + i, clear_depth_vec);
    }

    // Fill the remaining items (less than PF_SIMD_SIZE elements)
    for (; i < size; ++i) {
        fb[i] = clear_color;
        zb[i] = clear_depth;
    }

#else

    size_t size = rn->fb.w * rn->fb.h;
    pf_color_t* fb = rn->fb.buffer;
    float* zb = rn->zb.buffer;

#ifdef _OPENMP
#   pragma omp parallel for \
        if (size >= PF_OMP_CLEAR_BUFFER_SIZE_THRESHOLD)
#endif //_OPENMP
    for (size_t i = 0; i < size; ++i) {
        fb[i] = clear_color;
        zb[i] = clear_depth;
    }

#endif
}

void
pf_renderer3d_viewport(pf_renderer3d_t* rn, int x, int y, int w, int h)
{
    rn->viewport_position[0] = x;
    rn->viewport_position[1] = y;
    rn->viewport_dimension[0] = w - 1;
    rn->viewport_dimension[1] = h - 1;
}
