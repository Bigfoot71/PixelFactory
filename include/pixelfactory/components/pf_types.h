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

#ifndef PF_TYPES_H
#define PF_TYPES_H

#include "../misc/pf_stdinc.h"

typedef enum {
    PF_UNSIGNED_BYTE,
    PF_UNSIGNED_SHORT,
    PF_UNSIGNED_INT,
    PF_BYTE,
    PF_SHORT,
    PF_INT,
    PF_FLOAT
} pf_type_e;

static inline uint8_t
pf_type_get_size(pf_type_e type)
{
    uint8_t size = 0;

    switch (type) {
        case PF_UNSIGNED_BYTE:
        case PF_BYTE:
            size = sizeof(uint8_t);
            break;
        case PF_UNSIGNED_SHORT:
        case PF_SHORT:
            size = sizeof(uint16_t);
            break;
        case PF_UNSIGNED_INT:
        case PF_INT:
            size = sizeof(uint32_t);
            break;
        case PF_FLOAT:
            size = sizeof(float);
            break;
        default:
            break;
    }

    return size;
}

#endif //PF_TYPES_H
