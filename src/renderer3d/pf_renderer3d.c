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
    pf_mat4_perspective(rn.mat_proj, 45.0, (w > h) ? w / h : h / w, 0.01f, 1000.0f);

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
#   ifdef __AVX2__

    size_t size = rn->fb.w * rn->fb.h;
    pf_color_t* fb = rn->fb.buffer;
    float* zb = rn->zb.buffer;

    // Load clear color/depth into an AVX register
    __m256i clear_color_vec = _mm256_set1_epi32(clear_color.v);
    __m256 clear_depth_vec = _mm256_set1_ps(clear_depth);

    // Fill the buffer using AVX as much as possible
    size_t i = 0;
    for (; i + 7 < size; i += 8) {
        _mm256_storeu_si256((__m256i*)(fb + i), clear_color_vec);
        _mm256_storeu_ps(zb + i, clear_depth_vec);
    }

    // Fill the remaining items (less than 8 elements)
    for (; i < size; ++i) {
        fb[i] = clear_color;
        zb[i] = clear_depth;
    }

#else

    size_t size = rn->fb.w * rn->fb.h;
    pf_color_t* fb = rn->fb.buffer;
    float* zb = rn->zb.buffer;

    for (size_t i = 0; i < size; ++i) {
        fb[i] = clear_color;
        zb[i] = depth_color;
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
