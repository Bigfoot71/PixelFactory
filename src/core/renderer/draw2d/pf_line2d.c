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

#include "pixelfactory/components/pf_color.h"
#include "pixelfactory/core/pf_renderer.h"

/* Macros */

#define PF_LINE_TRAVEL(PIXEL_CODE)                                                      \
    if (pf_clip_line2d_INTERNAL(&x1, &y1, &x2, &y2, 0, 0, rn->fb.w - 1, rn->fb.h - 1) == 0) {    \
        return;                                                                         \
    }                                                                                   \
    int_fast8_t y_longer = 0;                                                           \
    int short_len = y2 - y1;                                                            \
    int long_len = x2 - x1;                                                             \
    /*
        Determine if the line is more vertical than horizontal
    */                                                                                  \
    if (abs(short_len) > abs(long_len)) {                                               \
        PF_SWAP(short_len, long_len);                                                   \
        y_longer = 1;                                                                   \
    }                                                                                   \
    /*
        Initialize variables for drawing loop
    */                                                                                  \
    int end = long_len;                                                                 \
    int_fast8_t sign = 1;                                                               \
    /*
        Adjust direction increment based on long_len sign
    */                                                                                  \
    if (long_len < 0) {                                                                 \
        long_len = -long_len;                                                           \
        sign = -1;                                                                      \
    }                                                                                   \
    /*
        Calculate fixed-point increment for shorter length
    */                                                                                  \
    int dec = (long_len == 0) ? 0 : (short_len << 16) / long_len;                       \
    /*
        Travel the line pixel by pixel
    */                                                                                  \
    if (y_longer) {                                                                     \
        /*
            If line is more vertical, iterate over y-axis
        */                                                                              \
        for (int i = 0, j = 0; i != end; i += sign, j += dec) {                         \
            int x = x1 + (j >> 16), y = y1 + i;                                         \
            size_t offset = y * rn->fb.w + x;                                           \
            PIXEL_CODE                                                                  \
        }                                                                               \
    } else {                                                                            \
        /*
            If line is more horizontal, iterate over x-axis
        */                                                                              \
        for (int i = 0, j = 0; i != end; i += sign, j += dec) {                         \
            int x = x1 + i, y = y1 + (j >> 16);                                         \
            size_t offset = y * rn->fb.w + x;                                           \
            PIXEL_CODE                                                                  \
        }                                                                               \
    }

#define PF_LINE_THICK_TRAVEL(LINE_CODE)                                                 \
    /*
        Calculate differences in x and y coordinates
    */                                                                                  \
    int dx = x2 - x1;                                                                   \
    int dy = y2 - y1;                                                                   \
    /*
        Draw the main line between (x1, y1) and (x2, y2)
    */                                                                                  \
    LINE_CODE                                                                           \
    /*
        Determine if the line is more horizontal or vertical
    */                                                                                  \
    if (dx != 0 && abs(dy/dx) < 1) {                                                    \
        int y1_copy = y1;                                                               \
        int y2_copy = y2;                                                               \
        /*
            Line is more horizontal
            Calculate half the width of the line
        */                                                                              \
        int wy = (thick - 1) * sqrtf((float)(dx*dx + dy*dy)) / (2*abs(dx));             \
        /*
            Draw additional lines above and below the main line
        */                                                                              \
        for (int i = 1; i <= wy; ++i) {                                                 \
            y1 = y1_copy - i;                                                           \
            y2 = y2_copy - i;                                                           \
            {                                                                           \
                LINE_CODE                                                               \
            }                                                                           \
            y1 = y1_copy + i;                                                           \
            y2 = y2_copy + i;                                                           \
            {                                                                           \
                LINE_CODE                                                               \
            }                                                                           \
        }                                                                               \
    } else if (dy != 0) {                                                               \
        int x1_copy = x1;                                                               \
        int x2_copy = x2;                                                               \
        /*
            Line is more vertical or perfectly horizontal
            Calculate half the width of the line
        */                                                                              \
        int wx = (thick - 1) * sqrtf((float)(dx*dx + dy*dy)) / (2*abs(dy));             \
        /*
            Draw additional lines to the left and right of the main line
        */                                                                              \
        for (int i = 1; i <= wx; ++i) {                                                 \
            x1 = x1_copy - i;                                                           \
            x2 = x2_copy - i;                                                           \
            {                                                                           \
                LINE_CODE                                                               \
            }                                                                           \
            x1 = x1_copy + i;                                                           \
            x2 = x2_copy + i;                                                           \
            {                                                                           \
                LINE_CODE                                                               \
            }                                                                           \
        }                                                                               \
    }


/* Internal */

enum pf_clipcode {
    CLIP_INSIDE = 0x00, // 0000
    CLIP_LEFT   = 0x01, // 0001
    CLIP_RIGHT  = 0x02, // 0010
    CLIP_BOTTOM = 0x04, // 0100
    CLIP_TOP    = 0x08, // 1000
};

static uint8_t
pf_encode_point_INTERNAL(int x, int y, int xmin, int ymin, int xmax, int ymax)
{
    uint8_t code = CLIP_INSIDE;
    if (x < xmin) code |= CLIP_LEFT;
    if (x > xmax) code |= CLIP_RIGHT;
    if (y < ymin) code |= CLIP_TOP;
    if (y > ymax) code |= CLIP_BOTTOM;
    return code;
}

static int_fast8_t
pf_clip_line2d_INTERNAL(int* x1, int* y1, int* x2, int* y2, int xmin, int ymin, int xmax, int ymax)
{
    int_fast8_t accept = 0;  // Initialize accept flag to false

    uint8_t code1 = pf_encode_point_INTERNAL(*x1, *y1, xmin, ymin, xmax, ymax);
    uint8_t code2 = pf_encode_point_INTERNAL(*x2, *y2, xmin, ymin, xmax, ymax);

    int dx = *x2 - *x1;
    int dy = *y2 - *y1;

    // Loop until the line is accepted or rejected
    for (;;) {

        // Accepted if both endpoints lie completely within the clipping rectangle
        if ((code1 | code2) == 0) {
            accept = 1;
            break;
        }

        // Rejected if both endpoints lie outside the same clipping region
        if (code1 & code2) break;

        // Determine which point to clip
        uint8_t code_out = code1 ? code1 : code2;
        int x = *x1, y = *y1;

        if (code_out & CLIP_LEFT) {
            if (dx) y += dy * (xmin - x) / dx;
            x = xmin;
        } else if (code_out & CLIP_RIGHT) {
            if (dx) y += dy * (xmax - x) / dx;
            x = xmax;
        } else if (code_out & CLIP_BOTTOM) {
            if (dy) x += dx * (ymax - y) / dy;
            y = ymax;
        } else /* code_out & CLIP_TOP */ {
            if (dy) x += dx * (ymin - y) / dy;
            y = ymin;
        }

        if (code_out == code1) {
            code1 = pf_encode_point_INTERNAL(x, y, xmin, ymin, xmax, ymax);
            *x1 = x, *y1 = y;
        } else {
            code2 = pf_encode_point_INTERNAL(x, y, xmin, ymin, xmax, ymax);
            *x2 = x, *y2 = y;
        }
    }

    return accept;
}


/* Public API */

void
pf_renderer_line2d(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t color)
{
    // Transformation
    if (rn->conf2d != NULL) {
        pf_vec2_transform_i(&x1, &y1, x1, y1, rn->conf2d->mat_view);
        pf_vec2_transform_i(&x2, &y2, x2, y2, rn->conf2d->mat_view);
    }

    // Rendering
    if (rn->conf2d != NULL && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_LINE_TRAVEL({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = blend(*ptr, color);
        })
    }
    else {
        PF_LINE_TRAVEL({
            rn->fb.buffer[offset] = color;
        })
    }
}

void
pf_renderer_line2d_gradient(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t c1,
    pf_color_t c2)
{
    // Transformation
    if (rn->conf2d != NULL) {
        pf_vec2_transform_i(&x1, &y1, x1, y1, rn->conf2d->mat_view);
        pf_vec2_transform_i(&x2, &y2, x2, y2, rn->conf2d->mat_view);
    }

    // Rendering
    if (rn->conf2d != NULL && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_LINE_TRAVEL({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = blend(*ptr, pf_color_lerpi(c1, c2, i, end));
        })
    } else {
        PF_LINE_TRAVEL({
            rn->fb.buffer[offset] = pf_color_lerpi(c1, c2, i, end);
        })
    }
}

void
pf_renderer_line2d_map(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    const pf_proc2d_t* proc)
{
    // Transformation
    if (rn->conf2d != NULL) {
        pf_vec2_transform_i(&x1, &y1, x1, y1, rn->conf2d->mat_view);
        pf_vec2_transform_i(&x2, &y2, x2, y2, rn->conf2d->mat_view);
    }

    // Setup processor
    pf_proc2d_fragment_fn fragment = pf_proc2d_fragment_default;
    const void* uniforms = NULL;
    if (proc != NULL) {
        if (proc->fragment != NULL) fragment = proc->fragment;
        if (proc->uniforms != NULL) uniforms = proc->uniforms;
    }

    // Rendering
    if (rn->conf2d != NULL && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_LINE_TRAVEL({
            pf_vertex_t vertex = pf_vertex_create_2d(x, y, 0, 0, PF_WHITE);
            pf_color_t *ptr = rn->fb.buffer + offset;
            pf_color_t final_color = *ptr;

            fragment(rn, &vertex, &final_color, uniforms);
            *ptr = blend(*ptr, final_color);
        })
    } else {
        PF_LINE_TRAVEL({
            pf_vertex_t vertex = pf_vertex_create_2d(x, y, 0, 0, PF_WHITE);
            pf_color_t *ptr = rn->fb.buffer + offset;
            pf_color_t final_color = *ptr;

            fragment(rn, &vertex, &final_color, uniforms);
            *ptr = final_color;
        })
    }
}

void
pf_renderer_line2d_thick(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick,
    pf_color_t color)
{
    PF_LINE_THICK_TRAVEL({
        pf_renderer_line2d(rn, x1, y1, x2, y2, color);
    })
}

void
pf_renderer_line2d_thick_gradient(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick,
    pf_color_t c1,
    pf_color_t c2)
{
    PF_LINE_THICK_TRAVEL({
        pf_renderer_line2d_gradient(rn, x1, y1, x2, y2, c1, c2);
    })
}

void
pf_renderer_line2d_thick_map(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick,
    const pf_proc2d_t* proc)
{
    PF_LINE_THICK_TRAVEL({
        pf_renderer_line2d_map(rn, x1, y1, x2, y2, proc);
    })
}
