#ifndef PF_STDINC_H
#define PF_STDINC_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __AVX2__
#   include <pmmintrin.h>
#   include <immintrin.h>
#endif //__AVX2__

#ifndef PF_MALLOC
#   define PF_MALLOC(ptr) (malloc(ptr))
#endif //PF_MALLOC

#ifndef PF_FREE
#   define PF_FREE(ptr) (free(ptr))
#endif //PF_FREE

#endif //PF_STDINC_H
