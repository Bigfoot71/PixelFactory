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

pf_vertex_t
pf_vertex_create_2d(
    float x, float y,
    float u, float v,
    pf_color_t color)
{
    pf_vertex_t vertex = { 0 };

    // Position
    pf_attrib_elem_t* pos = &vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX];
    pos->used = true;
    pos->comp = 2;
    pos->type = PF_FLOAT;
    pos->value[0].v_float = x;
    pos->value[1].v_float = y;

    // TexCoord
    pf_attrib_elem_t* tex = &vertex.elements[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX];
    tex->used = true;
    tex->comp = 2;
    tex->type = PF_FLOAT;
    tex->value[0].v_float = u;
    tex->value[1].v_float = v;

    // Color
    pf_attrib_elem_t* col = &vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX];
    col->used = true;
    col->comp = 4;
    col->type = PF_UNSIGNED_BYTE;
    for (int_fast8_t i = 0; i < 4; ++i) {
        col->value[i].v_uint8_t = color.a[i];
    }

    return vertex;
}

pf_vertex_t
pf_vertex_create_3d(
    float x, float y, float z,
    float u, float v,
    pf_color_t color)
{
    pf_vertex_t vertex = { 0 };

    // Position
    pf_attrib_elem_t* pos = &vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX];
    pos->used = true;
    pos->comp = 3;
    pos->type = PF_FLOAT;
    pos->value[0].v_float = x;
    pos->value[1].v_float = y;
    pos->value[2].v_float = z;

    // TexCoord
    pf_attrib_elem_t* tex = &vertex.elements[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX];
    tex->used = true;
    tex->comp = 2;
    tex->type = PF_FLOAT;
    tex->value[0].v_float = u;
    tex->value[1].v_float = v;

    // Color
    pf_attrib_elem_t* col = &vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX];
    col->used = true;
    col->comp = 4;
    col->type = PF_UNSIGNED_BYTE;
    for (int_fast8_t i = 0; i < 4; ++i) {
        col->value[i].v_uint8_t = color.a[i];
    }

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
            pf_vec2_t vec;
            pf_attrib_elem_get_vec(&v->elements[element_index], vec);

            pf_vec2_transform(vec, vec, transform);
            pf_attrib_elem_set_vec(&v->elements[element_index], vec);
        } break;
        case 3: {
            pf_vec3_t vec;
            pf_attrib_elem_get_vec(&v->elements[element_index], vec);

            pf_vec3_transform_mat3(vec, vec, transform);
            pf_attrib_elem_set_vec(&v->elements[element_index], vec);
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
            pf_vec3_t vec;
            pf_attrib_elem_get_vec(&v->elements[element_index], vec);

            pf_vec3_transform(vec, vec, transform);
            pf_attrib_elem_set_vec(&v->elements[element_index], vec);
        } break;
        case 4: {
            pf_vec4_t vec;
            pf_attrib_elem_get_vec(&v->elements[element_index], vec);

            pf_vec4_transform(vec, vec, transform);
            pf_attrib_elem_set_vec(&v->elements[element_index], vec);
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
#if defined(_OPENMP)
#define PF_VERTEX_LERP_CASE(TYPE, CTYPE)                        \
    case TYPE:                                                  \
        _Pragma("omp simd")                                     \
        for (int_fast8_t j = 0; j < c_count; ++j) {             \
            CTYPE val1 = e1->value[j].v_##CTYPE;                \
            CTYPE val2 = e2->value[j].v_##CTYPE;                \
            er->value[j].v_##CTYPE = val1 + t * (val2 - val1);  \
        }                                                       \
    break;
#else
#define PF_VERTEX_LERP_CASE(TYPE, CTYPE)                        \
    case TYPE:                                                  \
        for (int_fast8_t j = 0; j < c_count; ++j) {             \
            CTYPE val1 = e1->value[j].v_##CTYPE;                \
            CTYPE val2 = e2->value[j].v_##CTYPE;                \
            er->value[j].v_##CTYPE = val1 + t * (val2 - val1);  \
        }                                                       \
    break;
#endif

    for (uint32_t i = 0; i < PF_MAX_ATTRIBUTES; ++i) {
        const pf_attrib_elem_t* e1 = &start->elements[i];
        const pf_attrib_elem_t* e2 = &end->elements[i];
        pf_attrib_elem_t* er = &result->elements[i];
        er->used = (e1->used && e2->used);

        if (er->used) {
            int_fast8_t c_count = PF_MIN(e1->comp, e2->comp);
            er->type = e1->type;
            er->comp = c_count;

            switch (er->type) {
                PF_VERTEX_LERP_CASE(PF_FLOAT, float)
                PF_VERTEX_LERP_CASE(PF_UNSIGNED_BYTE, uint8_t)
                PF_VERTEX_LERP_CASE(PF_SHORT, int16_t)
                PF_VERTEX_LERP_CASE(PF_INT, int32_t)
                PF_VERTEX_LERP_CASE(PF_UNSIGNED_SHORT, uint16_t)
                PF_VERTEX_LERP_CASE(PF_UNSIGNED_INT, uint32_t)
                PF_VERTEX_LERP_CASE(PF_BYTE, int8_t)
                default:
                    break;
            }
        }
    }
}

void
pf_vertex_bary(
    pf_vertex_t* restrict result,
    const pf_vertex_t* restrict v1,
    const pf_vertex_t* restrict v2,
    const pf_vertex_t* restrict v3,
    const pf_vec3_t bary)
{
#if defined(_OPENMP)
#define PF_VERTEX_BARY_CASE(TYPE, CTYPE)                    \
    case TYPE:                                              \
        _Pragma("omp simd")                                 \
        for (int_fast8_t j = 0; j < c_count; ++j) {         \
            CTYPE val1 = e1->value[j].v_##CTYPE * bary[0];  \
            CTYPE val2 = e2->value[j].v_##CTYPE * bary[1];  \
            CTYPE val3 = e3->value[j].v_##CTYPE * bary[2];  \
            er->value[j].v_##CTYPE = val1 + val2 + val3;    \
        }                                                   \
    break;
#else
    case TYPE:                                              \
        for (int_fast8_t j = 0; j < c_count; ++j) {         \
            CTYPE val1 = e1->value[j].v_##CTYPE * bary[0];  \
            CTYPE val2 = e2->value[j].v_##CTYPE * bary[1];  \
            CTYPE val3 = e3->value[j].v_##CTYPE * bary[2];  \
            er->value[j].v_##CTYPE = val1 + val2 + val3;    \
        }                                                   \
    break;
#endif

    for (uint32_t i = 0; i < PF_MAX_ATTRIBUTES; ++i) {
        const pf_attrib_elem_t* e1 = &v1->elements[i];
        const pf_attrib_elem_t* e2 = &v2->elements[i];
        const pf_attrib_elem_t* e3 = &v3->elements[i];
        pf_attrib_elem_t* er = &result->elements[i];
        er->used = e1->used && e2->used && e3->used;

        if (er->used) {
            int_fast8_t c_count = PF_MIN(PF_MIN(e1->comp, e2->comp), e3->comp);
            er->type = e1->type;
            er->comp = c_count;

            switch (er->type) {
                PF_VERTEX_BARY_CASE(PF_FLOAT, float)
                PF_VERTEX_BARY_CASE(PF_UNSIGNED_BYTE, uint8_t)
                PF_VERTEX_BARY_CASE(PF_SHORT, int16_t)
                PF_VERTEX_BARY_CASE(PF_INT, int32_t)
                PF_VERTEX_BARY_CASE(PF_UNSIGNED_SHORT, uint16_t)
                PF_VERTEX_BARY_CASE(PF_UNSIGNED_INT, uint32_t)
                PF_VERTEX_BARY_CASE(PF_BYTE, int8_t)
                default:
                    break;
            }
        }
    }
}
