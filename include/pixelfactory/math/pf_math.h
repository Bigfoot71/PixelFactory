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
    float xhalf = 0.5f * x;
    int i = *(int*)&x;              // get bits for floating value
    i = 0x5f375a86 - (i >> 1);      // gives initial guess y0
    x = *(float*)&i;                // convert bits back to float
    x = x * (1.5f - xhalf * x * x); // Newton step, repeating increases accuracy
    return x;
}
#endif //PFM_FISR

/* Helper Functions */

static inline PF_MATH_FLOAT
pf_math_clamp(PF_MATH_FLOAT value, PF_MATH_FLOAT min, PF_MATH_FLOAT max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

static inline PF_MATH_FLOAT
pf_math_lerp(PF_MATH_FLOAT x, PF_MATH_FLOAT y, PF_MATH_FLOAT t)
{
    return x + t * (y - x);
}

static inline PF_MATH_FLOAT
pf_math_inverse_lerp(PF_MATH_FLOAT a, PF_MATH_FLOAT b, PF_MATH_FLOAT value)
{
    return (value - a) / (b - a);
}

static inline PF_MATH_FLOAT
pf_math_smoothstep(PF_MATH_FLOAT a, PF_MATH_FLOAT b, PF_MATH_FLOAT t)
{
    t = (t - a) / (b - a);
    return t * t * (3 - 2 * t);
}

static inline PF_MATH_FLOAT
pf_math_hermite(PF_MATH_FLOAT a, PF_MATH_FLOAT b, PF_MATH_FLOAT c, PF_MATH_FLOAT d, PF_MATH_FLOAT t)
{
    PF_MATH_FLOAT t2 = t * t;
    PF_MATH_FLOAT t3 = t2 * t;
    return (2 * t3 - 3 * t2 + 1) * a + (t3 - 2 * t2 + t) * b + (-2 * t3 + 3 * t2) * c + (t3 - t2) * d;
}

static inline PF_MATH_FLOAT
pf_math_cubic_interpolation(PF_MATH_FLOAT a, PF_MATH_FLOAT b, PF_MATH_FLOAT c, PF_MATH_FLOAT d, PF_MATH_FLOAT t)
{
    PF_MATH_FLOAT t2 = t * t;
    PF_MATH_FLOAT a0 = d - c - a + b;
    PF_MATH_FLOAT a1 = a - b - a0;
    PF_MATH_FLOAT a2 = c - a;
    PF_MATH_FLOAT a3 = b;

    return (a0 * t * t2 + a1 * t2 + a2 * t + a3);
}

static inline PF_MATH_FLOAT
pf_math_cosine_interpolation(PF_MATH_FLOAT a, PF_MATH_FLOAT b, PF_MATH_FLOAT t)
{
    PF_MATH_FLOAT t2 = (1 - cos(t * M_PI)) / 2;
    return (a * (1 - t2) + b * t2);
}

static inline void
pf_math_polar_to_cartesian(PF_MATH_FLOAT radius, PF_MATH_FLOAT angle, PF_MATH_FLOAT* x, PF_MATH_FLOAT* y)
{
    *x = radius * cos(angle);
    *y = radius * sin(angle);
}

static inline void
pf_math_cartesian_to_polar(PF_MATH_FLOAT x, PF_MATH_FLOAT y, PF_MATH_FLOAT* radius, PF_MATH_FLOAT* angle)
{
    *radius = sqrt(x * x + y * y);
    *angle = atan2(y, x);
}

#endif //PFM_H
