#include "pixelfactory/components/pf_vertex.h"

void
pf_vertex3d_lerp(
    pf_vertex3d_t* restrict result,
    const pf_vertex3d_t* restrict start,
    const pf_vertex3d_t* restrict end,
    float t)
{
    uint8_t uT = 255*t;

#ifdef _OPENMP
#   pragma omp simd
#endif //_OPENMP
    for (int_fast8_t i = 0; i < 4; ++i)
    {
        result->color.a[i] = start->color.a[i] + (uT * ((int)end->color.a[i] - start->color.a[i])) / 255;

        if (i < 2) {
            result->texcoord[i] = start->texcoord[i] + t * (end->texcoord[i] - start->texcoord[i]);
        }

        if (i < 3) {
            result->position[i] = start->position[i] + t * (end->position[i] - start->position[i]);
            result->normal[i] = start->normal[i] + t * (end->normal[i] - start->normal[i]);
        }
    }
}
