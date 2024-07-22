#ifndef PFM_H
#define PFM_H

#include <math.h>

#ifdef PF_MATH_USE_DOUBLE
#   define PF_MATH_FLOAT double
#else
#   define PF_MATH_FLOAT float
#endif

#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif //M_PI

#ifndef DEG2RAD
#   define DEG2RAD(deg) ((deg) * M_PI / 180.0)
#endif //DEG2RAD

#ifndef RAD2DEG
#   define RAD2DEG(deg) ((deg) * 180.0 / M_PI)
#endif //RAD2DEG

#ifndef PFM_FISR
#   ifdef PF_MATH_USE_DOUBLE
#       define pf_rsqrt(x) (1.0/sqrt(x))
#   else
#       define pf_rsqrt(x) (1.0f/sqrtf(x))
#   endif //PF_MATH_USE_DOUBLE
#else
// NOTE: More useful on older platforms.
// SEE: http://www.lomont.org/papers/2003/InvSqrt.pdf
static inline PF_MATH_FLOAT
pf_rsqrt(PF_MATH_FLOAT x)
{
    float xhalf = 0.5f*x;
    int i = *(int*)&x;              // get bits for floating value
    i = 0x5f375a86 - (i >> 1);      // gives initial guess y0
    x = *(float*)&i;                // convert bits back to float
    x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
    return x;
}
#endif //PFM_FISR

#endif //PFM_H