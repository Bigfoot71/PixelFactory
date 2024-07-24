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

#ifndef PF_CAMERA2D_H
#define PF_CAMERA2D_H

#include "math/pf_vec2.h"
#include "pf_helper.h"

typedef struct {
    pf_vec2_t target;
    pf_vec2_t offset;
    float rotation;
    float zoom;
} pf_camera2d_t;

void
pf_camera2d_translate(
    pf_camera2d_t* cam,
    float dx, float dy,
    int zoom_dependent);

void
pf_camera2d_zoom(
    pf_camera2d_t* cam, float inc,
    float min, float max,
    int zoom_dependent);

void
pf_camera2d_zoom_to_world_pos(
    pf_camera2d_t* cam, const pf_vec2_t world_pos,
    float inc, float min, float max,
    int zoom_dependent);

void
pf_camera2d_zoom_to_screen_pos(
    pf_camera2d_t* cam, const pf_vec2_t screen_pos,
    float inc, float min, float max,
    int zoom_dependent);

void
pf_camera2d_to_world(
    const pf_camera2d_t* cam,
    pf_vec2_t screen_pos);

void
pf_camera2d_to_screen(
    const pf_camera2d_t* cam,
    pf_vec2_t wolrd_pos);

void
pf_camera2d_get_view_matrix(
    const pf_camera2d_t* cam,
    pf_mat3_t dst);

#endif //PF_CAMERA2D_H
