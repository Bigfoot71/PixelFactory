#include "pixelfactory/pf_renderer2d.h"
#include "pixelfactory/pf_framebuffer.h"
#include "pixelfactory/pf_stdinc.h"

/* Public API */

pf_renderer2d_t
pf_renderer2d_create(uint32_t w, uint32_t h, pf_color_blend_fn blend)
{
    pf_renderer2d_t rn = { 0 };
    pf_mat3_identity(rn.mat_view);
    rn.fb = pf_framebuffer_create(w, h, PF_BLANK);
    rn.blend = blend;
    return rn;
}

void
pf_renderer2d_delete(pf_renderer2d_t* rn)
{
    pf_framebuffer_delete(&rn->fb);
    rn->blend = NULL;
}

void
pf_renderer2d_clear(pf_renderer2d_t* rn, pf_color_t clear_color)
{
#   ifdef __AVX2__

    pf_color_t* buffer = rn->fb.buffer;
    size_t size = rn->fb.w * rn->fb.h;

    // Load clear color into an AVX register
    __m256i clear_color_vec = _mm256_set1_epi32(clear_color.v);

    // Fill the buffer using AVX as much as possible
    size_t i = 0;
    for (; i + 7 < size; i += 8) {
        _mm256_storeu_si256((__m256i*)&buffer[i], clear_color_vec);
    }

    // Fill the remaining items (less than 8 elements)
    for (; i < size; ++i) {
        buffer[i] = clear_color;
    }

#else

    pf_color_t* buffer = rn->fb.buffer;
    size_t size = rn->fb.w * rn->fb.h;

    for (size_t i = 0; i < size; ++i) {
        buffer[i] = clear_color;
    }

#endif
}

void
pf_renderer2d_point(pf_renderer2d_t* rn, int x, int y, pf_color_t color)
{
    if (rn->blend != NULL) {
        color = rn->blend(pf_framebuffer_get(&rn->fb, x, y), color);
    }
    pf_framebuffer_put(&rn->fb, x, y, color);
}
