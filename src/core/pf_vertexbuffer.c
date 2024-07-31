#include "pixelfactory/core/pf_vertexbuffer.h"

/* Helper Vertex Buffer Functions */

pf_vertexbuffer_t
pf_vertexbuffer_create_2d(
    uint32_t num_vertices,
    float* positions,
    float* texcoords,
    pf_color_t* colors)
{
    pf_vertexbuffer_t vb = { 0 };

    if (positions != NULL) {
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].buffer = positions;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].size = num_vertices;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].type = PF_FLOAT;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].comp = 2;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].used = true;
    }

    if (texcoords != NULL) {
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].buffer = texcoords;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].size = num_vertices;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].type = PF_FLOAT;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].comp = 2;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].used = true;
    }

    if (colors != NULL) {
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].buffer = colors;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].size = num_vertices;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].type = PF_UNSIGNED_BYTE;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].comp = 4;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].used = true;
    }

    vb.num_vertices = num_vertices;

    return vb;
}

pf_vertexbuffer_t
pf_vertexbuffer_create_3d(
    uint32_t num_vertices,
    float* positions,
    float* texcoords,
    float* normals,
    pf_color_t* colors)
{
    pf_vertexbuffer_t vb = { 0 };

    if (positions != NULL) {
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].buffer = positions;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].size = num_vertices;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].type = PF_FLOAT;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].comp = 3;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].used = true;
    }

    if (texcoords != NULL) {
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].buffer = texcoords;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].size = num_vertices;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].type = PF_FLOAT;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].comp = 2;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].used = true;
    }

    if (normals != NULL) {
        vb.attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX].buffer = normals;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX].size = num_vertices;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX].type = PF_FLOAT;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX].comp = 3;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX].used = true;
    }

    if (colors != NULL) {
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].buffer = colors;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].size = num_vertices;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].type = PF_UNSIGNED_BYTE;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].comp = 4;
        vb.attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].used = true;
    }

    vb.num_vertices = num_vertices;

    return vb;
}
