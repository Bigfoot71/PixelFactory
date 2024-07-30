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

#include "pixelfactory/core/pf_renderer3d.h"
#include <stdint.h>

/* Internal Functions Declarations */

void
pf_renderer3d_triangle_INTERNAL(
    pf_renderer3d_t* rn, pf_vertex_t vertices[PF_MAX_CLIPPED_POLYGON_VERTICES],
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal,
    const pf_mat4_t mat_mvp, const pf_proc3d_t* proc);

void
pf_renderer3d_point_INTERNAL(
    pf_renderer3d_t* rn, const pf_vertex_t* point, float radius,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal,
    const pf_mat4_t mat_mvp, const pf_proc3d_t* proc);

void
pf_renderer3d_line_INTERNAL(
    pf_renderer3d_t* rn, const pf_vertex_t* v1, const pf_vertex_t* v2, float thick,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal,
    const pf_mat4_t mat_mvp, const pf_proc3d_t* proc);


/* Public API Functions */

void
pf_renderer3d_vertex_buffer(
    pf_renderer3d_t* rn, const pf_vertex_buffer_t* vb,
    const pf_mat4_t transform, const pf_proc3d_t* proc)
{
    pf_renderer3d_vertex_buffer_ex(rn, vb, transform, proc);
}

void
pf_renderer3d_vertex_buffer_ex(
    pf_renderer3d_t* rn, const pf_vertex_buffer_t* vb,
    const pf_mat4_t transform, const pf_proc3d_t* proc)
{
    /* Preparation of matrices */

    pf_mat4_t mat_model;
    pf_mat4_t mat_normal;
    if (transform == NULL) {
        pf_mat4_identity(mat_model);
        pf_mat4_identity(mat_normal);
    } else {
        pf_mat4_copy(mat_model, transform);
        pf_mat4_inverse(mat_normal, mat_model);
        pf_mat4_transpose(mat_normal, mat_normal);
    }

    pf_mat4_t mat_mvp;
    pf_mat4_mul_r(mat_mvp, mat_model, rn->mat_view);
    pf_mat4_mul(mat_mvp, mat_mvp, rn->mat_proj);

    /* Setup processors */

    pf_proc3d_t processor = { 0 };
    processor.vertex = pf_proc3d_vertex_default;
    processor.fragment = pf_proc3d_fragment_default;

    if (proc != NULL) {
        if (proc->vertex != NULL) processor.vertex = proc->vertex;
        if (proc->fragment != NULL) processor.fragment = proc->fragment;
        if (proc->uniforms != NULL) processor.uniforms = proc->uniforms;
    }

    /* Iterates through all vertices in the vertex buffer */

    const uint16_t* indices = vb->indices;
    bool has_indices = (indices != NULL);
    uint32_t num = (has_indices) ? vb->num_indices : vb->num_vertices;

    for (uint32_t i = 0; i < num; i += 3) {

        pf_vertex_t vertices[PF_MAX_CLIPPED_POLYGON_VERTICES];

        uint32_t index_1 = i + 0;
        uint32_t index_2 = i + 1;
        uint32_t index_3 = i + 2;

        if (has_indices) {
            index_1 = indices[index_1];
            index_2 = indices[index_2];
            index_3 = indices[index_3];
        }

        for (uint32_t j = 0; j < PF_MAX_ATTRIBUTES; ++j) {
            const pf_attribute_t* attr = &vb->attributes[j];
            if (attr->used != 0) {
#               define PF_GET_ATTRIB_ELEM(TYPE, CTYPE)                                                                      \
                    case TYPE: {                                                                                            \
                        for (int_fast8_t k = 0; k < attr->comp; ++k) {                                                      \
                            vertices[0].elements[j].value[k].v_##CTYPE = ((CTYPE*)attr->buffer)[index_1 * attr->comp + k];  \
                            vertices[1].elements[j].value[k].v_##CTYPE = ((CTYPE*)attr->buffer)[index_2 * attr->comp + k];  \
                            vertices[2].elements[j].value[k].v_##CTYPE = ((CTYPE*)attr->buffer)[index_3 * attr->comp + k];  \
                        }                                                                                                   \
                    } break;
                switch (attr->type) {
                    PF_GET_ATTRIB_ELEM(PF_UNSIGNED_BYTE, uint8_t)
                    PF_GET_ATTRIB_ELEM(PF_BYTE, int8_t)
                    PF_GET_ATTRIB_ELEM(PF_UNSIGNED_SHORT, uint16_t)
                    PF_GET_ATTRIB_ELEM(PF_SHORT, int16_t)
                    PF_GET_ATTRIB_ELEM(PF_UNSIGNED_INT, uint32_t)
                    PF_GET_ATTRIB_ELEM(PF_INT, int32_t)
                    PF_GET_ATTRIB_ELEM(PF_FLOAT, float)
                    default:
                        break;
                }
#               undef PF_GET_ATTRIB_ELEM
                for (int_fast8_t k = 0; k < 3; ++k) {
                    vertices[k].elements[j].type = attr->type;
                    vertices[k].elements[j].comp = attr->comp;
                    vertices[k].elements[j].used = attr->used;
                }
            }
        }

        pf_renderer3d_triangle_INTERNAL(
            rn, vertices, mat_model, mat_normal,
            mat_mvp, &processor);
    }
}

void
pf_renderer3d_vertex_buffer_points(
    pf_renderer3d_t* rn, const pf_vertex_buffer_t* vb,
    const pf_mat4_t transform, const pf_proc3d_t* proc)
{
    pf_renderer3d_vertex_buffer_points_thick(rn, vb, 0, transform, proc);
}

void
pf_renderer3d_vertex_buffer_points_thick(
    pf_renderer3d_t* rn, const pf_vertex_buffer_t* vb, float radius,
    const pf_mat4_t transform, const pf_proc3d_t* proc)
{
    /* Preparation of matrices */

    pf_mat4_t mat_model;
    pf_mat4_t mat_normal;
    if (transform == NULL) {
        pf_mat4_identity(mat_model);
        pf_mat4_identity(mat_normal);
    } else {
        pf_mat4_copy(mat_model, transform);
        pf_mat4_inverse(mat_normal, mat_model);
        pf_mat4_transpose(mat_normal, mat_normal);
    }

    pf_mat4_t mat_mvp;
    pf_mat4_mul_r(mat_mvp, mat_model, rn->mat_view);
    pf_mat4_mul(mat_mvp, mat_mvp, rn->mat_proj);

    /* Setup processors */

    pf_proc3d_t processor = { 0 };
    processor.vertex = pf_proc3d_vertex_default;
    processor.fragment = pf_proc3d_fragment_default;

    if (proc != NULL) {
        if (proc->vertex != NULL) processor.vertex = proc->vertex;
        if (proc->fragment != NULL) processor.fragment = proc->fragment;
        if (proc->uniforms != NULL) processor.uniforms = proc->uniforms;
    }

    /* Iterates through all vertices in the vertex buffer */

    const uint16_t* indices = vb->indices;
    bool has_indices = (indices != NULL);
    uint32_t num = (has_indices) ? vb->num_indices : vb->num_vertices;

    for (uint32_t i = 0; i < num; i++) {
        uint32_t index = (has_indices) ? indices[i] : i;

        pf_vertex_t vertex = { 0 };

        for (uint32_t j = 0; j < PF_MAX_ATTRIBUTES; ++j) {
            const pf_attribute_t* attr = &vb->attributes[j];
            if (attr->used != 0) {
                vertex.elements[j] = pf_attribute_get_elem(attr, index);
            }
        }

        pf_renderer3d_point_INTERNAL(
            rn, &vertex, radius, mat_model, mat_normal, mat_mvp, &processor);
    }
}

void
pf_renderer3d_vertex_buffer_lines(
    pf_renderer3d_t* rn, const pf_vertex_buffer_t* vb,
    const pf_mat4_t transform, const pf_proc3d_t* proc)
{
    pf_renderer3d_vertex_buffer_lines_thick(rn, vb, 0, transform, proc);
}

void
pf_renderer3d_vertex_buffer_lines_thick(
    pf_renderer3d_t* rn, const pf_vertex_buffer_t* vb, float thick,
    const pf_mat4_t transform, const pf_proc3d_t* proc)
{
    /* Preparation of matrices */

    pf_mat4_t mat_model;
    pf_mat4_t mat_normal;
    if (transform == NULL) {
        pf_mat4_identity(mat_model);
        pf_mat4_identity(mat_normal);
    } else {
        pf_mat4_copy(mat_model, transform);
        pf_mat4_inverse(mat_normal, mat_model);
        pf_mat4_transpose(mat_normal, mat_normal);
    }

    pf_mat4_t mat_mvp;
    pf_mat4_mul_r(mat_mvp, mat_model, rn->mat_view);
    pf_mat4_mul(mat_mvp, mat_mvp, rn->mat_proj);

    /* Setup processors */

    pf_proc3d_t processor = { 0 };
    processor.vertex = pf_proc3d_vertex_default;
    processor.fragment = pf_proc3d_fragment_default;

    if (proc != NULL) {
        if (proc->vertex != NULL) processor.vertex = proc->vertex;
        if (proc->fragment != NULL) processor.fragment = proc->fragment;
        if (proc->uniforms != NULL) processor.uniforms = proc->uniforms;
    }

    /* Iterates through all vertices in the vertex buffer */

    const uint16_t* indices = vb->indices;
    bool has_indices = (indices != NULL);
    uint32_t num = (has_indices) ? vb->num_indices : vb->num_vertices;

    for (uint32_t i = 0; i < num; i += 3) {
        uint32_t tri_indices[3];

        if (has_indices) {
            tri_indices[0] = indices[i + 0];
            tri_indices[1] = indices[i + 1];
            tri_indices[2] = indices[i + 2];
        } else {
            tri_indices[0] = i + 0;
            tri_indices[1] = i + 1;
            tri_indices[2] = i + 2;
        }

        for (uint32_t j = 0; j < 3; ++j) {
            uint32_t index_1 = tri_indices[j];
            uint32_t index_2 = tri_indices[(j + 1) % 3];

            pf_vertex_t v1 = { 0 };
            pf_vertex_t v2 = { 0 };

            for (uint32_t j = 0; j < PF_MAX_ATTRIBUTES; ++j) {
                const pf_attribute_t* attr = &vb->attributes[j];
                if (attr->used != 0) {
                    v1.elements[j] = pf_attribute_get_elem(attr, index_1);
                    v2.elements[j] = pf_attribute_get_elem(attr, index_2);
                }
            }

            pf_renderer3d_line_INTERNAL(
                rn, &v1, &v2, thick, mat_model, mat_normal, mat_mvp, &processor);
        }
    }
}
