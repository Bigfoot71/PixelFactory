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
