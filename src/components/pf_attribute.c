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

#include "pixelfactory/components/pf_attribute.h"

/* Attribute Functions */

void*
pf_attribute_get_elem_ptr(const pf_attribute_t* attr, size_t index)
{
    uint8_t element_size = 0;

    switch (attr->type) {
        case PF_ATTRIB_FLOAT: element_size = sizeof(float); break;
        case PF_ATTRIB_UBYTE: element_size = sizeof(uint8_t); break;
    }

    return (char*)attr->buffer + index * element_size * attr->comp;
}

pf_attrib_elem_t
pf_attribute_get_elem(const pf_attribute_t* attr, size_t index)
{
#define PF_ATTRIBUTE_GET_ELEM_CASE(TYPE, CTYPE)                         \
    case TYPE: {                                                        \
        CTYPE* ptr = (CTYPE*)attr->buffer + index * attr->comp;         \
        for (int_fast8_t i = 0; i < attr->comp; ++i) {                  \
            result.value[i].v_##CTYPE = ptr[i];                         \
        }                                                               \
    } break;

    pf_attrib_elem_t result;

    switch (attr->type) {
        PF_ATTRIBUTE_GET_ELEM_CASE(PF_ATTRIB_FLOAT, float)
        PF_ATTRIBUTE_GET_ELEM_CASE(PF_ATTRIB_UBYTE, uint8_t)
        default:
            break;
    }

    result.type = attr->type;
    result.comp = attr->comp;
    result.used = attr->used;

    return result;
}

/* Attribute Element Functions */

float
pf_attrib_elem_get_comp_f(const pf_attrib_elem_t* elem, uint8_t index)
{
#define PF_ATTRIB_ELEM_GET_COMP_F_CASE(TYPE, CTYPE)     \
    case TYPE:                                          \
        return (float)elem->value[index].v_##CTYPE;

    switch (elem->type) {
        PF_ATTRIB_ELEM_GET_COMP_F_CASE(PF_ATTRIB_FLOAT, float)
        PF_ATTRIB_ELEM_GET_COMP_F_CASE(PF_ATTRIB_UBYTE, uint8_t)
        default:
            return 0.0f;
    }
}

void
pf_attrib_elem_get_comp(const pf_attrib_elem_t* elem, uint8_t index, void* out)
{
#define PF_ATTRIB_ELEM_GET_COMP_CASE(TYPE, CTYPE)       \
    case TYPE:                                          \
        *((CTYPE*)out) = elem->value[index].v_##CTYPE;  \
        break;

    switch (elem->type) {
        PF_ATTRIB_ELEM_GET_COMP_CASE(PF_ATTRIB_FLOAT, float)
        PF_ATTRIB_ELEM_GET_COMP_CASE(PF_ATTRIB_UBYTE, uint8_t)
        default:
            break;
    }
}

void
pf_attrib_elem_get_vec(const pf_attrib_elem_t* elem, void* out)
{
#define PF_ATTRIB_ELEM_GET_COMP_VEC_CASE(TYPE, CTYPE)       \
    case TYPE:                                              \
        for (int_fast8_t i = 0; i < elem->comp; ++i) {      \
            ((CTYPE*)out)[i] = elem->value[i].v_##CTYPE;    \
        }                                                   \
        break;

    switch (elem->type) {
        PF_ATTRIB_ELEM_GET_COMP_VEC_CASE(PF_ATTRIB_FLOAT, float)
        PF_ATTRIB_ELEM_GET_COMP_VEC_CASE(PF_ATTRIB_UBYTE, uint8_t)
        default:
            break;
    }
}

void
pf_attrib_elem_set_comp_f(pf_attrib_elem_t* elem, uint8_t index, float value)
{
#define PF_ATTRIB_ELEM_SET_COMP_F_CASE(TYPE, CTYPE)     \
    case TYPE:                                          \
        elem->value[index].v_##CTYPE = (CTYPE)value;    \
        break;

    switch (elem->type) {
        PF_ATTRIB_ELEM_SET_COMP_F_CASE(PF_ATTRIB_FLOAT, float)
        PF_ATTRIB_ELEM_SET_COMP_F_CASE(PF_ATTRIB_UBYTE, uint8_t)
        default:
            break;
    }
}

void
pf_attrib_elem_set_comp(pf_attrib_elem_t* elem, uint8_t index, const void* in)
{
#define PF_ATTRIB_ELEM_SET_COMP_CASE(TYPE, CTYPE)       \
    case TYPE:                                          \
        elem->value[index].v_##CTYPE = *((CTYPE*)in);   \
        break;

    switch (elem->type) {
        PF_ATTRIB_ELEM_SET_COMP_CASE(PF_ATTRIB_FLOAT, float)
        PF_ATTRIB_ELEM_SET_COMP_CASE(PF_ATTRIB_UBYTE, uint8_t)
        default:
            break;
    }
}

void
pf_attrib_elem_set_vec(pf_attrib_elem_t* elem, const void* in)
{
#define PF_ATTRIB_ELEM_SET_COMP_VEC_CASE(TYPE, CTYPE)  \
    case TYPE:                                          \
        for (int_fast8_t i = 0; i < elem->comp; ++i) {  \
            elem->value[i].v_##CTYPE = ((CTYPE*)in)[i]; \
        }                                               \
        break;

    switch (elem->type) {
        PF_ATTRIB_ELEM_SET_COMP_VEC_CASE(PF_ATTRIB_FLOAT, float)
        PF_ATTRIB_ELEM_SET_COMP_VEC_CASE(PF_ATTRIB_UBYTE, uint8_t)
        default:
            break;
    }
}
