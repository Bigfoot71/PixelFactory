#include "pixelfactory/pf_renderer3d.h"

/* Internal Macros */

#define PF_POINT_THICK_TRAVEL_NODEPTH(PIXEL_CODE)                               \
    float rSq = radius*radius;                                                  \
    for (int y = -radius; y <= radius; y++) {                                   \
        uint32_t py = screen_pos[1] + y;                                        \
        for (int x = -radius; x <= radius; x++) {                               \
            if (y*y + x*x <= rSq){                                              \
                uint32_t px = screen_pos[0] + x;                                \
                size_t offset = py * rn->fb.w + px;                             \
                if (px < rn->fb.w && py < rn->fb.h) {                           \
                    rn->zb.buffer[offset] = homogen[2];                         \
                    PIXEL_CODE                                                  \
                }                                                               \
            }                                                                   \
        }                                                                       \
    }

#define PF_POINT_THICK_TRAVEL_DEPTH(PIXEL_CODE)                                 \
    float rSq = radius*radius;                                                  \
    for (int y = -radius; y <= radius; y++) {                                   \
        uint32_t py = screen_pos[1] + y;                                        \
        for (int x = -radius; x <= radius; x++) {                               \
            if (y*y + x*x <= rSq){                                              \
                uint32_t px = screen_pos[0] + x;                                \
                size_t offset = py * rn->fb.w + px;                             \
                if (px < rn->fb.w && py < rn->fb.h) {                           \
                    if (rn->test(rn->zb.buffer[offset], homogen[2])) {          \
                        rn->zb.buffer[offset] = homogen[2];                     \
                        PIXEL_CODE                                              \
                    }                                                           \
                }                                                               \
            }                                                                   \
        }                                                                       \
    }


/* Public API Functions */

void
pf_renderer3d_point_INTERNAL(
    pf_renderer3d_t* rn, const pf_vertex3d_t* point, float radius,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal, const pf_mat4_t mat_mvp,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr)
{
    pf_vertex3d_t vertex = *point;
    pf_vec4_t homogen = { 0 };
    int screen_pos[2] = { 0 };
    size_t num = 1;

    vert_proc(&vertex, homogen, mat_model, mat_normal, mat_mvp, attr);
    clip_proc(rn, &vertex, &homogen, &num);
    proj_proc(rn, &vertex, &homogen, num, &screen_pos);

    if (radius == 0) {
        size_t offset = screen_pos[1] * rn->fb.w + screen_pos[0];
        if (rn->test != NULL && rn->test(rn->zb.buffer[offset], homogen[2])) {
            frag_proc(rn, &vertex, rn->fb.buffer + offset, attr);
            rn->zb.buffer[offset] = homogen[2];
        } else {
            frag_proc(rn, &vertex, rn->fb.buffer + offset, attr);
        }
    } else {
        if (rn->test != NULL) {
            PF_POINT_THICK_TRAVEL_DEPTH({
                frag_proc(rn, &vertex, rn->fb.buffer + offset, attr);
            })
        } else {
            PF_POINT_THICK_TRAVEL_NODEPTH({
                frag_proc(rn, &vertex, rn->fb.buffer + offset, attr);
            })
        }
    }
}
