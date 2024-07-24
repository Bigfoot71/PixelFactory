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

#ifndef PF_HELPER_H
#define PF_HELPER_H

#ifndef PF_MIN
#   define PF_MIN(x, y) ((x) < (y) ? (x) : (y))
#endif //PF_MIN

#ifndef PF_MAX
#   define PF_MAX(x, y) ((x) > (y) ? (x) : (y))
#endif //PF_MAX

#ifndef PF_CLAMP
#   define PF_CLAMP(v, min, max) (PF_MAX((min), PF_MIN((v), (max))))
#endif //PF_CLAMP

#ifndef PF_MIN_255
#   define PF_MIN_255(n) ( \
    (uint8_t)((int)(n) | ((255 - (int)(n)) >> 31)))
#endif //PF_MIN_255

#ifndef PF_MAX_0
#   define PF_MAX_0(n) (\
    (uint8_t)((int)(n) & -((int)(n) >= 0)))
#endif //PF_MAX_0

#ifndef PF_SWAP
#   define PF_SWAP(x, y) {      \
    __typeof__(x) __tmp = (x);  \
    (x) = (y);                  \
    (y) = __tmp;                \
}
#endif //PF_SWAP

#endif //PF_HELPER_H
