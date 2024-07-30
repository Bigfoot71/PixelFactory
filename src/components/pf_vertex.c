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

#include "pixelfactory/components/pf_vertex.h"
#include <stdint.h>

pf_vertex_t
pf_vertex_create_2d(
    float x, float y,
    float u, float v,
    pf_color_t color)
{
    pf_vertex_t vertex = { 0 };

    vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].comp = 2;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].type = PF_FLOAT;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].value[0].v_float = x;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].value[1].v_float = y;

    vertex.elements[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].comp = 2;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].type = PF_FLOAT;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].value[0].v_float = u;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].value[1].v_float = v;

    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].comp = 4;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].type = PF_UNSIGNED_BYTE;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].value[0].v_uint8_t = color.a[0];
    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].value[1].v_uint8_t = color.a[1];
    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].value[2].v_uint8_t = color.a[2];
    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].value[3].v_uint8_t = color.a[3];

    return vertex;
}

pf_vertex_t
pf_vertex_create_3d(
    float x, float y, float z,
    float u, float v,
    pf_color_t color)
{
    pf_vertex_t vertex = { 0 };
    vertex.num_elements = 3;

    vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].comp = 3;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].type = PF_FLOAT;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].value[0].v_float = x;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].value[1].v_float = y;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].value[2].v_float = z;

    vertex.elements[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].comp = 2;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].type = PF_FLOAT;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].value[0].v_float = u;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].value[1].v_float = v;

    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].comp = 4;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].type = PF_UNSIGNED_BYTE;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].value[0].v_uint8_t = color.a[0];
    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].value[1].v_uint8_t = color.a[1];
    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].value[2].v_uint8_t = color.a[2];
    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].value[3].v_uint8_t = color.a[3];

    return vertex;
}

void
pf_vertex_scale_vec(
    pf_vertex_t* v, int index,
    float scale)
{
#define PF_VERTEX_SCALE_VEC_CASE(TYPE, CTYPE)           \
    case TYPE:                                          \
        for (int_fast8_t i = 0; i < elem->comp; ++i) {  \
            elem->value[i].v_##CTYPE *= scale;          \
        }                                               \
        break;

    pf_attrib_elem_t* elem = &v->elements[index];

    switch (elem->type) {
        PF_VERTEX_SCALE_VEC_CASE(PF_UNSIGNED_BYTE, uint8_t)
        PF_VERTEX_SCALE_VEC_CASE(PF_UNSIGNED_SHORT, uint16_t)
        PF_VERTEX_SCALE_VEC_CASE(PF_UNSIGNED_INT, uint32_t)
        PF_VERTEX_SCALE_VEC_CASE(PF_BYTE, int8_t)
        PF_VERTEX_SCALE_VEC_CASE(PF_SHORT, int16_t)
        PF_VERTEX_SCALE_VEC_CASE(PF_INT, int32_t)
        PF_VERTEX_SCALE_VEC_CASE(PF_FLOAT, float)
    }
}

void
pf_vertex_transform_vec_mat3(
    pf_vertex_t* v, int element_index,
    const pf_mat3_t transform)
{
    switch (v->elements[element_index].comp) {
        case 2: {
            pf_vec2_t position;
            pf_attrib_elem_get_vec(&v->elements[element_index], position);

            pf_vec2_transform(position, position, transform);
            pf_attrib_elem_set_vec(&v->elements[element_index], position);
        } break;
        case 3: {
            pf_vec3_t position;
            pf_attrib_elem_get_vec(&v->elements[element_index], position);

            pf_vec3_transform_mat3(position, position, transform);
            pf_attrib_elem_set_vec(&v->elements[element_index], position);
        } break;
        default:
            break;
    }
}

void
pf_vertex_transform_vec_mat4(
    pf_vertex_t* v, int element_index,
    const pf_mat4_t transform)
{
    switch (v->elements[element_index].comp) {
        case 3: {
            pf_vec3_t position;
            pf_attrib_elem_get_vec(&v->elements[element_index], position);

            pf_vec3_transform(position, position, transform);
            pf_attrib_elem_set_vec(&v->elements[element_index], position);
        } break;
        case 4: {
            pf_vec4_t position;
            pf_attrib_elem_get_vec(&v->elements[element_index], position);

            pf_vec4_transform(position, position, transform);
            pf_attrib_elem_set_vec(&v->elements[element_index], position);
        } break;
        default:
            break;
    }
}

void
pf_vertex_lerp(
    pf_vertex_t* restrict result,
    const pf_vertex_t* restrict start,
    const pf_vertex_t* restrict end,
    float t)
{
    uint32_t e_count = PF_MIN(start->num_elements, end->num_elements);

#ifdef _OPENMP
#   pragma omp simd
#endif //_OPENMP
    for (uint32_t i = 0; i < e_count; ++i) {
        const pf_attrib_elem_t* e_start = &start->elements[i];
        const pf_attrib_elem_t* e_end = &end->elements[i];

        int_fast8_t c_count = PF_MIN(e_start->comp, e_end->comp);

        pf_attrib_elem_t* er = &result->elements[i];
        er->type = e_start->type;
        er->comp = c_count;

        for (int_fast8_t j = 0; j < c_count; ++j) {
            float v_start = pf_attrib_elem_get_comp_f(e_start, j);
            float v_end = pf_attrib_elem_get_comp_f(e_end, j);
            pf_attrib_elem_set_comp_f(er, j, v_start + t * (v_end - v_start));
        }
    }

    result->num_elements = e_count;
}

void
pf_vertex_bary(
    pf_vertex_t* restrict result,
    const pf_vertex_t* restrict v1,
    const pf_vertex_t* restrict v2,
    const pf_vertex_t* restrict v3,
    float w1, float w2, float w3)
{
    uint32_t e_count = PF_MIN(v1->num_elements, v2->num_elements);
    e_count = PF_MIN(e_count, v3->num_elements);

#ifdef _OPENMP
#   pragma omp simd
#endif //_OPENMP
    for (uint32_t i = 0; i < e_count; ++i) {
        const pf_attrib_elem_t* e1 = &v1->elements[i];
        const pf_attrib_elem_t* e2 = &v2->elements[i];
        const pf_attrib_elem_t* e3 = &v3->elements[i];

        int_fast8_t c_count = PF_MIN(e1->comp, e2->comp);
        c_count = PF_MIN(c_count, e3->comp);

        pf_attrib_elem_t* er = &result->elements[i];
        er->type = e1->type;
        er->comp = c_count;

        for (int_fast8_t j = 0; j < c_count; ++j) {
            float val1 = pf_attrib_elem_get_comp_f(e1, j);
            float val2 = pf_attrib_elem_get_comp_f(e2, j);
            float val3 = pf_attrib_elem_get_comp_f(e3, j);
            pf_attrib_elem_set_comp_f(er, j, val1 * w1 + val2 * w2 + val3 * w3);
        }
    }

    result->num_elements = e_count;
}

/* Helper Vertex Buffer Functions */

pf_vertex_buffer_t
pf_vertex_buffer_create_2d(
    uint32_t num_vertices,
    float* positions,
    float* texcoords,
    pf_color_t* colors)
{
    pf_vertex_buffer_t vb = { 0 };

    if (positions != NULL) {
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].buffer = positions;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].size = num_vertices;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].type = PF_FLOAT;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].comp = 2;
        vb.num_attributes++;
    }

    if (texcoords != NULL) {
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].buffer = texcoords;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].size = num_vertices;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].type = PF_FLOAT;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].comp = 2;
        vb.num_attributes++;
    }

    if (colors != NULL) {
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].buffer = colors;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].size = num_vertices;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].type = PF_UNSIGNED_BYTE;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].comp = 4;
        vb.num_attributes++;
    }

    vb.num_vertices = num_vertices;

    return vb;
}

pf_vertex_buffer_t
pf_vertex_buffer_create_3d(
    uint32_t num_vertices,
    float* positions,
    float* texcoords,
    float* normals,
    pf_color_t* colors)
{
    pf_vertex_buffer_t vb = { 0 };

    if (positions != NULL) {
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].buffer = positions;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].size = num_vertices;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].type = PF_FLOAT;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].comp = 3;
        vb.num_attributes++;
    }

    if (texcoords != NULL) {
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].buffer = texcoords;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].size = num_vertices;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].type = PF_FLOAT;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].comp = 2;
        vb.num_attributes++;
    }

    if (normals != NULL) {
        vb.attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX].buffer = normals;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX].size = num_vertices;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX].type = PF_FLOAT;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX].comp = 3;
        vb.num_attributes++;
    }

    if (colors != NULL) {
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].buffer = colors;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].size = num_vertices;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].type = PF_UNSIGNED_BYTE;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].comp = 4;
        vb.num_attributes++;
    }

    vb.num_vertices = num_vertices;

    return vb;
}
