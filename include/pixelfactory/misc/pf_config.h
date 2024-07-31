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

#ifndef PF_CONFIG_H
#define PF_CONFIG_H

#if defined(_WIN32)
#   if defined(PF_BUILD_SHARED)
#       define PFAPI __declspec(dllexport)
#   elif defined(PF_USE_SHARED)
#       define PFAPI __declspec(dllimport)
#   endif
#else
#   if defined(PF_BUILD_SHARED)
#       define PFAPI __attribute__((visibility("default")))
#   endif
#endif

#ifndef PFAPI
#   define PFAPI
#endif

#ifndef PF_MAX_ATTRIBUTES
#   define PF_MAX_ATTRIBUTES 4
#endif //PF_MAX_ATTRIBUTES

#ifndef PF_MAX_CLIPPED_TRIANGLE_VERTICES
#   define PF_MAX_CLIPPED_POLYGON_VERTICES 12
#endif //PF_MAX_CLIPPED_POLYGON_VERTICES

#ifndef PF_OMP_BUFFER_COPY_SIZE_THRESHOLD
#    define PF_OMP_BUFFER_COPY_SIZE_THRESHOLD 640*480
#endif //PF_OMP_BUFFER_COPY_SIZE_THRESHOLD

#ifndef PF_OMP_BUFFER_MAP_SIZE_THRESHOLD
#    define PF_OMP_BUFFER_MAP_SIZE_THRESHOLD 640*480
#endif //PF_OMP_BUFFER_MAP_SIZE_THRESHOLD

#ifndef PF_OMP_TEXTURE_RN2D_SIZE_THRESHOLD
#   define PF_OMP_TEXTURE_RN2D_SIZE_THRESHOLD 32*32
#endif //PF_OMP_TEXTURE_RN2D_SIZE_THRESHOLD

#ifndef PF_OMP_TRIANGLE_AABB_THRESHOLD
#   define PF_OMP_TRIANGLE_AABB_THRESHOLD 32*32
#endif //PF_OMP_TRIANGLE_AABB_THRESHOLD

#ifndef PF_OMP_CLEAR_BUFFER_SIZE_THRESHOLD
#    define PF_OMP_CLEAR_BUFFER_SIZE_THRESHOLD 640*480
#endif //PF_OPENMP_CLEAR_BUFFER_SIZE_THRESHOLD

#ifndef PF_OMP_TRIANGLE_NUMBER_THRESHOLD
// NOTE: If the number of triangles rendered via a vertexbuffer
//       is greater than or equal to this value, the parallelization 
//       will be done by group of triangles rather than by line when
//       rendering the triangles individually.
#   define PF_OMP_TRIANGLE_NUMBER_THRESHOLD 16
#endif //PF_OMP_TRIANGLE_NUMBER_THRESHOLD

#endif //PF_CONFIG_H
