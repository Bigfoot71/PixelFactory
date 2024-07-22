#ifndef PF_DEPTHBUFFER_H
#define PF_DEPTHBUFFER_H

#include <stdint.h>

typedef struct {
    float* buffer;
    uint32_t w;
    uint32_t h;
} pf_depthbuffer_t;

pf_depthbuffer_t
pf_depthbuffer_create(uint32_t w, uint32_t h, float def);

void
pf_depthbuffer_delete(pf_depthbuffer_t* fb);

int
pf_depthbuffer_is_valid(const pf_depthbuffer_t* fb);

void
pf_depthbuffer_put(pf_depthbuffer_t* fb, uint32_t x, uint32_t y, float depth);

void
pf_depthbuffer_fill(pf_depthbuffer_t* fb, const uint32_t rect[4], float depth);

#endif //PF_DEPTHBUFFER_H
