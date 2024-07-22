#include "pixelfactory/pf_processors.h"
#include "pixelfactory/pf_renderer2d.h"
#include "pixelfactory/math/pf_vec2.h"
#include "pixelfactory/pf_color.h"

/* Default Processor 2D Functions */

void
pf_proc2d_vertex_default(pf_vertex2d_t* out_vertex, pf_mat3_t transform, const void* attr)
{
    (void)attr;

    pf_vec2_transform(out_vertex->position, out_vertex->position, transform);
}

void
pf_proc2d_rasterizer_default(pf_vertex2d_t* out_vertex, pf_vertex2d_t* vertex_1, pf_vertex2d_t* vertex_2, pf_vertex2d_t* vertex_3, size_t index_vertex_1, size_t index_vertex_2, size_t index_vertex_3, pf_vec3_t bary, void* attr)
{
    (void)attr;
    (void)index_vertex_1;
    (void)index_vertex_2;
    (void)index_vertex_3;

    pf_vec2_bary_v_r(out_vertex->position, vertex_1->position, vertex_2->position, vertex_3->position, bary);
    pf_vec2_bary_v_r(out_vertex->texcoord, vertex_1->texcoord, vertex_2->texcoord, vertex_3->texcoord, bary);
    out_vertex->color = pf_color_bary_v(vertex_1->color, vertex_2->color, vertex_3->color, bary);
}

void
pf_proc2d_fragment_default(struct pf_renderer2d* rn, pf_vertex2d_t* vertex, pf_color_t* out_color, const void* attr)
{
    (void)attr;

    *out_color = (rn->blend == NULL) ? vertex->color : rn->blend(*out_color, vertex->color);
}
