#ifndef PF_PROCESSORS_H
#define PF_PROCESSORS_H

#include "pf_vertex.h"

struct pf_renderer2d;

/* Processor 2D Prototypes */

typedef void (*pf_vertex_proc2d_fn)(
    pf_vertex2d_t* out_vertex,
    pf_mat3_t transform,
    const void* attr
);

typedef void (*pf_rasterizer_proc2d_fn)(
    pf_vertex2d_t* out_vertex,
    pf_vertex2d_t* vertex_1,
    pf_vertex2d_t* vertex_2,
    pf_vertex2d_t* vertex_3,
    size_t index_vertex_1,
    size_t index_vertex_2,
    size_t index_vertex_3,
    pf_vec3_t bary,
    void* attr
);

typedef void (*pf_fragment_proc2d_fn)(
    struct pf_renderer2d* rn,
    pf_vertex2d_t* vertex,
    pf_color_t* out_color,
    const void* attr
);

/* Default Processor 2D Functions */

void
pf_proc2d_vertex_default(
    pf_vertex2d_t* out_vertex,
    pf_mat3_t transform,
    const void* attr
);

void
pf_proc2d_rasterizer_default(
    pf_vertex2d_t* out_vertex,
    pf_vertex2d_t* vertex_1,
    pf_vertex2d_t* vertex_2,
    pf_vertex2d_t* vertex_3,
    size_t index_vertex_1,
    size_t index_vertex_2,
    size_t index_vertex_3,
    pf_vec3_t bary,
    void* attr
);

void
pf_proc2d_fragment_default(
    struct pf_renderer2d* rn,
    pf_vertex2d_t* vertex,
    pf_color_t* out_color,
    const void* attr
);

#endif //PF_PROCESSORS_H
