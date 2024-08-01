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

#include "pixelfactory/utils/pf_camera2d.h"

void
pf_camera2d_translate(
    pf_camera2d_t* cam,
    float dx, float dy,
    int zoom_dependent)
{
    cam->target[0] += zoom_dependent ? dx / cam->zoom : dx;
    cam->target[1] += zoom_dependent ? dy / cam->zoom : dy;
}

void
pf_camera2d_zoom(
    pf_camera2d_t* cam, float inc,
    float min, float max,
    int zoom_dependent)
{
    cam->zoom = PF_CLAMP(cam->zoom + (zoom_dependent ? inc * cam->zoom : inc), min, max);
}

void
pf_camera2d_zoom_to_world_pos(
    pf_camera2d_t* cam, const pf_vec2_t world_pos,
    float inc, float min, float max,
    int zoom_dependent)
{
    pf_vec2_t offset;
    pf_vec2_sub_r(offset, world_pos, cam->target);
    const float previousZoom = cam->zoom;

    pf_camera2d_zoom(cam, inc, min, max, zoom_dependent);
    cam->target[0] = world_pos[0] - offset[0] * (previousZoom / cam->zoom);
    cam->target[1] = world_pos[1] - offset[1] * (previousZoom / cam->zoom);
}

void
pf_camera2d_zoom_to_screen_pos(
    pf_camera2d_t* cam, const pf_vec2_t screen_pos,
    float inc, float min, float max,
    int zoom_dependent)
{
    pf_vec2_t world_pos;
    memcpy(world_pos, screen_pos, sizeof(pf_vec2_t));

    pf_camera2d_to_world(cam, world_pos);
    pf_camera2d_zoom_to_world_pos(cam, screen_pos, inc, min, max, zoom_dependent);
}

void
pf_camera2d_to_world(
    const pf_camera2d_t* cam,
    pf_vec2_t screen_pos)
{
    pf_mat3_t mat;
    pf_camera2d_get_view_matrix(cam, mat);
    pf_mat3_inverse(mat, mat);
    pf_vec2_transform(screen_pos, screen_pos, mat);
}

void
pf_camera2d_to_screen(
    const pf_camera2d_t* cam,
    pf_vec2_t wolrd_pos)
{
    pf_mat3_t mat;
    pf_camera2d_get_view_matrix(cam, mat);
    pf_vec2_transform(wolrd_pos, wolrd_pos, mat);
}

void
pf_camera2d_get_view_matrix(
    const pf_camera2d_t* cam,
    pf_mat3_t dst)
{
    // Translation to the origin
    pf_mat3_translate(dst, -cam->target[0], -cam->target[1]);

    // Rotation matrix
    pf_mat3_t rotation;
    pf_mat3_rotate(rotation, cam->rotation);

    // Zoom/scale matrix
    pf_mat3_t scale;
    pf_mat3_scale(scale, cam->zoom, cam->zoom);

    // Multiplication of the zoom matrix and the rotation matrix
    pf_mat3_mul(dst, scale, rotation);

    // Multiplication with the translation matrix to the origin
    pf_mat3_mul(dst, dst, dst);

    // Final translation matrix (offset)
    pf_mat3_t translation;
    pf_mat3_translate(translation, cam->offset[0], cam->offset[1]);

    // Final multiplication with the offset matrix
    pf_mat3_mul(dst, dst, translation);
}
