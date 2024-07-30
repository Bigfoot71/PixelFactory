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

#ifndef PF_ATTRIBUTE_H
#define PF_ATTRIBUTE_H

#include "../misc/pf_config.h"
#include "../math/pf_vec4.h"
#include "pf_types.h"

/* Helper Definitions */

#define PF_DEFAULT_ATTRIBUTE_POSITION_INDEX     0
#define PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX     1
#define PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX       2
#define PF_DEFAULT_ATTRIBUTE_COLOR_INDEX        3

/* Attribute Type */

typedef struct {
    void*       buffer;
    size_t      size;
    pf_type_e   type;
    uint8_t     comp;
    uint8_t     used;
} pf_attribute_t;

/* Attribute Element Type */

typedef union {
    uint8_t     v_uint8_t;
    int8_t      v_int8_t;
    uint16_t    v_uint16_t;
    int16_t     v_int16_t;
    uint32_t    v_uint32_t;
    int32_t     v_int32_t;
    float       v_float;
} pf_attrib_value_t;

typedef struct {
    pf_attrib_value_t   value[4];
    pf_type_e           type;
    uint8_t             comp;
    uint8_t             used;
} pf_attrib_elem_t;

/* Attribute Functions */

PFAPI void*
pf_attribute_get_elem_ptr(const pf_attribute_t* attr, size_t index);

PFAPI pf_attrib_elem_t
pf_attribute_get_elem(const pf_attribute_t* attr, size_t index);

/* Attribute Element Functions */

PFAPI float
pf_attrib_elem_get_comp_f(const pf_attrib_elem_t* elem, uint8_t index);

PFAPI void
pf_attrib_elem_get_comp(const pf_attrib_elem_t* elem, uint8_t index, void* out);

PFAPI void
pf_attrib_elem_get_vec(const pf_attrib_elem_t* elem, void* out);

PFAPI void
pf_attrib_elem_set_comp_f(pf_attrib_elem_t* elem, uint8_t index, float value);

PFAPI void
pf_attrib_elem_set_comp(pf_attrib_elem_t* elem, uint8_t index, const void* in);

PFAPI void
pf_attrib_elem_set_vec(pf_attrib_elem_t* elem, const void* in);

#endif //PF_ATTRIBUTE_H
