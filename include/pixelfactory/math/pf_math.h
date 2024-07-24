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

#ifndef PF_EPSILON
#   define PF_EPSILON 1e-5
#endif //PF_EPSILON

#ifndef DEG2RAD
#   define DEG2RAD(deg) ((deg) * M_PI / 180.0)
#endif //DEG2RAD

#ifndef RAD2DEG
#   define RAD2DEG(deg) ((deg) * 180.0 / M_PI)
#endif //RAD2DEG

#ifndef PFM_FISR
#   ifdef PF_MATH_USE_DOUBLE
#       define pf_math_rsqrt(x) (1.0/sqrt(x))
#   else
#       define pf_math_rsqrt(x) (1.0f/sqrtf(x))
#   endif //PF_MATH_USE_DOUBLE
#else
// NOTE: More useful on older platforms.
// SEE: http://www.lomont.org/papers/2003/InvSqrt.pdf
static inline PF_MATH_FLOAT
pf_math_rsqrt(PF_MATH_FLOAT x)
{
    float xhalf = 0.5f*x;
    int i = *(int*)&x;              // get bits for floating value
    i = 0x5f375a86 - (i >> 1);      // gives initial guess y0
    x = *(float*)&i;                // convert bits back to float
    x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
    return x;
}
#endif //PFM_FISR

/* Helper Functions */

static inline float pf_math_lerp(float x, float y, float t)
{
    return x + t * (y - x);
}

#endif //PFM_H