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

#ifndef PF_STDINC_H
#define PF_STDINC_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef PF_MALLOC
#   define PF_MALLOC(size) (malloc(size))
#endif //PF_MALLOC

#ifndef PF_CALLOC
#   define PF_CALLOC(count, size) (calloc(count, size))
#endif //PF_CALLOC

#ifndef PF_FREE
#   define PF_FREE(ptr) (free(ptr))
#endif //PF_FREE

#endif //PF_STDINC_H
