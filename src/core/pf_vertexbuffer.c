#include "pixelfactory/core/pf_vertexbuffer.h"
#include "pixelfactory/math/pf_vec3.h"

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

void
pf_vertexbuffer_delete(
    pf_vertexbuffer_t* vb)
{
    for (int i = 0; i < PF_MAX_ATTRIBUTES; ++i) {
        if (vb->attributes[i].buffer != NULL) {
            PF_FREE(vb->attributes[i].buffer);
        }
    }
    *vb = (pf_vertexbuffer_t) { 0 };
}


/* VERTEX BUFFER EXTENSION */


#ifdef PF_EXT_VERTEXBUFFER

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

DIAGNOSTIC_PUSH
DIAGNOSTIC_IGNORE_UNUSED_PARAMETER
#   define PAR_SHAPES_IMPLEMENTATION
#   include "par_shapes.h"
DIAGNOSTIC_POP

static void
pfext_objloader_file_reader_INTERNAL(
    void *ctx, const char *filename,
    int is_mtl, const char *obj_filename,
    char **buf, size_t *len)
{
    (void)obj_filename;
    (void)is_mtl;
    (void)ctx;

    FILE *file = fopen(filename, "r");
    if (!file) {
        *buf = NULL;
        *len = 0;
        return;
    }

    fseek(file, 0, SEEK_END);
    *len = ftell(file);
    fseek(file, 0, SEEK_SET);

    *buf = (char *)PF_MALLOC(*len);
    if (*buf == NULL) {
        fclose(file);
        *len = 0;
        return;
    }

    int r = fread(*buf, 1, *len, file);
    (void)r;

    fclose(file);
}

pf_vertexbuffer_t
pfext_vertexbuffer_load_obj(
    const char* file_path,
    bool triangulate)
{
    pf_vertexbuffer_t vertexbuffer = { 0 };

    // Initialize variables for tinyobj
    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes = NULL;
    size_t num_shapes;
    tinyobj_material_t* materials = NULL;
    size_t num_materials;

    // Open the OBJ file
    FILE *file = fopen(file_path, "r");
    if (!file) {
        fprintf(stderr, "ERROR: Unable to load file [%s]\n", file_path);
        return vertexbuffer;
    }

    // Load the OBJ file
    int result = tinyobj_parse_obj(
        &attrib, &shapes, &num_shapes,
        &materials, &num_materials, file_path,
        pfext_objloader_file_reader_INTERNAL, file,
        triangulate ? TINYOBJ_FLAG_TRIANGULATE : 0);

    if (result != TINYOBJ_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to load OBJ mesh [%s]\n", file_path);
        fclose(file);
        return vertexbuffer;
    }

    // Define the number of vertices and indices
    vertexbuffer.num_vertices = attrib.num_vertices / 3;
    vertexbuffer.num_indices = attrib.num_face_num_verts;

    // Load vertex positions
    pf_attribute_t* position_attr = &vertexbuffer.attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX];
    position_attr->type = PF_FLOAT;
    position_attr->comp = 3; // x, y, z
    position_attr->size = vertexbuffer.num_vertices * 3 * sizeof(float);
    position_attr->buffer = PF_MALLOC(position_attr->size);
    if (position_attr->buffer == NULL) {
        fprintf(stderr, "ERROR: Unable to allocate memory for vertex positions\n");
        goto finish;
    }
    memcpy(position_attr->buffer, attrib.vertices, position_attr->size);
    position_attr->used = true;

    // Load texture coordinates, if present
    if (attrib.num_texcoords > 0) {
        pf_attribute_t* texcoord_attr = &vertexbuffer.attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX];
        texcoord_attr->type = PF_FLOAT;
        texcoord_attr->comp = 2; // u, v
        texcoord_attr->size = vertexbuffer.num_vertices * 2 * sizeof(float);
        texcoord_attr->buffer = PF_MALLOC(texcoord_attr->size);
        if (texcoord_attr->buffer == NULL) {
            fprintf(stderr, "ERROR: Unable to allocate memory for texture coordinates\n");
            goto finish;
        }
        memcpy(texcoord_attr->buffer, attrib.texcoords, texcoord_attr->size);
        texcoord_attr->used = true;
    }

    // Load normals, if present
    if (attrib.num_normals > 0) {
        pf_attribute_t* normal_attr = &vertexbuffer.attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX];
        normal_attr->type = PF_FLOAT;
        normal_attr->comp = 3; // nx, ny, nz
        normal_attr->size = vertexbuffer.num_vertices * 3 * sizeof(float);
        normal_attr->buffer = PF_MALLOC(normal_attr->size);
        if (normal_attr->buffer == NULL) {
            fprintf(stderr, "ERROR: Unable to allocate memory for normals\n");
            goto finish;
        }
        memcpy(normal_attr->buffer, attrib.normals, normal_attr->size);
        normal_attr->used = true;
    }

    // Load indices
    vertexbuffer.indices = (uint16_t*)PF_MALLOC(vertexbuffer.num_indices * sizeof(uint16_t));
    if (vertexbuffer.indices == NULL) {
        fprintf(stderr, "ERROR: Unable to allocate memory for indices\n");
        goto finish;
    }
    for (size_t f = 0; f < attrib.num_face_num_verts; f++) {
        vertexbuffer.indices[f] = (uint16_t)attrib.faces[f].v_idx;
    }

finish:
    // Free tinyobj resources
    tinyobj_attrib_free(&attrib);
    tinyobj_shapes_free(shapes, num_shapes);
    tinyobj_materials_free(materials, num_materials);

    return vertexbuffer;
}

static void
pfext_cgltf_copy_accessor_data_INTERNAL(
    float** dest, cgltf_accessor* accessor)
{
    *dest = PF_MALLOC(accessor->count * sizeof(float) * cgltf_num_components(accessor->type));
    cgltf_accessor_unpack_floats(accessor, *dest, accessor->count * cgltf_num_components(accessor->type));
}

pf_vertexbuffer_t*
pfext_vertexbuffer_load_gltf(
    const char* file_path,
    int* num_vertexbuffer)
{
    pf_vertexbuffer_t* vbs = NULL;
    *num_vertexbuffer = 0;

    cgltf_options options = {0};
    cgltf_data* data = NULL;

    cgltf_result err = cgltf_parse_file(&options, file_path, &data);
    if (err != cgltf_result_success) {
        fprintf(stderr, "ERROR: Unable to load file [%s]\n", file_path);
        exit(EXIT_FAILURE);
    }

    err = cgltf_load_buffers(&options, data, file_path);
    if (err != cgltf_result_success) {
        fprintf(stderr, "ERROR: Unable to load GLTF model [%s]\n", file_path);
        cgltf_free(data);
        exit(EXIT_FAILURE);
    }

    if (data->meshes_count == 0) {
        fprintf(stderr, "ERROR: The GLTF model [%s] does not contain any mesh\n", file_path);
        cgltf_free(data);
        exit(EXIT_FAILURE);
    }

    vbs = PF_CALLOC(data->meshes_count, sizeof(pf_vertexbuffer_t));
    *num_vertexbuffer = data->meshes_count;

    for (cgltf_size i = 0; i < data->meshes_count; ++i) {
        cgltf_mesh* cgltf_mesh = &data->meshes[i];
        pf_vertexbuffer_t* vb = &vbs[i];

        if (cgltf_mesh->primitives_count == 0) {
            continue;
        }

        cgltf_primitive* primitive = &cgltf_mesh->primitives[0];

        for (cgltf_size j = 0; j < primitive->attributes_count; ++j) {
            cgltf_attribute* attribute = &primitive->attributes[j];
            if (attribute->data->type == cgltf_type_scalar) {
                continue;
            }

            switch (attribute->type) {
                case cgltf_attribute_type_position: {
                    pf_attribute_t* attr = &vb->attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX];
                    float* positions = attr->buffer;
                    pfext_cgltf_copy_accessor_data_INTERNAL(&positions, attribute->data);
                    vb->num_vertices = attribute->data->count;
                    attr->size = attribute->data->count;
                    attr->type = PF_FLOAT;
                    attr->used = true;
                    attr->comp = 3;
                } break;
                case cgltf_attribute_type_texcoord: {
                    pf_attribute_t* attr = &vb->attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX];
                    float* texcoords = attr->buffer;
                    pfext_cgltf_copy_accessor_data_INTERNAL(&texcoords, attribute->data);
                    attr->size = attribute->data->count;
                    attr->type = PF_FLOAT;
                    attr->used = true;
                    attr->comp = 2;
                } break;
                case cgltf_attribute_type_normal: {
                    pf_attribute_t* attr = &vb->attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX];
                    float* normals = attr->buffer;
                    pfext_cgltf_copy_accessor_data_INTERNAL(&normals, attribute->data);
                    attr->size = attribute->data->count;
                    attr->type = PF_FLOAT;
                    attr->used = true;
                    attr->comp = 3;
                } break;
                case cgltf_attribute_type_color: {
                    pf_attribute_t* attr = &vb->attributes[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX];
                    attr->buffer = PF_MALLOC(attribute->data->count * sizeof(pf_color_t));
                    cgltf_accessor_unpack_floats(attribute->data, (float*)attr->buffer, attribute->data->count * 4);
                } break;
                default:
                    break;
            }
        }

        if (primitive->indices != NULL) {
            vb->num_indices = primitive->indices->count;
            vb->indices = PF_MALLOC(vb->num_indices * sizeof(uint16_t));
            for (cgltf_size k = 0; k < primitive->indices->count; ++k) {
                vb->indices[k] = (uint16_t)cgltf_accessor_read_index(primitive->indices, k);
            }
        }
    }

    // Load materials
    /*
    model.materials_count = data->materials_count;
    model.materials = PF_CALLOC(data->materials_count, sizeof(material_t));

    for (cgltf_size i = 0; i < data->materials_count; ++i) {
        cgltf_material* cgltf_material = &data->materials[i];
        material_t* material = &model.materials[i];

        material->tex_diffuse = default_texture;
        material->col_diffuse = PF_WHITE;

        if (cgltf_material->has_pbr_metallic_roughness) {
            cgltf_texture_view* texture_view = &cgltf_material->pbr_metallic_roughness.base_color_texture;
            if (texture_view->texture && texture_view->texture->image) {
                material->tex_diffuse = load_texture(texture_view->texture->image, filename);
            }

            float* color_factor = cgltf_material->pbr_metallic_roughness.base_color_factor;
            if (color_factor) material->col_diffuse = pf_color_from_factor(color_factor);
        }
    }

    // Map meshes to materials (assuming one material per mesh for simplicity)
    model.mesh_material = PF_MALLOC(data->meshes_count * sizeof(size_t));
    for (cgltf_size i = 0; i < data->meshes_count; ++i) {
        cgltf_mesh* cgltf_mesh = &data->meshes[i];
        if (cgltf_mesh->primitives_count > 0) {
            model.mesh_material[i] = cgltf_mesh->primitives[0].material - data->materials;
        }
    }
    */

    cgltf_free(data);

    return vbs;
}

static void
pfext_set_vb_from_par_shapes_mesh_INTERNAL(pf_vertexbuffer_t* vb, par_shapes_mesh* mesh)
{
    vb->num_vertices = mesh->npoints;

    vb->attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].buffer = mesh->points;
    vb->attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].size = mesh->npoints;
    vb->attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].type = PF_FLOAT;
    vb->attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].used = true;
    vb->attributes[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].comp = 3;

    vb->attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].buffer = mesh->tcoords;
    vb->attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].size = mesh->npoints;
    vb->attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].type = PF_FLOAT;
    vb->attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].used = true;
    vb->attributes[PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX].comp = 2;

    vb->attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX].buffer = mesh->normals;
    vb->attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX].size = mesh->npoints;
    vb->attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX].type = PF_FLOAT;
    vb->attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX].used = true;
    vb->attributes[PF_DEFAULT_ATTRIBUTE_NORMAL_INDEX].comp = 3;

    vb->indices = mesh->triangles;
    vb->num_indices = mesh->ntriangles;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_cylinder(
    int slices, int stacks)
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_cylinder(slices, stacks);
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_cone(
    int slices, int stacks)
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_cone(slices, stacks);
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_disk(
    int slices, int stacks)
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_parametric_disk(slices, stacks);
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_torus(
    int slices, int stacks, float radius)
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_torus(slices, stacks, radius);
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_sphere(
    int slices, int stacks)
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_parametric_sphere(slices, stacks);
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_subdivided_sphere(
    int nsubdivisions)
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_subdivided_sphere(nsubdivisions);
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_klein_bottle(
    int slices, int stacks)
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_klein_bottle(slices, stacks);
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_trefoil_knot(
    int slices, int stacks, float radius)
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_trefoil_knot(slices, stacks, radius);
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_hemisphere(
    int slices, int stacks)
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_hemisphere(slices, stacks);
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_plane(
    int slices, int stacks)
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_plane(slices, stacks);
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_icosahedron()
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_icosahedron();
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_dodecahedron()
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_dodecahedron();
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_octahedron()
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_octahedron();
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_tetrahedron()
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_tetrahedron();
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_cube()
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_cube();
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_disk_ex(
    float radius, int slices,
    const pf_vec3_t center,
    const pf_vec3_t normal)
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_disk(radius, slices, center, normal);
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_empty()
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_empty();
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

pf_vertexbuffer_t
pfext_vertexbuffer_gen_rock(
    int seed, int nsubdivisions)
{
    pf_vertexbuffer_t vb = { 0 };
    par_shapes_mesh* mesh = par_shapes_create_rock(seed, nsubdivisions);
    pfext_set_vb_from_par_shapes_mesh_INTERNAL(&vb, mesh);
    return vb;
}

#endif //PF_EXT_VERTEXBUFFER
