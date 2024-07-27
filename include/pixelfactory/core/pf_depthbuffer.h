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

#ifndef PF_DEPTHBUFFER_H
#define PF_DEPTHBUFFER_H

#include "../misc/pf_config.h"
#include <stdint.h>

typedef struct {
    float* buffer;
    uint32_t w;
    uint32_t h;
} pf_depthbuffer_t;

pf_depthbuffer_t
pf_depthbuffer_create(
    uint32_t w, uint32_t h,
    float def);

PFAPI void
pf_depthbuffer_delete(
    pf_depthbuffer_t* zb);

PFAPI int
pf_depthbuffer_is_valid(
    const pf_depthbuffer_t* zb);

PFAPI void
pf_depthbuffer_put(
    pf_depthbuffer_t* zb,
    uint32_t x, uint32_t y,
    float depth);

PFAPI void
pf_depthbuffer_fill(
    pf_depthbuffer_t* zb,
    const uint32_t rect[4],
    float depth);

#endif //PF_DEPTHBUFFER_H
