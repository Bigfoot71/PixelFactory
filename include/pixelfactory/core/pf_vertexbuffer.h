#ifndef PF_VERTEXBUFFER_H
#define PF_VERTEXBUFFER_H

#include "../components/pf_attribute.h"
#include "../components/pf_color.h"
#include "../misc/pf_stdinc.h"

/* Vertex Buffer Types */

typedef struct {
    pf_attribute_t attributes[PF_MAX_ATTRIBUTES];
    uint16_t* indices;
    uint32_t num_vertices;
    uint32_t num_indices;
} pf_vertexbuffer_t;

/* Helper Vertex Buffer Functions */

PFAPI pf_vertexbuffer_t
pf_vertexbuffer_create_2d(
    uint32_t num_vertices,
    float* positions,
    float* texcoords,
    pf_color_t* colors);

PFAPI pf_vertexbuffer_t
pf_vertexbuffer_create_3d(
    uint32_t num_vertices,
    float* positions,
    float* texcoords,
    float* normals,
    pf_color_t* colors);

#endif //PF_VERTEXBUFFER_H
