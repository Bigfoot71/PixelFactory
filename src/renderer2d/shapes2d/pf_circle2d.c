#include "pixelfactory/pf_renderer2d.h"
#include "pixelfactory/math/pf_vec2.h"
#include "pixelfactory/pf_stdinc.h"

/* Macros */

#define PF_CIRCLE_TRAVEL(PIXEL_CODE)                                                    \
    int x = 0;                                                                          \
    int y = radius;                                                                     \
    int d = 3 - 2 * radius;                                                             \
    while (y >= x) {                                                                    \
        for (int i = cx - x; i <= cx + x; ++i) {                                        \
            if (i >= 0 && i < (int)rn->fb.w) {                                          \
                if (cy + y >= 0 && cy + y < (int)rn->fb.h) {                            \
                    size_t offset = (cy + y) * rn->fb.w + i;                            \
                    PIXEL_CODE                                                          \
                }                                                                       \
                if (cy - y >= 0 && cy - y < (int)rn->fb.h) {                            \
                    size_t offset = (cy - y) * rn->fb.w + i;                            \
                    PIXEL_CODE                                                          \
                }                                                                       \
            }                                                                           \
        }                                                                               \
        for (int i = cx - y; i <= cx + y; ++i) {                                        \
            if (i >= 0 && i < (int)rn->fb.w) {                                          \
                if (cy + x >= 0 && cy + x < (int)rn->fb.h) {                            \
                    size_t offset = (cy + x) * rn->fb.w + i;                            \
                    PIXEL_CODE                                                          \
                }                                                                       \
                if (cy - x >= 0 && cy - x < (int)rn->fb.h) {                            \
                    size_t offset = (cy - x) * rn->fb.w + i;                            \
                    PIXEL_CODE                                                          \
                }                                                                       \
            }                                                                           \
        }                                                                               \
        ++x;                                                                            \
        if (d > 0) {                                                                    \
            --y;                                                                        \
            d = d + 4 * (x - y) + 10;                                                   \
        } else {                                                                        \
            d = d + 4 * x + 6;                                                          \
        }                                                                               \
    }

#define PF_CIRCLE_TRAVEL_EX(PC_A, PC_B, PC_C, PC_D)                                     \
    int x = 0;                                                                          \
    int y = radius;                                                                     \
    int d = 3 - 2 * radius;                                                             \
    while (y >= x) {                                                                    \
        for (int i = cx - x; i <= cx + x; ++i) {                                        \
            if (i >= 0 && i < (int)rn->fb.w) {                                          \
                if (cy + y >= 0 && cy + y < (int)rn->fb.h) {                            \
                    size_t offset = (cy + y) * rn->fb.w + i;                            \
                    PC_A                                                                \
                }                                                                       \
                if (cy - y >= 0 && cy - y < (int)rn->fb.h) {                            \
                    size_t offset = (cy - y) * rn->fb.w + i;                            \
                    PC_B                                                                \
                }                                                                       \
            }                                                                           \
        }                                                                               \
        for (int i = cx - y; i <= cx + y; ++i) {                                        \
            if (i >= 0 && i < (int)rn->fb.w) {                                          \
                if (cy + x >= 0 && cy + x < (int)rn->fb.h) {                            \
                    size_t offset = (cy + x) * rn->fb.w + i;                            \
                    PC_C                                                                \
                }                                                                       \
                if (cy - x >= 0 && cy - x < (int)rn->fb.h) {                            \
                    size_t offset = (cy - x) * rn->fb.w + i;                            \
                    PC_D                                                                \
                }                                                                       \
            }                                                                           \
        }                                                                               \
        ++x;                                                                            \
        if (d > 0) {                                                                    \
            --y;                                                                        \
            d = d + 4 * (x - y) + 10;                                                   \
        } else {                                                                        \
            d = d + 4 * x + 6;                                                          \
        }                                                                               \
    }

#define PF_CIRCLE_LINE_TRAVEL(PIXEL_CODE)                                               \
    int x = 0;                                                                          \
    int y = radius;                                                                     \
    int d = 3 - 2 * radius;                                                             \
    while (y >= x) {                                                                    \
        int px1 = cx + x, px2 = cx - x;                                                 \
        int py1 = cy + y, py2 = cy - y;                                                 \
        int px3 = cx + y, px4 = cx - y;                                                 \
        int py3 = cy + x, py4 = cy - x;                                                 \
        if (px1 >= 0 && px1 < (int)rn->fb.w) {                                          \
            if (py1 >= 0 && py1 < (int)rn->fb.h) {                                      \
                size_t offset = py1 * rn->fb.w + px1;                                   \
                PIXEL_CODE                                                              \
            }                                                                           \
            if (py2 >= 0 && py2 < (int)rn->fb.h) {                                      \
                size_t offset = py2 * rn->fb.w + px1;                                   \
                PIXEL_CODE                                                              \
            }                                                                           \
        }                                                                               \
        if (px2 >= 0 && px2 < (int)rn->fb.w) {                                          \
            if (py1 >= 0 && py1 < (int)rn->fb.h) {                                      \
                size_t offset = py1 * rn->fb.w + px2;                                   \
                PIXEL_CODE                                                              \
            }                                                                           \
            if (py2 >= 0 && py2 < (int)rn->fb.h) {                                      \
                size_t offset = py2 * rn->fb.w + px2;                                   \
                PIXEL_CODE                                                              \
            }                                                                           \
        }                                                                               \
        if (px3 >= 0 && px3 < (int)rn->fb.w) {                                          \
            if (py3 >= 0 && py3 < (int)rn->fb.h) {                                      \
                size_t offset = py3 * rn->fb.w + px3;                                   \
                PIXEL_CODE                                                              \
            }                                                                           \
            if (py4 >= 0 && py4 < (int)rn->fb.h) {                                      \
                size_t offset = py4 * rn->fb.w + px3;                                   \
                PIXEL_CODE                                                              \
            }                                                                           \
        }                                                                               \
        if (px4 >= 0 && px4 < (int)rn->fb.w) {                                          \
            if (py3 >= 0 && py3 < (int)rn->fb.h) {                                      \
                size_t offset = py3 * rn->fb.w + px4;                                   \
                PIXEL_CODE                                                              \
            }                                                                           \
            if (py4 >= 0 && py4 < (int)rn->fb.h) {                                      \
                size_t offset = py4 * rn->fb.w + px4;                                   \
                PIXEL_CODE                                                              \
            }                                                                           \
        }                                                                               \
        ++x;                                                                            \
        if (d > 0) {                                                                    \
            --y;                                                                        \
            d = d + 4 * (x - y) + 10;                                                   \
        } else {                                                                        \
            d = d + 4 * x + 6;                                                          \
        }                                                                               \
    }


/* Public API */

void
pf_renderer2d_circle(pf_renderer2d_t* rn, int cx, int cy, int radius, pf_color_t color)
{
    if (pf_mat3_is_identity(rn->mat_view) != 0)
    {
        pf_vec2_transform_i(&cx, &cy, cx, cy, rn->mat_view);

        float scale_x = sqrtf(rn->mat_view[0] * rn->mat_view[0] + rn->mat_view[3] * rn->mat_view[3]);
        float scale_y = sqrtf(rn->mat_view[1] * rn->mat_view[1] + rn->mat_view[4] * rn->mat_view[4]);

        float scale = (scale_x + scale_y) / 2.0f;
        radius = (int)(radius * scale + 0.5f);
    }

    if (rn->blend == NULL) {
        PF_CIRCLE_TRAVEL({
            rn->fb.buffer[offset] = color;
        })
    }
    else {
        PF_CIRCLE_TRAVEL({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = rn->blend(*ptr, color);
        })
    }
}

void pf_renderer2d_circle_gradient(pf_renderer2d_t* rn, int cx, int cy, int radius, pf_color_t c1, pf_color_t c2)
{
    if (pf_mat3_is_identity(rn->mat_view) != 0)
    {
        pf_vec2_transform_i(&cx, &cy, cx, cy, rn->mat_view);

        float scale_x = sqrtf(rn->mat_view[0] * rn->mat_view[0] + rn->mat_view[3] * rn->mat_view[3]);
        float scale_y = sqrtf(rn->mat_view[1] * rn->mat_view[1] + rn->mat_view[4] * rn->mat_view[4]);

        float scale = (scale_x + scale_y) / 2.0f;
        radius = (int)(radius * scale + 0.5f);
    }

    if (rn->blend == NULL) {
        PF_CIRCLE_TRAVEL_EX({
            rn->fb.buffer[offset] = pf_color_lerpi(c1, c2,
                sqrtf((i - cx) * (i - cx) + (cy + y - cy) * (cy + y - cy)), radius);
        }, {
            rn->fb.buffer[offset] = pf_color_lerpi(c1, c2,
                sqrtf((i - cx) * (i - cx) + (cy - y - cy) * (cy - y - cy)), radius);
        }, {
            rn->fb.buffer[offset] = pf_color_lerpi(c1, c2,
                sqrtf((i - cx) * (i - cx) + (cy + x - cy) * (cy + x - cy)), radius);
        }, {
            rn->fb.buffer[offset] = pf_color_lerpi(c1, c2,
                sqrtf((i - cx) * (i - cx) + (cy - x - cy) * (cy - x - cy)), radius);
        })
    } else {
        PF_CIRCLE_TRAVEL_EX({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = rn->blend(*ptr, pf_color_lerpi(c1, c2,
                sqrtf((i - cx) * (i - cx) + (cy + y - cy) * (cy + y - cy)), radius));
        }, {
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = rn->blend(*ptr, pf_color_lerpi(c1, c2,
                sqrtf((i - cx) * (i - cx) + (cy - y - cy) * (cy - y - cy)), radius));
        }, {
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = rn->blend(*ptr, pf_color_lerpi(c1, c2,
                sqrtf((i - cx) * (i - cx) + (cy + x - cy) * (cy + x - cy)), radius));
        }, {
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = rn->blend(*ptr, pf_color_lerpi(c1, c2,
                sqrtf((i - cx) * (i - cx) + (cy - x - cy) * (cy - x - cy)), radius));
        })
    }
}

void
pf_renderer2d_circle_map(pf_renderer2d_t* rn, int cx, int cy, int radius, pf_fragment_proc2d_fn frag_proc, const void* attr)
{
    if (pf_mat3_is_identity(rn->mat_view) != 0)
    {
        pf_vec2_transform_i(&cx, &cy, cx, cy, rn->mat_view);

        float scale_x = sqrtf(rn->mat_view[0] * rn->mat_view[0] + rn->mat_view[3] * rn->mat_view[3]);
        float scale_y = sqrtf(rn->mat_view[1] * rn->mat_view[1] + rn->mat_view[4] * rn->mat_view[4]);

        float scale = (scale_x + scale_y) / 2.0f;
        radius = (int)(radius * scale + 0.5f);
    }

    PF_CIRCLE_TRAVEL({
        pf_vertex2d_t vertex;
        vertex.position[0] = x;
        vertex.position[1] = y;
        vertex.texcoord[0] = 0;
        vertex.texcoord[1] = 0;
        vertex.color = PF_WHITE;
        frag_proc(rn, &vertex, rn->fb.buffer + offset, attr);
    })
}

void
pf_renderer2d_circle_lines(pf_renderer2d_t* rn, int cx, int cy, int radius, pf_color_t color)
{
    if (pf_mat3_is_identity(rn->mat_view) != 0)
    {
        pf_vec2_transform_i(&cx, &cy, cx, cy, rn->mat_view);

        float scale_x = sqrtf(rn->mat_view[0] * rn->mat_view[0] + rn->mat_view[3] * rn->mat_view[3]);
        float scale_y = sqrtf(rn->mat_view[1] * rn->mat_view[1] + rn->mat_view[4] * rn->mat_view[4]);

        float scale = (scale_x + scale_y) / 2.0f;
        radius = (int)(radius * scale + 0.5f);
    }

    if (rn->blend == NULL)
    {
        PF_CIRCLE_LINE_TRAVEL({
            rn->fb.buffer[offset] = color;
        })
    }
    else {
        PF_CIRCLE_LINE_TRAVEL({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = rn->blend(*ptr, color);
        })
    }
}

void
pf_renderer2d_circle_lines_map(pf_renderer2d_t* rn, int cx, int cy, int radius, pf_fragment_proc2d_fn frag_proc, const void* attr)
{
    if (pf_mat3_is_identity(rn->mat_view) != 0)
    {
        pf_vec2_transform_i(&cx, &cy, cx, cy, rn->mat_view);

        float scale_x = sqrtf(rn->mat_view[0] * rn->mat_view[0] + rn->mat_view[3] * rn->mat_view[3]);
        float scale_y = sqrtf(rn->mat_view[1] * rn->mat_view[1] + rn->mat_view[4] * rn->mat_view[4]);

        float scale = (scale_x + scale_y) / 2.0f;
        radius = (int)(radius * scale + 0.5f);
    }

    PF_CIRCLE_LINE_TRAVEL({
        pf_vertex2d_t vertex;
        vertex.position[0] = x;
        vertex.position[1] = y;
        vertex.texcoord[0] = 0;
        vertex.texcoord[1] = 0;
        vertex.color = PF_WHITE;
        frag_proc(rn, &vertex, rn->fb.buffer + offset, attr);
    })
}

void
pf_renderer2d_circle_lines_thick(pf_renderer2d_t* rn, int cx, int cy, int radius, int thick, pf_color_t color)
{
    int ht = thick/2;
    for (int i = -ht; i <= ht; i++) {
        pf_renderer2d_circle_lines(rn, cx, cy, radius + i, color);
    }
}

void
pf_renderer2d_circle_lines_thick_map(pf_renderer2d_t* rn, int cx, int cy, int radius, int thick, pf_fragment_proc2d_fn frag_proc, const void* attr)
{
    int ht = thick/2;
    for (int i = -ht; i <= ht; i++) {
        pf_renderer2d_circle_lines_map(rn, cx, cy, radius + i, frag_proc, attr);
    }
}
