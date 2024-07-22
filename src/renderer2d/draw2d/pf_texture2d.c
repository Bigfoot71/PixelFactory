#include "pixelfactory/pf_color.h"
#include "pixelfactory/pf_renderer2d.h"
#include "pixelfactory/math/pf_mat3.h"
#include "pixelfactory/math/pf_vec2.h"
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

#define PF_TRAVEL_TEXTURE2D_MAT(PIXEL_CODE)                                         \
    pf_framebuffer_t* fb = &rn->fb;                                                 \
    pf_mat3_t invTransform;                                                         \
    pf_mat3_inverse(invTransform, transform);                                       \
    float corners[4][2] = {                                                         \
        {0, 0}, {tex->w, 0},                                                        \
        {tex->w, tex->h}, {0, tex->h}                                               \
    };                                                                              \
    float transformedCorners[4][2];                                                 \
    for (int i = 0; i < 4; ++i) {                                                   \
        pf_vec2_transform(transformedCorners[i], corners[i], transform);            \
    }                                                                               \
    float xmin = FLT_MAX, ymin = FLT_MAX;                                           \
    float xmax = -FLT_MAX, ymax = -FLT_MAX;                                         \
    for (int i = 0; i < 4; ++i) {                                                   \
        if (transformedCorners[i][0] < xmin) xmin = transformedCorners[i][0];       \
        if (transformedCorners[i][0] > xmax) xmax = transformedCorners[i][0];       \
        if (transformedCorners[i][1] < ymin) ymin = transformedCorners[i][1];       \
        if (transformedCorners[i][1] > ymax) ymax = transformedCorners[i][1];       \
    }                                                                               \
    int x1 = PF_CLAMP((int)floorf(xmin), 0, (int)fb->w - 1);                        \
    int y1 = PF_CLAMP((int)floorf(ymin), 0, (int)fb->h - 1);                        \
    int x2 = PF_CLAMP((int)ceilf(xmax), 0, (int)fb->w - 1);                         \
    int y2 = PF_CLAMP((int)ceilf(ymax), 0, (int)fb->h - 1);                         \
    if (x1 > x2) PF_SWAP(x1, x2);                                                   \
    if (y1 > y2) PF_SWAP(y1, y2);                                                   \
    float inv00 = invTransform[0], inv01 = invTransform[1], inv02 = invTransform[2];\
    float inv10 = invTransform[3], inv11 = invTransform[4], inv12 = invTransform[5];\
    _Pragma("omp parallel for")                                                     \
    for (int y = y1; y <= y2; ++y) {                                                \
        for (int x = x1; x <= x2; ++x) {                                            \
            float tx = inv00 * x + inv01 * y + inv02;                               \
            float ty = inv10 * x + inv11 * y + inv12;                               \
            float u = (tx - tex->tx) / tex->w;                                      \
            float v = (ty - tex->ty) / tex->h;                                      \
            if (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f) {                 \
                PIXEL_CODE                                                          \
            }                                                                       \
        }                                                                           \
    }

void
pf_renderer2d_texture2d(pf_renderer2d_t* rn, const pf_texture2d_t* tex, int x, int y)
{
    if (rn->blend == NULL) {
        PF_TRAVEL_TEXTURE2D({
            fb->buffer[fb_offset] = tex->getter(tex->texels, t_offset);
        })
    } else {
        PF_TRAVEL_TEXTURE2D({
            pf_color_t* ptr = fb->buffer + fb_offset;
            *ptr = rn->blend(*ptr, tex->getter(tex->texels, t_offset));
        })
    }
}

void
pf_renderer2d_texture2d_tint(pf_renderer2d_t* rn, const pf_texture2d_t* tex, int x, int y, pf_color_t tint)
{
    if (rn->blend == NULL) {
        PF_TRAVEL_TEXTURE2D({
            pf_color_t texel = tex->getter(tex->texels, t_offset);
            fb->buffer[fb_offset] = pf_color_blend_mul(texel, tint);
        })
    } else {
        PF_TRAVEL_TEXTURE2D({
            pf_color_t* ptr = fb->buffer + fb_offset;
            pf_color_t texel = tex->getter(tex->texels, t_offset);
            *ptr = rn->blend(*ptr, pf_color_blend_mul(texel, tint));
        })
    }
}

void
pf_renderer2d_texture2d_ex(pf_renderer2d_t *rn, const pf_texture2d_t *tex, int x, int y, float sx, float sy, float r, int ox, int oy)
{
    pf_mat3_t transform;
    pf_mat3_t temp;

    // Initialiser la matrice en identité
    pf_mat3_identity(transform);

    // Traduction pour positionner l'objet à (x, y)
    pf_mat3_translate(temp, x - ox, y - oy);
    pf_mat3_mul(transform, transform, temp);

    // Rotation autour du point (ox, oy)
    pf_mat3_translate(temp, ox, oy);
    pf_mat3_mul(transform, transform, temp);
    pf_mat3_rotate(temp, r);
    pf_mat3_mul(transform, transform, temp);
    pf_mat3_translate(temp, -ox, -oy);
    pf_mat3_mul(transform, transform, temp);

    // Mise à l'échelle par (sx, sy)
    pf_mat3_translate(temp, ox, oy);
    pf_mat3_mul(transform, transform, temp);
    pf_mat3_scale(temp, sx, sy);
    pf_mat3_mul(transform, transform, temp);
    pf_mat3_translate(temp, -ox, -oy);
    pf_mat3_mul(transform, transform, temp);

    // Appel à la fonction ex avec la matrice de transformation complète
    pf_renderer2d_texture2d_mat(rn, tex, transform);
}

void
pf_renderer2d_texture2d_ex_tint(pf_renderer2d_t *rn, const pf_texture2d_t *tex, int x, int y, float sx, float sy, float r, int ox, int oy, pf_color_t tint)
{
    pf_mat3_t transform;
    pf_mat3_t temp;

    // Initialiser la matrice en identité
    pf_mat3_identity(transform);

    // Traduction pour positionner l'objet à (x, y)
    pf_mat3_translate(temp, x - ox, y - oy);
    pf_mat3_mul(transform, transform, temp);

    // Rotation autour du point (ox, oy)
    pf_mat3_translate(temp, ox, oy);
    pf_mat3_mul(transform, transform, temp);
    pf_mat3_rotate(temp, r);
    pf_mat3_mul(transform, transform, temp);
    pf_mat3_translate(temp, -ox, -oy);
    pf_mat3_mul(transform, transform, temp);

    // Mise à l'échelle par (sx, sy)
    pf_mat3_translate(temp, ox, oy);
    pf_mat3_mul(transform, transform, temp);
    pf_mat3_scale(temp, sx, sy);
    pf_mat3_mul(transform, transform, temp);
    pf_mat3_translate(temp, -ox, -oy);
    pf_mat3_mul(transform, transform, temp);

    // Appel à la fonction ex avec la matrice de transformation complète
    pf_renderer2d_texture2d_mat_tint(rn, tex, transform, tint);
}

void
pf_renderer2d_texture2d_ex_map(pf_renderer2d_t *rn, const pf_texture2d_t *tex, int x, int y, float sx, float sy, float r, int ox, int oy, pf_fragment_proc2d_fn frag_proc)
{
    pf_mat3_t transform;
    pf_mat3_t temp;

    // Initialiser la matrice en identité
    pf_mat3_identity(transform);

    // Traduction pour positionner l'objet à (x, y)
    pf_mat3_translate(temp, x - ox, y - oy);
    pf_mat3_mul(transform, transform, temp);

    // Rotation autour du point (ox, oy)
    pf_mat3_translate(temp, ox, oy);
    pf_mat3_mul(transform, transform, temp);
    pf_mat3_rotate(temp, r);
    pf_mat3_mul(transform, transform, temp);
    pf_mat3_translate(temp, -ox, -oy);
    pf_mat3_mul(transform, transform, temp);

    // Mise à l'échelle par (sx, sy)
    pf_mat3_translate(temp, ox, oy);
    pf_mat3_mul(transform, transform, temp);
    pf_mat3_scale(temp, sx, sy);
    pf_mat3_mul(transform, transform, temp);
    pf_mat3_translate(temp, -ox, -oy);
    pf_mat3_mul(transform, transform, temp);

    // Appel à la fonction ex avec la matrice de transformation complète
    pf_renderer2d_texture2d_mat_map(rn, tex, transform, frag_proc);
}

void
pf_renderer2d_texture2d_mat(pf_renderer2d_t* rn, const pf_texture2d_t* tex, pf_mat3_t transform)
{
    if (rn->blend == NULL) {
        PF_TRAVEL_TEXTURE2D_MAT({
            pf_color_t texel = tex->sampler(tex, u, v);
            fb->buffer[y * fb->w + x] = texel;
        })
    } else {
        PF_TRAVEL_TEXTURE2D_MAT({
            pf_color_t* ptr = fb->buffer + y * fb->w + x;
            pf_color_t texel = tex->sampler(tex, u, v);
            *ptr = rn->blend(*ptr, texel);
        })
    }
}

void
pf_renderer2d_texture2d_mat_tint(pf_renderer2d_t* rn, const pf_texture2d_t* tex, pf_mat3_t transform, pf_color_t tint)
{
    if (rn->blend == NULL) {
        PF_TRAVEL_TEXTURE2D_MAT({
            pf_color_t texel = tex->sampler(tex, u, v);
            fb->buffer[y * fb->w + x] = pf_color_blend_mul(texel, tint);
        })
    } else {
        PF_TRAVEL_TEXTURE2D_MAT({
            pf_color_t texel = tex->sampler(tex, u, v);
            pf_color_t* ptr = fb->buffer + y * fb->w + x;
            *ptr = rn->blend(*ptr, pf_color_blend_mul(texel, tint));
        })
    }
}

void
pf_renderer2d_texture2d_mat_map(pf_renderer2d_t* rn, const pf_texture2d_t* tex, pf_mat3_t transform, pf_fragment_proc2d_fn frag_proc)
{
    PF_TRAVEL_TEXTURE2D_MAT({
        pf_vertex2d_t vertex;
        vertex.position[0] = x;
        vertex.position[1] = y;
        vertex.texcoord[0] = u;
        vertex.texcoord[1] = v;
        vertex.color = PF_WHITE;
        frag_proc(rn, &vertex, fb->buffer + y * fb->w + x, tex);
    })
}
