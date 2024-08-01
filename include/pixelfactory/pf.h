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

#ifndef PF_H
#define PF_H

#include "components/pf_attribute.h"
#include "components/pf_color.h"
#include "components/pf_depth.h"
#include "components/pf_pixel.h"
#include "components/pf_processors.h"
#include "components/pf_simd.h"
#include "components/pf_vertex.h"

#include "core/pf_depthbuffer.h"
#include "core/pf_framebuffer.h"
#include "core/pf_renderer.h"
#include "core/pf_texture2d.h"
#include "core/pf_vertexbuffer.h"

#include "math/pf_math.h"
#include "math/pf_vec2.h"
#include "math/pf_vec3.h"
#include "math/pf_vec4.h"
#include "math/pf_mat3.h"
#include "math/pf_mat4.h"

#include "misc/pf_config.h"
#include "misc/pf_helper.h"
#include "misc/pf_stdinc.h"

#include "utils/pf_camera2d.h"
#include "utils/pf_camera3d.h"

#endif //PF_H
