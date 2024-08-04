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

/* Internal Functions Defintions */

void
pf_renderer_line3d_INTERNAL(
    pf_renderer_t* rn, const pf_vertex_t* v1, const pf_vertex_t* v2, float thick,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal,
    const pf_mat4_t mat_mvp, const pf_proc3d_t* proc);


/* Public API Functions */

void
pf_renderer_line(
    pf_renderer_t* rn,
    const pf_vec3_t p1,
    const pf_vec3_t p2,
    pf_color_t color)
{
    if (rn->conf3d == NULL) {
        return;
    }

    const float mat_identity[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    pf_mat4_t mat_mvp;
    pf_mat4_mul(mat_mvp,
        rn->conf3d->mat_view,
        rn->conf3d->mat_proj);

    pf_proc3d_t processor = { 0 };
    processor.vertex = pf_proc3d_vertex_default;
    processor.fragment = pf_proc3d_fragment_default;

    pf_vertex_t vertices[3] = { 0 };

    for (int_fast8_t i = 0; i < 2; ++i) {
        pf_attrib_elem_t* pos = &vertices[i].elements[PF_ATTRIB_POSITION];
        pf_attrib_elem_t* col = &vertices[i].elements[PF_ATTRIB_COLOR];

        pos->used = true, pos->comp = 3, pos->type = PF_ATTRIB_FLOAT;
        col->used = true, col->comp = 4, col->type = PF_ATTRIB_UBYTE;

        for (int_fast8_t i = 0; i < 4; ++i) {
            col->value[i].v_uint8_t = color.a[i];
        }
    }

    vertices[0].elements[PF_ATTRIB_POSITION].value[0].v_float = p1[0];
    vertices[0].elements[PF_ATTRIB_POSITION].value[1].v_float = p1[1];
    vertices[0].elements[PF_ATTRIB_POSITION].value[2].v_float = p1[2];

    vertices[1].elements[PF_ATTRIB_POSITION].value[0].v_float = p2[0];
    vertices[1].elements[PF_ATTRIB_POSITION].value[1].v_float = p2[1];
    vertices[1].elements[PF_ATTRIB_POSITION].value[2].v_float = p2[2];

    pf_renderer_line3d_INTERNAL(rn, &vertices[0], &vertices[1], 0, mat_identity, mat_identity, mat_mvp, &processor);
}

void
pf_renderer_line_thick(
    pf_renderer_t* rn,
    const pf_vec3_t p1,
    const pf_vec3_t p2,
    float thickness,
    pf_color_t color)
{
    if (rn->conf3d == NULL) {
        return;
    }

    const float mat_identity[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    pf_mat4_t mat_mvp;
    pf_mat4_mul(mat_mvp,
        rn->conf3d->mat_view,
        rn->conf3d->mat_proj);

    pf_proc3d_t processor = { 0 };
    processor.vertex = pf_proc3d_vertex_default;
    processor.fragment = pf_proc3d_fragment_default;

    pf_vertex_t vertices[3] = { 0 };

    for (int_fast8_t i = 0; i < 2; ++i) {
        pf_attrib_elem_t* pos = &vertices[i].elements[PF_ATTRIB_POSITION];
        pf_attrib_elem_t* col = &vertices[i].elements[PF_ATTRIB_COLOR];

        pos->used = true, pos->comp = 3, pos->type = PF_ATTRIB_FLOAT;
        col->used = true, col->comp = 4, col->type = PF_ATTRIB_UBYTE;

        for (int_fast8_t i = 0; i < 4; ++i) {
            col->value[i].v_uint8_t = color.a[i];
        }
    }

    vertices[0].elements[PF_ATTRIB_POSITION].value[0].v_float = p1[0];
    vertices[0].elements[PF_ATTRIB_POSITION].value[1].v_float = p1[1];
    vertices[0].elements[PF_ATTRIB_POSITION].value[2].v_float = p1[2];

    vertices[1].elements[PF_ATTRIB_POSITION].value[0].v_float = p2[0];
    vertices[1].elements[PF_ATTRIB_POSITION].value[1].v_float = p2[1];
    vertices[1].elements[PF_ATTRIB_POSITION].value[2].v_float = p2[2];

    pf_renderer_line3d_INTERNAL(rn, &vertices[0], &vertices[1], thickness, mat_identity, mat_identity, mat_mvp, &processor);
}
