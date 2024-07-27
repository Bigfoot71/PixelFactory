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

#ifndef PF_VERTEX_H
#define PF_VERTEX_H

#include "../math/pf_vec2.h"
#include "../math/pf_vec4.h"
#include "pf_color.h"

typedef struct {
    pf_vec2_t position;
    pf_vec2_t texcoord;
    pf_color_t color;
    uint32_t index;
} pf_vertex2d_t;

typedef struct {
    float* positions;
    float* texcoords;
    pf_color_t* colors;
    uint16_t* indices;
    uint32_t num_vertices;
    uint32_t num_indices;
} pf_vertexbuffer2d_t;

typedef struct {
    pf_vec3_t position;
    pf_vec2_t texcoord;
    pf_vec3_t normal;
    pf_color_t color;
    uint32_t index;
} pf_vertex3d_t;

typedef struct {
    float* positions;
    float* texcoords;
    float* normals;
    pf_color_t* colors;
    uint16_t* indices;
    uint32_t num_vertices;
    uint32_t num_indices;
} pf_vertexbuffer3d_t;

#endif //PF_VERTEX_H
