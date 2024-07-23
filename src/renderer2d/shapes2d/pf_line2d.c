#include "pixelfactory/math/pf_vec2.h"
#include "pixelfactory/pf_renderer2d.h"
#include "pixelfactory/pf_stdinc.h"

/* Macros */

#define PF_LINE_TRAVEL(PIXEL_CODE)                                                      \
    if (pf_clip_line2d(&x1, &y1, &x2, &y2, 0, 0, rn->fb.w - 1, rn->fb.h - 1) == 0) {    \
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
            PIXEL_CODE                                                                  \
        }                                                                               \
    } else {                                                                            \
        /*
            If line is more horizontal, iterate over x-axis
        */                                                                              \
        for (int i = 0, j = 0; i != end; i += sign, j += dec) {                         \
            int x = x1 + i, y = y1 + (j >> 16);                                         \
            PIXEL_CODE                                                                  \
        }                                                                               \
    }

#define PF_LINE_THICK_TRAVEL(C_LINE_CODE, H_LINE_CODE, V_LINE_CODE)                     \
    /*
        Calculate differences in x and y coordinates
    */                                                                                  \
    int dx = x2 - x1;                                                                   \
    int dy = y2 - y1;                                                                   \
    /*
        Draw the main line between (x1, y1) and (x2, y2)
    */                                                                                  \
    C_LINE_CODE                                                                         \
    /*
        Determine if the line is more horizontal or vertical
    */                                                                                  \
    if (dx != 0 && abs(dy/dx) < 1)                                                      \
    {                                                                                   \
        /*
            Line is more horizontal
            Calculate half the width of the line
        */                                                                              \
        int wy = (thick - 1)*(int)sqrtf((float)(dx*dx + dy*dy))/(2*abs(dx));            \
        /*
            Draw additional lines above and below the main line
        */                                                                              \
        for (int i = 1; i <= wy; ++i)                                                   \
        {                                                                               \
            H_LINE_CODE                                                                 \
        }                                                                               \
    }                                                                                   \
    else if (dy != 0)                                                                   \
    {                                                                                   \
        /*
            Line is more vertical or perfectly horizontal
            Calculate half the width of the line
        */                                                                              \
        int wx = (thick - 1)*(int)sqrtf((float)(dx*dx + dy*dy))/(2*abs(dy));            \
        /*
            Draw additional lines to the left and right of the main line
        */                                                                              \
        for (int i = 1; i <= wx; ++i)                                                   \
        {                                                                               \
            V_LINE_CODE                                                                 \
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
pf_encode_point(int x, int y, int xmin, int ymin, int xmax, int ymax)
{
    uint8_t code = CLIP_INSIDE;
    if (x < xmin) code |= CLIP_LEFT;
    if (x > xmax) code |= CLIP_RIGHT;
    if (y < ymin) code |= CLIP_TOP;
    if (y > ymax) code |= CLIP_BOTTOM;
    return code;
}

static int_fast8_t
pf_clip_line2d(int* x1, int* y1, int* x2, int* y2, int xmin, int ymin, int xmax, int ymax)
{
    int_fast8_t accept = 0;  // Initialize accept flag to false

    uint8_t code1 = pf_encode_point(*x1, *y1, xmin, ymin, xmax, ymax);
    uint8_t code2 = pf_encode_point(*x2, *y2, xmin, ymin, xmax, ymax);

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
            code1 = pf_encode_point(x, y, xmin, ymin, xmax, ymax);
            *x1 = x, *y1 = y;
        } else {
            code2 = pf_encode_point(x, y, xmin, ymin, xmax, ymax);
            *x2 = x, *y2 = y;
        }
    }

    return accept;
}


/* Public API */

void
pf_renderer2d_line(pf_renderer2d_t* rn, int x1, int y1, int x2, int y2, pf_color_t color)
{
    pf_vec2_transform_i(&x1, &y1, x1, y1, rn->mat_view);
    pf_vec2_transform_i(&x2, &y2, x2, y2, rn->mat_view);

    if (rn->blend == NULL) {
        PF_LINE_TRAVEL({
            rn->fb.buffer[y * rn->fb.w + x] = color;
        })
    }
    else {
        PF_LINE_TRAVEL({
            pf_color_t* ptr = rn->fb.buffer + y * rn->fb.w + x;
            *ptr = rn->blend(*ptr, color);
        })
    }
}

void
pf_renderer2d_line_gradient(pf_renderer2d_t* rn, int x1, int y1, int x2, int y2, pf_color_t c1, pf_color_t c2)
{
    pf_vec2_transform_i(&x1, &y1, x1, y1, rn->mat_view);
    pf_vec2_transform_i(&x2, &y2, x2, y2, rn->mat_view);

    if (rn->blend == NULL) {
        PF_LINE_TRAVEL({
            rn->fb.buffer[y * rn->fb.w + x] = pf_color_lerpi(c1, c2, i, end);
        })
    } else {
        PF_LINE_TRAVEL({
            pf_color_t* ptr = rn->fb.buffer + y * rn->fb.w + x;
            *ptr = rn->blend(*ptr, pf_color_lerpi(c1, c2, i, end));
        })
    }
}

void
pf_renderer2d_line_map(pf_renderer2d_t* rn, int x1, int y1, int x2, int y2, pf_proc2d_fragment_fn frag_proc, const void* attr)
{
    pf_vec2_transform_i(&x1, &y1, x1, y1, rn->mat_view);
    pf_vec2_transform_i(&x2, &y2, x2, y2, rn->mat_view);

    PF_LINE_TRAVEL({
        pf_vertex2d_t vertex;
        vertex.position[0] = x;
        vertex.position[1] = y;
        vertex.texcoord[0] = 0;
        vertex.texcoord[1] = 0;
        vertex.color = PF_WHITE;
        frag_proc(rn, &vertex, rn->fb.buffer + y * rn->fb.w + x, attr);
    })
}

void
pf_renderer2d_line_thick(pf_renderer2d_t* rn, int x1, int y1, int x2, int y2, int thick, pf_color_t color)
{
    pf_vec2_transform_i(&x1, &y1, x1, y1, rn->mat_view);
    pf_vec2_transform_i(&x2, &y2, x2, y2, rn->mat_view);

    PF_LINE_THICK_TRAVEL({
        pf_renderer2d_line(rn, x1, y1, x2, y2, color);
    }, {
        pf_renderer2d_line(rn, x1, y1 - i, x2, y2 - i, color);
        pf_renderer2d_line(rn, x1, y1 + i, x2, y2 + i, color);
    }, {
        pf_renderer2d_line(rn, x1 - i, y1, x2 - i, y2, color);
        pf_renderer2d_line(rn, x1 + i, y1, x2 + i, y2, color);
    })
}

void
pf_renderer2d_line_thick_gradient(pf_renderer2d_t* rn, int x1, int y1, int x2, int y2, int thick, pf_color_t c1, pf_color_t c2)
{
    pf_vec2_transform_i(&x1, &y1, x1, y1, rn->mat_view);
    pf_vec2_transform_i(&x2, &y2, x2, y2, rn->mat_view);

    PF_LINE_THICK_TRAVEL({
        pf_renderer2d_line_gradient(rn, x1, y1, x2, y2, c1, c2);
    }, {
        pf_renderer2d_line_gradient(rn, x1, y1 - i, x2, y2 - i, c1, c2);
        pf_renderer2d_line_gradient(rn, x1, y1 + i, x2, y2 + i, c1, c2);
    }, {
        pf_renderer2d_line_gradient(rn, x1 - i, y1, x2 - i, y2, c1, c2);
        pf_renderer2d_line_gradient(rn, x1 + i, y1, x2 + i, y2, c1, c2);
    })
}

void
pf_renderer2d_line_thick_map(pf_renderer2d_t* rn, int x1, int y1, int x2, int y2, int thick, pf_proc2d_fragment_fn frag_proc, const void* attr)
{
    pf_vec2_transform_i(&x1, &y1, x1, y1, rn->mat_view);
    pf_vec2_transform_i(&x2, &y2, x2, y2, rn->mat_view);

    PF_LINE_THICK_TRAVEL({
        pf_renderer2d_line_map(rn, x1, y1, x2, y2, frag_proc, attr);
    }, {
        pf_renderer2d_line_map(rn, x1, y1 - i, x2, y2 - i, frag_proc, attr);
        pf_renderer2d_line_map(rn, x1, y1 + i, x2, y2 + i, frag_proc, attr);
    }, {
        pf_renderer2d_line_map(rn, x1 - i, y1, x2 - i, y2, frag_proc, attr);
        pf_renderer2d_line_map(rn, x1 + i, y1, x2 + i, y2, frag_proc, attr);
    })
}
