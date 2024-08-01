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

#include "pixelfactory/core/pf_renderer.h"
#include <float.h>

#define PF_TRAVEL_TEXTURE2D(PIXEL_CODE)                                             \
    pf_framebuffer_t* fb = &rn->fb;                                                 \
    int tex_w = tex->w;                                                             \
    int tex_h = tex->h;                                                             \
    int fb_w = fb->w;                                                               \
    int fb_h = fb->h;                                                               \
    int xmin = PF_CLAMP(x, 0, fb_w);                                                \
    int ymin = PF_CLAMP(y, 0, fb_h);                                                \
    int xmax = PF_CLAMP(x + tex_w, 0, fb_w);                                        \
    int ymax = PF_CLAMP(y + tex_h, 0, fb_h);                                        \
    for (int fb_y = ymin; fb_y < ymax; ++fb_y) {                                    \
        size_t ty_offset = (fb_y - y) * tex_w;                                      \
        size_t fb_y_offset = fb_y * fb_w;                                           \
        for (int fb_x = xmin; fb_x < xmax; ++fb_x) {                                \
            size_t t_offset = ty_offset + (fb_x - x);                               \
            size_t fb_offset = fb_y_offset + fb_x;                                  \
            PIXEL_CODE                                                              \
        }                                                                           \
    }

#define PF_TRAVEL_TEXTURE2D_REC(PIXEL_CODE)                                         \
    for (int y = ymin; y <= ymax; ++y) {                                            \
        float u = u_start;                                                          \
        float v = v_start + iv * (y - ymin);                                        \
        size_t y_offset = y * rn->fb.w;                                             \
        for (int x = xmin; x <= xmax; ++x) {                                        \
            size_t offset = y_offset + x;                                           \
            PIXEL_CODE                                                              \
            u += iu;                                                                \
        }                                                                           \
    }

#define PF_TRAVEL_TEXTURE2D_REC_OMP(PIXEL_CODE)                                     \
    _Pragma("omp parallel for                                                       \
        if ((xmax - xmin) * (ymax - ymin) >= PF_OMP_TEXTURE_RN2D_SIZE_THRESHOLD)")  \
    PF_TRAVEL_TEXTURE2D_REC(PIXEL_CODE)

#define PF_PREPARE_TEXTURE2D_MAT()                                                  \
    pf_framebuffer_t* fb = &rn->fb;                                                 \
    pf_mat3_t inv_transform;                                                        \
    pf_mat3_inverse(inv_transform, transform);                                      \
    PF_MATH_FLOAT corners[4][2] = {                                                 \
        {0, 0}, {tex->w, 0},                                                        \
        {tex->w, tex->h}, {0, tex->h}                                               \
    };                                                                              \
    PF_MATH_FLOAT transformed_corners[4][2];                                        \
    for (int i = 0; i < 4; ++i) {                                                   \
        pf_vec2_transform(transformed_corners[i], corners[i], transform);           \
    }                                                                               \
    float xmin = FLT_MAX, ymin = FLT_MAX;                                           \
    float xmax = -FLT_MAX, ymax = -FLT_MAX;                                         \
    for (int i = 0; i < 4; ++i) {                                                   \
        if (transformed_corners[i][0] < xmin) xmin = transformed_corners[i][0];     \
        if (transformed_corners[i][0] > xmax) xmax = transformed_corners[i][0];     \
        if (transformed_corners[i][1] < ymin) ymin = transformed_corners[i][1];     \
        if (transformed_corners[i][1] > ymax) ymax = transformed_corners[i][1];     \
    }                                                                               \
    int x1 = PF_CLAMP((int)floorf(xmin), 0, (int)fb->w - 1);                        \
    int y1 = PF_CLAMP((int)floorf(ymin), 0, (int)fb->h - 1);                        \
    int x2 = PF_CLAMP((int)ceilf(xmax), 0, (int)fb->w - 1);                         \
    int y2 = PF_CLAMP((int)ceilf(ymax), 0, (int)fb->h - 1);                         \
    if (x1 > x2) PF_SWAP(x1, x2);                                                   \
    if (y1 > y2) PF_SWAP(y1, y2);                                                   \
    float inv00 = inv_transform[0], inv01 = inv_transform[1], inv02 = inv_transform[2];\
    float inv10 = inv_transform[3], inv11 = inv_transform[4], inv12 = inv_transform[5];\

#define PF_TRAVEL_TEXTURE2D_MAT(PIXEL_CODE)                                         \
    for (int y = y1; y <= y2; ++y) {                                                \
        for (int x = x1; x <= x2; ++x) {                                            \
            float tx = inv00 * x + inv01 * y + inv02;                               \
            float ty = inv10 * x + inv11 * y + inv12;                               \
            float u = (tx - tex->tx) * tex->tx;                                     \
            float v = (ty - tex->ty) * tex->ty;                                      \
            if (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f) {                 \
                PIXEL_CODE                                                          \
            }                                                                       \
        }                                                                           \
    }

#define PF_TRAVEL_TEXTURE2D_MAT_OMP(PIXEL_CODE)                                     \
    _Pragma("omp parallel for                                                       \
            if ((x2 - x1) * (y2 - y1) >= PF_OMP_TEXTURE_RN2D_SIZE_THRESHOLD)")      \
    PF_TRAVEL_TEXTURE2D_MAT(PIXEL_CODE)

void
pf_renderer_texture2d(
    pf_renderer_t* rn,
    const pf_texture2d_t* tex,
    int x, int y)
{
    if (rn->conf2d != NULL && rn->conf2d->color_blend) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_TRAVEL_TEXTURE2D({
            pf_color_t* ptr = fb->buffer + fb_offset;
            *ptr = blend(*ptr, tex->getter(tex->texels, t_offset));
        })
    } else {
        PF_TRAVEL_TEXTURE2D({
            fb->buffer[fb_offset] = tex->getter(tex->texels, t_offset);
        })
    }
}

void
pf_renderer_texture2d_tint(
    pf_renderer_t* rn,
    const pf_texture2d_t* tex,
    int x, int y, pf_color_t tint)
{
    if (rn->conf2d != NULL && rn->conf2d->color_blend) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_TRAVEL_TEXTURE2D({
            pf_color_t* ptr = fb->buffer + fb_offset;
            pf_color_t texel = tex->getter(tex->texels, t_offset);
            *ptr = blend(*ptr, pf_color_blend_mul(texel, tint));
        })
    } else {
        PF_TRAVEL_TEXTURE2D({
            pf_color_t texel = tex->getter(tex->texels, t_offset);
            fb->buffer[fb_offset] = pf_color_blend_mul(texel, tint);
        })
    }
}

void
pf_renderer_texture2d_ex(
    pf_renderer_t *rn, const pf_texture2d_t *tex,
    int x, int y, float sx, float sy,
    float r, int ox, int oy)
{
    pf_mat3_t transform;
    {
        pf_mat3_t temp;

        // Initialize the identity matrix
        pf_mat3_identity(transform);

        // Translation to position the object at (x, y)
        pf_mat3_translate(temp, x - ox, y - oy);
        pf_mat3_mul(transform, transform, temp);

        // Rotation around point (ox, oy)
        pf_mat3_translate(temp, ox, oy);
        pf_mat3_mul(transform, transform, temp);
        pf_mat3_rotate(temp, r);
        pf_mat3_mul(transform, transform, temp);
        pf_mat3_translate(temp, -ox, -oy);
        pf_mat3_mul(transform, transform, temp);

        // Scaling by (sx, sy)
        pf_mat3_translate(temp, ox, oy);
        pf_mat3_mul(transform, transform, temp);
        pf_mat3_scale(temp, sx, sy);
        pf_mat3_mul(transform, transform, temp);
        pf_mat3_translate(temp, -ox, -oy);
        pf_mat3_mul(transform, transform, temp);
    }

    // Calling the ex function with the full transformation matrix
    pf_renderer_texture2d_mat(rn, tex, transform);
}

void
pf_renderer_texture2d_ex_tint(
    pf_renderer_t *rn, const pf_texture2d_t *tex,
    int x, int y, float sx, float sy,
    float r, int ox, int oy,
    pf_color_t tint)
{
    pf_mat3_t transform;
    {
        pf_mat3_t temp;

        // Initialize the identity matrix
        pf_mat3_identity(transform);

        // Translation to position the object at (x, y)
        pf_mat3_translate(temp, x - ox, y - oy);
        pf_mat3_mul(transform, transform, temp);

        // Rotation around point (ox, oy)
        pf_mat3_translate(temp, ox, oy);
        pf_mat3_mul(transform, transform, temp);
        pf_mat3_rotate(temp, r);
        pf_mat3_mul(transform, transform, temp);
        pf_mat3_translate(temp, -ox, -oy);
        pf_mat3_mul(transform, transform, temp);

        // Scaling by (sx, sy)
        pf_mat3_translate(temp, ox, oy);
        pf_mat3_mul(transform, transform, temp);
        pf_mat3_scale(temp, sx, sy);
        pf_mat3_mul(transform, transform, temp);
        pf_mat3_translate(temp, -ox, -oy);
        pf_mat3_mul(transform, transform, temp);
    }

    // Calling the ex function with the full transformation matrix
    pf_renderer_texture2d_mat_tint(rn, tex, transform, tint);
}

void
pf_renderer_texture2d_ex_map(
    pf_renderer_t *rn, const pf_texture2d_t *tex,
    int x, int y, float sx, float sy,
    float r, int ox, int oy,
    pf_proc2d_fragment_fn frag_proc)
{
    pf_mat3_t transform;
    {
        pf_mat3_t temp;

        // Initialize the identity matrix
        pf_mat3_identity(transform);

        // Translation to position the object at (x, y)
        pf_mat3_translate(temp, x - ox, y - oy);
        pf_mat3_mul(transform, transform, temp);

        // Rotation around point (ox, oy)
        pf_mat3_translate(temp, ox, oy);
        pf_mat3_mul(transform, transform, temp);
        pf_mat3_rotate(temp, r);
        pf_mat3_mul(transform, transform, temp);
        pf_mat3_translate(temp, -ox, -oy);
        pf_mat3_mul(transform, transform, temp);

        // Scaling by (sx, sy)
        pf_mat3_translate(temp, ox, oy);
        pf_mat3_mul(transform, transform, temp);
        pf_mat3_scale(temp, sx, sy);
        pf_mat3_mul(transform, transform, temp);
        pf_mat3_translate(temp, -ox, -oy);
        pf_mat3_mul(transform, transform, temp);
    }

    // Calling the ex function with the full transformation matrix
    pf_renderer_texture2d_mat_map(rn, tex, transform, frag_proc);
}

// TODO: It would be more ideal if the function was implemented without having to render via vertexbuffers
void
pf_renderer_texture2d_rec(
    pf_renderer_t *rn, const pf_texture2d_t *tex,
    const int* src_rect, const int* dst_rect,
    float r, int ox, int oy)
{
    static const uint16_t indices[6] = {
        0, 1, 2,
        2, 3, 0
    };

    /* Compute vertices */

    pf_vec2_t positions[4] = {
        { dst_rect[0], dst_rect[1] },   // Top Left
        { dst_rect[0], dst_rect[3] },   // Bottom Left
        { dst_rect[2], dst_rect[3] },   // Bottom Right
        { dst_rect[2], dst_rect[1] }    // Top Right
    };

    if (r != 0) {
        float rs = sinf(r);
        float rc = cosf(r);

        for (int i = 0; i < 4; i++) {
            float dx = positions[i][0] - ox;
            float dy = positions[i][1] - oy;

            positions[i][0] = ox + (dx * rc - dy * rs);
            positions[i][1] = oy + (dx * rs + dy * rc);
        }
    }

    /* Compute texcoords */

    pf_vec2_t texcoords[4] = { 0 };
    {
        bool x_flip = 0;
        float src_x = src_rect[0];
        float src_y = src_rect[1];
        float src_w = src_rect[2] - src_rect[0];
        float src_h = src_rect[3] - src_rect[1];

        if (src_w < 0) x_flip = 1, src_w *= -1;
        if (src_h < 0) src_y -= src_h;

        if (x_flip) {
            texcoords[0][0] = (src_x + src_w) * tex->tx;
            texcoords[0][1] = src_y * tex->ty;

            texcoords[1][0] = (src_x + src_w) * tex->tx;
            texcoords[1][1] = (src_y + src_h) * tex->ty;

            texcoords[2][0] = src_x * tex->tx;
            texcoords[2][1] = (src_y + src_h) * tex->ty;

            texcoords[3][0] = src_x * tex->tx;
            texcoords[3][1] = src_y * tex->ty;
        } else {
            texcoords[0][0] = src_x * tex->tx;
            texcoords[0][1] = src_y * tex->ty;

            texcoords[1][0] = src_x * tex->tx;
            texcoords[1][1] = (src_y + src_h) * tex->ty;

            texcoords[2][0] = (src_x + src_w) * tex->tx;
            texcoords[2][1] = (src_y + src_h) * tex->ty;

            texcoords[3][0] = (src_x + src_w) * tex->tx;
            texcoords[3][1] = src_y * tex->ty;
        }
    }

    /* Rendering */

    const struct {
        float* positions;
        float* texcoords;
        pf_color_t* colors;
        const uint16_t* indices;
        uint32_t num_vertices;
        uint32_t num_indices;
    } vb = {
        .positions = (float*)positions,
        .texcoords = (float*)texcoords,
        .colors = NULL,
        .indices = indices,
        .num_vertices = 4,
        .num_indices = 6
    };

    pf_proc2d_t proc = { 0 };
    proc.fragment = pf_proc2d_fragment_texture_as_uniform;
    proc.uniforms = tex;

    pf_renderer_vertexbuffer2d(rn,
        (const pf_vertexbuffer_t*)(&vb),
        rn->conf2d ? rn->conf2d->mat_view : NULL, &proc);
}

// TODO: It would be more ideal if the function was implemented without having to render via vertexbuffers
void
pf_renderer_texture2d_rec_tint(
    pf_renderer_t *rn, const pf_texture2d_t *tex,
    const int* src_rect, const int* dst_rect,
    float r, int ox, int oy,
    pf_color_t tint)
{
    static const uint16_t indices[6] = {
        0, 1, 2,
        2, 3, 0
    };

    /* Compute vertices */

    pf_vec2_t positions[4] = {
        { dst_rect[0], dst_rect[1] },   // Top Left
        { dst_rect[0], dst_rect[3] },   // Bottom Left
        { dst_rect[2], dst_rect[3] },   // Bottom Right
        { dst_rect[2], dst_rect[1] }    // Top Right
    };

    if (r != 0) {
        float rs = sinf(r);
        float rc = cosf(r);

        for (int i = 0; i < 4; i++) {
            float dx = positions[i][0] - ox;
            float dy = positions[i][1] - oy;

            positions[i][0] = ox + (dx * rc - dy * rs);
            positions[i][1] = oy + (dx * rs + dy * rc);
        }
    }

    /* Compute texcoords */

    pf_vec2_t texcoords[4] = { 0 };
    {
        bool x_flip = 0;
        float src_x = src_rect[0];
        float src_y = src_rect[1];
        float src_w = src_rect[2] - src_rect[0];
        float src_h = src_rect[3] - src_rect[1];

        if (src_w < 0) x_flip = 1, src_w *= -1;
        if (src_h < 0) src_y -= src_h;

        if (x_flip) {
            texcoords[0][0] = (src_x + src_w) * tex->tx;
            texcoords[0][1] = src_y * tex->ty;

            texcoords[1][0] = (src_x + src_w) * tex->tx;
            texcoords[1][1] = (src_y + src_h) * tex->ty;

            texcoords[2][0] = src_x * tex->tx;
            texcoords[2][1] = (src_y + src_h) * tex->ty;

            texcoords[3][0] = src_x * tex->tx;
            texcoords[3][1] = src_y * tex->ty;
        } else {
            texcoords[0][0] = src_x * tex->tx;
            texcoords[0][1] = src_y * tex->ty;

            texcoords[1][0] = src_x * tex->tx;
            texcoords[1][1] = (src_y + src_h) * tex->ty;

            texcoords[2][0] = (src_x + src_w) * tex->tx;
            texcoords[2][1] = (src_y + src_h) * tex->ty;

            texcoords[3][0] = (src_x + src_w) * tex->tx;
            texcoords[3][1] = src_y * tex->ty;
        }
    }

    /* Colors */

    pf_color_t colors[4] = {
        tint, tint,
        tint, tint
    };

    /* Rendering */

    const struct {
        float* positions;
        float* texcoords;
        pf_color_t* colors;
        const uint16_t* indices;
        uint32_t num_vertices;
        uint32_t num_indices;
    } vb = {
        .positions = (float*)positions,
        .texcoords = (float*)texcoords,
        .colors = colors,
        .indices = indices,
        .num_vertices = 4,
        .num_indices = 6
    };

    pf_proc2d_t proc = { 0 };
    proc.fragment = pf_proc2d_fragment_texture_as_uniform;
    proc.uniforms = tex;

    pf_renderer_vertexbuffer2d(rn,
        (const pf_vertexbuffer_t*)(&vb),
        rn->conf2d ? rn->conf2d->mat_view : NULL, &proc);
}

// TODO: It would be more ideal if the function was implemented without having to render via vertexbuffers
void
pf_renderer_texture2d_rec_map(
    pf_renderer_t *rn, const pf_texture2d_t *tex,
    const int* src_rect, const int* dst_rect,
    float r, int ox, int oy,
    pf_proc2d_fragment_fn frag_proc)
{
    static const uint16_t indices[6] = {
        0, 1, 2,
        2, 3, 0
    };

    /* Compute vertices */

    pf_vec2_t positions[4] = {
        { dst_rect[0], dst_rect[1] },   // Top Left
        { dst_rect[0], dst_rect[3] },   // Bottom Left
        { dst_rect[2], dst_rect[3] },   // Bottom Right
        { dst_rect[2], dst_rect[1] }    // Top Right
    };

    if (r != 0) {
        float rs = sinf(r);
        float rc = cosf(r);

        for (int i = 0; i < 4; i++) {
            float dx = positions[i][0] - ox;
            float dy = positions[i][1] - oy;

            positions[i][0] = ox + (dx * rc - dy * rs);
            positions[i][1] = oy + (dx * rs + dy * rc);
        }
    }

    /* Compute texcoords */

    pf_vec2_t texcoords[4] = { 0 };
    {
        bool x_flip = 0;
        float src_x = src_rect[0];
        float src_y = src_rect[1];
        float src_w = src_rect[2] - src_rect[0];
        float src_h = src_rect[3] - src_rect[1];

        if (src_w < 0) x_flip = 1, src_w *= -1;
        if (src_h < 0) src_y -= src_h;

        if (x_flip) {
            texcoords[0][0] = (src_x + src_w) * tex->tx;
            texcoords[0][1] = src_y * tex->ty;

            texcoords[1][0] = (src_x + src_w) * tex->tx;
            texcoords[1][1] = (src_y + src_h) * tex->ty;

            texcoords[2][0] = src_x * tex->tx;
            texcoords[2][1] = (src_y + src_h) * tex->ty;

            texcoords[3][0] = src_x * tex->tx;
            texcoords[3][1] = src_y * tex->ty;
        } else {
            texcoords[0][0] = src_x * tex->tx;
            texcoords[0][1] = src_y * tex->ty;

            texcoords[1][0] = src_x * tex->tx;
            texcoords[1][1] = (src_y + src_h) * tex->ty;

            texcoords[2][0] = (src_x + src_w) * tex->tx;
            texcoords[2][1] = (src_y + src_h) * tex->ty;

            texcoords[3][0] = (src_x + src_w) * tex->tx;
            texcoords[3][1] = src_y * tex->ty;
        }
    }

    /* Rendering */

    const struct {
        float* positions;
        float* texcoords;
        pf_color_t* colors;
        const uint16_t* indices;
        uint32_t num_vertices;
        uint32_t num_indices;
    } vb = {
        .positions = (float*)positions,
        .texcoords = (float*)texcoords,
        .colors = NULL,
        .indices = indices,
        .num_vertices = 4,
        .num_indices = 6
    };

    pf_proc2d_t proc = { 0 };
    proc.fragment = frag_proc;
    proc.uniforms = tex;

    pf_renderer_vertexbuffer2d(rn,
        (const pf_vertexbuffer_t*)(&vb),
        rn->conf2d ? rn->conf2d->mat_view : NULL, &proc);
}

void
pf_renderer_texture2d_mat(
    pf_renderer_t* rn,
    const pf_texture2d_t* tex,
    pf_mat3_t transform)
{
    PF_PREPARE_TEXTURE2D_MAT();

#if defined(_OPENMP)
    if (rn->conf2d && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_TRAVEL_TEXTURE2D_MAT_OMP({
            pf_color_t* ptr = fb->buffer + y * fb->w + x;
            pf_color_t texel = tex->sampler(tex, u, v);
            *ptr = blend(*ptr, texel);
        })
    } else {
        PF_TRAVEL_TEXTURE2D_MAT_OMP({
            pf_color_t texel = tex->sampler(tex, u, v);
            fb->buffer[y * fb->w + x] = texel;
        })
    }
#else
    if (rn->conf2d && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_TRAVEL_TEXTURE2D_MAT({
            pf_color_t* ptr = fb->buffer + y * fb->w + x;
            pf_color_t texel = tex->sampler(tex, u, v);
            *ptr = blend(*ptr, texel);
        })
    } else {
        PF_TRAVEL_TEXTURE2D_MAT({
            pf_color_t texel = tex->sampler(tex, u, v);
            fb->buffer[y * fb->w + x] = texel;
        })
    }
#endif
}

void
pf_renderer_texture2d_mat_tint(
    pf_renderer_t* rn,
    const pf_texture2d_t* tex,
    pf_mat3_t transform,
    pf_color_t tint)
{
    PF_PREPARE_TEXTURE2D_MAT();

#if defined(_OPENMP)
    if (rn->conf2d && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_TRAVEL_TEXTURE2D_MAT_OMP({
            pf_color_t texel = tex->sampler(tex, u, v);
            pf_color_t* ptr = fb->buffer + y * fb->w + x;
            *ptr = blend(*ptr, pf_color_blend_mul(texel, tint));
        })
    } else {
        PF_TRAVEL_TEXTURE2D_MAT_OMP({
            pf_color_t texel = tex->sampler(tex, u, v);
            fb->buffer[y * fb->w + x] = pf_color_blend_mul(texel, tint);
        })
    }
#else
    if (rn->conf2d && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_TRAVEL_TEXTURE2D_MAT({
            pf_color_t texel = tex->sampler(tex, u, v);
            pf_color_t* ptr = fb->buffer + y * fb->w + x;
            *ptr = blend(*ptr, pf_color_blend_mul(texel, tint));
        })
    } else {
        PF_TRAVEL_TEXTURE2D_MAT({
            pf_color_t texel = tex->sampler(tex, u, v);
            fb->buffer[y * fb->w + x] = pf_color_blend_mul(texel, tint);
        })
    }
#endif
}

void
pf_renderer_texture2d_mat_map(
    pf_renderer_t* rn,
    const pf_texture2d_t* tex,
    pf_mat3_t transform,
    pf_proc2d_fragment_fn frag_proc)
{
    PF_PREPARE_TEXTURE2D_MAT();

#if defined(_OPENMP)
    if (rn->conf2d && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_TRAVEL_TEXTURE2D_MAT_OMP({
            pf_vertex_t vertex = pf_vertex_create_2d(x, y, u, v, PF_WHITE);
            pf_color_t *ptr = rn->fb.buffer + y * fb->w + x;
            pf_color_t final_color = *ptr;

            frag_proc(rn, &vertex, &final_color, tex);
            *ptr = blend(*ptr, final_color);
        })
    } else {
        PF_TRAVEL_TEXTURE2D_MAT_OMP({
            pf_vertex_t vertex = pf_vertex_create_2d(x, y, u, v, PF_WHITE);
            pf_color_t *ptr = rn->fb.buffer + y * fb->w + x;
            pf_color_t final_color = *ptr;

            frag_proc(rn, &vertex, &final_color, tex);
            *ptr = final_color;
        })
    }
#else
    if (rn->conf2d && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_TRAVEL_TEXTURE2D_MAT({
            pf_vertex_t vertex = pf_vertex_create_2d(x, y, u, v, PF_WHITE);
            pf_color_t *ptr = rn->fb.buffer + y * fb->w + x;
            pf_color_t final_color = *ptr;

            frag_proc(rn, &vertex, &final_color, tex);
            *ptr = blend(*ptr, final_color);
        })
    } else {
        PF_TRAVEL_TEXTURE2D_MAT({
            pf_vertex_t vertex = pf_vertex_create_2d(x, y, u, v, PF_WHITE);
            pf_color_t *ptr = rn->fb.buffer + y * fb->w + x;
            pf_color_t final_color = *ptr;

            frag_proc(rn, &vertex, &final_color, tex);
            *ptr = final_color;
        })
    }
#endif
}
