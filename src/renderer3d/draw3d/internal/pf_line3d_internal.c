#include "pixelfactory/pf_renderer3d.h"

/* Internal Macros */

#define PF_LINE_TRAVEL_NODEPTH(PIXEL_CODE)                                              \
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
    float inv_end = 1.0f / end;                                                         \
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
            int x = screen_pos[0][0] + (j >> 16), y = screen_pos[0][1] + i;             \
            size_t offset = y * rn->fb.w + x;                                           \
            float t = (float)i * inv_end;                                               \
            float z = z1 + t * (z2 - z1);                                               \
            rn->zb.buffer[offset] = z;                                                  \
            PIXEL_CODE                                                                  \
        }                                                                               \
    } else {                                                                            \
        /*
            If line is more horizontal, iterate over x-axis
        */                                                                              \
        for (int i = 0, j = 0; i != end; i += sign, j += dec) {                         \
            int x = screen_pos[0][0] + i, y = screen_pos[0][1] + (j >> 16);             \
            size_t offset = y * rn->fb.w + x;                                           \
            float t = (float)i * inv_end;                                               \
            float z = z1 + t * (z2 - z1);                                               \
            rn->zb.buffer[offset] = z;                                                  \
            PIXEL_CODE                                                                  \
        }                                                                               \
    }

#define PF_LINE_TRAVEL_DEPTH(PIXEL_CODE)                                                \
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
    float inv_end = 1.0f / end;                                                         \
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
            float t = (float)i * inv_end;                                               \
            float z = z1 + t * (z2 - z1);                                               \
            if (rn->test(rn->zb.buffer[offset], z)) {                                   \
                rn->zb.buffer[offset] = z;                                              \
                PIXEL_CODE                                                              \
            }                                                                           \
        }                                                                               \
    } else {                                                                            \
        /*
            If line is more horizontal, iterate over x-axis
        */                                                                              \
        for (int i = 0, j = 0; i != end; i += sign, j += dec) {                         \
            int x = x1 + i, y = y1 + (j >> 16);                                         \
            size_t offset = y * rn->fb.w + x;                                           \
            float t = (float)i * inv_end;                                               \
            float z = z1 + t * (z2 - z1);                                               \
            if (rn->test(rn->zb.buffer[offset], z)) {                                   \
                rn->zb.buffer[offset] = z;                                              \
                PIXEL_CODE                                                              \
            }                                                                           \
        }                                                                               \
    }

#define PF_LINE_THICK_TRAVEL(C_LINE_CODE, H_LINE_CODE, V_LINE_CODE)                     \
    /*
        Calculate differences in x and y coordinates
    */                                                                                  \
    int dx = screen_pos[1][0] - screen_pos[0][0];                                       \
    int dy = screen_pos[1][1] - screen_pos[0][1];                                       \
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

#define PF_PIXEL_CODE_NOBLEND()                                                         \
    pf_vertex3d_t vertex = pf_vertex3d_lerp_INTERNAL(&vertices[0], &vertices[1], t);    \
    pf_color_t *ptr = rn->fb.buffer + offset;                                           \
    pf_color_t final_color = *ptr;                                                      \
    frag_proc(rn, &vertex, &final_color, attr);                                         \
    *ptr = final_color;                                                                 \

#define PF_PIXEL_CODE_BLEND()                                                           \
    pf_vertex3d_t vertex = pf_vertex3d_lerp_INTERNAL(&vertices[0], &vertices[1], t);    \
    pf_color_t *ptr = rn->fb.buffer + offset;                                           \
    pf_color_t final_color = *ptr;                                                      \
    frag_proc(rn, &vertex, &final_color, attr);                                         \
    *ptr = rn->blend(*ptr, final_color);                                                \


/* Internal Functions Declarations */

// NOTE: Defined in pf_processors.c
pf_vertex3d_t
pf_vertex3d_lerp_INTERNAL(const pf_vertex3d_t* start, const pf_vertex3d_t* end, float t);


/* Internal Rendering Functions */

void
pf_renderer3d_line_INTERNAL(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, float thick,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal, const pf_mat4_t mat_mvp,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
    pf_vertex3d_t vertices[2] = { *v1, *v2 };
    pf_vec4_t homogens[2] = { 0 };
    int screen_pos[2][2] = { 0 };
    size_t num = 1;

    vert_proc(&vertices[0], homogens[0], mat_model, mat_normal, mat_mvp, attr);
    vert_proc(&vertices[1], homogens[1], mat_model, mat_normal, mat_mvp, attr);

    clip_proc(rn, vertices, homogens, &num);
    if (num != 2) return;

    proj_proc(rn, vertices, homogens, num, screen_pos);

    int x1 = screen_pos[0][0];
    int y1 = screen_pos[0][1];
    int x2 = screen_pos[1][0];
    int y2 = screen_pos[1][1];
    float z1 = homogens[0][2];
    float z2 = homogens[1][2];

    if (fabsf(thick) > 1) {
        if (rn->test != NULL) {
            if (rn->blend != NULL) {
                PF_LINE_THICK_TRAVEL({
                    PF_LINE_TRAVEL_DEPTH({
                        PF_PIXEL_CODE_BLEND()
                    })
                }, {
                    int y1 = screen_pos[0][1] - i;
                    int y2 = screen_pos[1][1] - i;
                    {
                        PF_LINE_TRAVEL_DEPTH({
                            PF_PIXEL_CODE_BLEND()
                        })
                    }
                    y1 = screen_pos[0][1] + i;
                    y2 = screen_pos[1][1] + i;
                    {
                        PF_LINE_TRAVEL_DEPTH({
                            PF_PIXEL_CODE_BLEND()
                        })
                    }
                }, {
                    int x1 = screen_pos[0][0] - i;
                    int x2 = screen_pos[1][0] - i;
                    {
                        PF_LINE_TRAVEL_DEPTH({
                            PF_PIXEL_CODE_BLEND()
                        })
                    }
                    x1 = screen_pos[0][0] + i;
                    x2 = screen_pos[1][0] + i;
                    {
                        PF_LINE_TRAVEL_DEPTH({
                            PF_PIXEL_CODE_BLEND()
                        })
                    }
                })
            } else {
                PF_LINE_THICK_TRAVEL({
                    PF_LINE_TRAVEL_DEPTH({
                        PF_PIXEL_CODE_NOBLEND()
                    })
                }, {
                    int y1 = screen_pos[0][1] - i;
                    int y2 = screen_pos[1][1] - i;
                    {
                        PF_LINE_TRAVEL_DEPTH({
                            PF_PIXEL_CODE_NOBLEND()
                        })
                    }
                    y1 = screen_pos[0][1] + i;
                    y2 = screen_pos[1][1] + i;
                    {
                        PF_LINE_TRAVEL_DEPTH({
                            PF_PIXEL_CODE_NOBLEND()
                        })
                    }
                }, {
                    int x1 = screen_pos[0][0] - i;
                    int x2 = screen_pos[1][0] - i;
                    {
                        PF_LINE_TRAVEL_DEPTH({
                            PF_PIXEL_CODE_NOBLEND()
                        })
                    }
                    x1 = screen_pos[0][0] + i;
                    x2 = screen_pos[1][0] + i;
                    {
                        PF_LINE_TRAVEL_DEPTH({
                            PF_PIXEL_CODE_NOBLEND()
                        })
                    }
                })
            }
        } else {
            if (rn->blend != NULL) {
                PF_LINE_THICK_TRAVEL({
                    PF_LINE_TRAVEL_NODEPTH({
                        PF_PIXEL_CODE_BLEND()
                    })
                }, {
                    int y1 = screen_pos[0][1] - i;
                    int y2 = screen_pos[1][1] - i;
                    {
                        PF_LINE_TRAVEL_NODEPTH({
                            PF_PIXEL_CODE_BLEND()
                        })
                    }
                    y1 = screen_pos[0][1] + i;
                    y2 = screen_pos[1][1] + i;
                    {
                        PF_LINE_TRAVEL_NODEPTH({
                            PF_PIXEL_CODE_BLEND()
                        })
                    }
                }, {
                    int x1 = screen_pos[0][0] - i;
                    int x2 = screen_pos[1][0] - i;
                    {
                        PF_LINE_TRAVEL_NODEPTH({
                            PF_PIXEL_CODE_BLEND()
                        })
                    }
                    x1 = screen_pos[0][0] + i;
                    x2 = screen_pos[1][0] + i;
                    {
                        PF_LINE_TRAVEL_NODEPTH({
                            PF_PIXEL_CODE_BLEND()
                        })
                    }
                })
            } else {
                PF_LINE_THICK_TRAVEL({
                    PF_LINE_TRAVEL_NODEPTH({
                        PF_PIXEL_CODE_NOBLEND()
                    })
                }, {
                    int y1 = screen_pos[0][1] - i;
                    int y2 = screen_pos[1][1] - i;
                    {
                        PF_LINE_TRAVEL_NODEPTH({
                            PF_PIXEL_CODE_NOBLEND()
                        })
                    }
                    y1 = screen_pos[0][1] + i;
                    y2 = screen_pos[1][1] + i;
                    {
                        PF_LINE_TRAVEL_NODEPTH({
                            PF_PIXEL_CODE_NOBLEND()
                        })
                    }
                }, {
                    int x1 = screen_pos[0][0] - i;
                    int x2 = screen_pos[1][0] - i;
                    {
                        PF_LINE_TRAVEL_NODEPTH({
                            PF_PIXEL_CODE_NOBLEND()
                        })
                    }
                    x1 = screen_pos[0][0] + i;
                    x2 = screen_pos[1][0] + i;
                    {
                        PF_LINE_TRAVEL_NODEPTH({
                            PF_PIXEL_CODE_NOBLEND()
                        })
                    }
                })
            }
        }
    } else {
        if (rn->test != NULL) {
            if (rn->blend != NULL) {
                PF_LINE_TRAVEL_DEPTH({
                    PF_PIXEL_CODE_BLEND()
                })
            } else {
                PF_LINE_TRAVEL_DEPTH({
                    PF_PIXEL_CODE_NOBLEND()
                })
            }
        } else {
            if (rn->blend != NULL) {
                PF_LINE_TRAVEL_NODEPTH({
                    PF_PIXEL_CODE_BLEND()
                })
            } else {
                PF_LINE_TRAVEL_NODEPTH({
                    PF_PIXEL_CODE_NOBLEND()
                })
            }
        }
    }
}
