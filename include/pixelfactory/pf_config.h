#ifndef PF_CONFIG_H
#define PF_CONFIG_H

#ifndef PF_MAX_CLIPPED_TRIANGLE_VERTICES
#   define PF_MAX_CLIPPED_POLYGON_VERTICES 12
#endif //PF_MAX_CLIPPED_POLYGON_VERTICES

#ifndef PF_OMP_TRIANGLE_AABB_THRESHOLD
#   define PF_OMP_TRIANGLE_AABB_THRESHOLD 32*32
#endif //PF_OMP_TRIANGLE_AABB_THRESHOLD

#ifndef PF_OMP_CLEAR_BUFFER_SIZE_THRESHOLD
#    define PF_OMP_CLEAR_BUFFER_SIZE_THRESHOLD 640*480
#endif //PF_OPENMP_CLEAR_BUFFER_SIZE_THRESHOLD

#endif //PF_CONFIG_H
