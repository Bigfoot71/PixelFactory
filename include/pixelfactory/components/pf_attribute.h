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

/* Helper Definitions */

typedef enum {
    PF_ATTRIB_POSITION = 0,
    PF_ATTRIB_TEXCOORD,
    PF_ATTRIB_NORMAL,
    PF_ATTRIB_COLOR
} pf_attrib_default_e;

/* Attribute Type */

typedef enum {
    PF_ATTRIB_FLOAT,
    PF_ATTRIB_UBYTE,
} pf_attrib_type_e;

typedef struct {
    void*               buffer;
    size_t              size;
    pf_attrib_type_e    type;
    uint8_t             comp;
    bool                used;
} pf_attribute_t;

/* Attribute Element Type */

typedef union {
    float       v_float;
    uint8_t     v_uint8_t;
} pf_attrib_variant_t;

typedef struct {
    pf_attrib_variant_t value[4];
    pf_attrib_type_e    type;
    uint8_t             comp;
    bool                used;
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
