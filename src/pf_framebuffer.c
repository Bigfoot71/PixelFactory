#include "pixelfactory/pf_framebuffer.h"
#include "pixelfactory/pf_stdinc.h"
#include "pixelfactory/pf_helper.h"
#include <stdint.h>

pf_framebuffer_t
pf_framebuffer_create(
    uint32_t w, uint32_t h,
    pf_color_t def)
{
    pf_framebuffer_t result = { 0 };
    if (w == 0 || h == 0) return result;

    pf_color_t* buffer = PF_MALLOC(w*h*sizeof(pf_color_t));
    if (buffer == NULL) return result;

    pf_color_t* end = buffer + w*h;
    pf_color_t* ptr = buffer;
    while (ptr < end) {
        *ptr++ = def;
    }

    result.buffer = buffer;
    result.w = w;
    result.h = h;

    return result;
}

void
pf_framebuffer_delete(
    pf_framebuffer_t* fb)
{
    PF_FREE(fb->buffer);
    fb->buffer = NULL;
    fb->w = fb->h = 0;
}

int
pf_framebuffer_is_valid(
    const pf_framebuffer_t* fb)
{
    return (fb->buffer != NULL || fb->w > 0 || fb->h > 0);
}

void
pf_framebuffer_copy(
    pf_framebuffer_t * restrict dst_fb,
    const uint32_t dst_rect[4],
    const pf_framebuffer_t * restrict src_fb,
    const uint32_t src_rect[4])
{
    int dst_xmin, dst_ymin, dst_xmax, dst_ymax;
    int src_xmin, src_ymin, src_xmax, src_ymax;

    if (dst_fb == NULL || dst_fb->buffer == NULL
     || src_fb == NULL || src_fb->buffer == NULL) {
        return;
    }

    if (dst_rect == NULL) {
        dst_xmin = dst_ymin = 0;
        dst_xmax = dst_fb->w - 1;
        dst_ymax = dst_fb->h - 1;
    } else {
        dst_xmin = PF_MIN(dst_rect[0], dst_fb->w - 1);
        dst_ymin = PF_MIN(dst_rect[1], dst_fb->h - 1);
        dst_xmax = PF_MIN(dst_rect[2], dst_fb->w - 1);
        dst_ymax = PF_MIN(dst_rect[3], dst_fb->h - 1);
        if (dst_xmin > dst_xmax) PF_SWAP(dst_xmin, dst_xmax);
        if (dst_ymin > dst_ymax) PF_SWAP(dst_ymin, dst_ymax);
    }

    if (src_rect == NULL) {
        src_xmin = src_ymin = 0;
        src_xmax = src_fb->w - 1;
        src_ymax = src_fb->h - 1;
    } else {
        src_xmin = PF_MIN(src_rect[0], src_fb->w - 1);
        src_ymin = PF_MIN(src_rect[1], src_fb->h - 1);
        src_xmax = PF_MIN(src_rect[2], src_fb->w - 1);
        src_ymax = PF_MIN(src_rect[3], src_fb->h - 1);
        if (src_xmin > src_xmax) PF_SWAP(src_xmin, src_xmax);
        if (src_ymin > src_ymax) PF_SWAP(src_ymin, src_ymax);
    }

    float inv_dst_w = 1.0f / (float)dst_fb->w;
    float inv_dst_h = 1.0f / (float)dst_fb->h;

    for (int y = dst_ymin; y <= dst_ymax; ++y)
    {
        size_t y_dst_offset = y * dst_fb->w;
        size_t y_src_offset = y * inv_dst_h * (src_ymax - src_ymin) + src_ymin;

        for (int x = dst_xmin; x <= dst_xmax; ++x)
        {
            size_t dst_offset = y_dst_offset + x;
            size_t src_offset = y_src_offset + x * inv_dst_w * (src_xmax - src_xmin) + src_xmin;

            dst_fb->buffer[dst_offset] = src_fb->buffer[src_offset];
        }
    }
}

void
pf_framebuffer_copy_pixels(
    void * restrict dst,
    const pf_framebuffer_t * restrict src_fb,
    const uint32_t src_rect[4])
{
    int xmin, ymin, xmax, ymax;
    if (dst == NULL || src_fb == NULL || src_fb->buffer == NULL) {
        return;
    }
    if (src_rect == NULL) {
        xmin = ymin = 0;
        xmax = src_fb->w - 1;
        ymax = src_fb->h - 1;
    } else {
        xmin = PF_MIN(src_rect[0], src_fb->w - 1);
        ymin = PF_MIN(src_rect[1], src_fb->h - 1);
        xmax = PF_MIN(src_rect[2], src_fb->w - 1);
        ymax = PF_MIN(src_rect[3], src_fb->h - 1);
        if (xmin > xmax) PF_SWAP(xmin, xmax);
        if (ymin > ymax) PF_SWAP(ymin, ymax);
    }
    uint32_t *ptr = dst;
    for (int y = ymin; y <= ymax; ++y) {
        for (int x = xmin; x <= xmax; ++x) {
            *ptr++ = src_fb->buffer[y * src_fb->w + x].v;
        }
    }
}

pf_color_t
pf_framebuffer_get(
    const pf_framebuffer_t* fb,
    uint32_t x, uint32_t y)
{
    pf_color_t result = { 0 };
    if (x < fb->w && y < fb->h) {
        result = fb->buffer[y * fb->w + x];
    }
    return result;
}

void
pf_framebuffer_put(
    pf_framebuffer_t* fb,
    uint32_t x, uint32_t y,
    pf_color_t color)
{
    if (x < fb->w && y < fb->h) {
        fb->buffer[y * fb->w + x] = color;
    }
}

void
pf_framebuffer_fill(
    pf_framebuffer_t* fb,
    const uint32_t rect[4],
    pf_color_t color)
{
    int xmin, ymin, xmax, ymax;
    if (fb == NULL || fb->buffer == NULL) {
        return;
    }
    if (rect == NULL) {
        xmin = ymin = 0;
        xmax = fb->w - 1;
        ymax = fb->h - 1;
    } else {
        xmin = PF_MIN(rect[0], fb->w - 1);
        ymin = PF_MIN(rect[1], fb->h - 1);
        xmax = PF_MIN(rect[2], fb->w - 1);
        ymax = PF_MIN(rect[3], fb->h - 1);
        if (xmin > xmax) PF_SWAP(xmin, xmax);
        if (ymin > ymax) PF_SWAP(ymin, ymax);
    }
    for (int y = ymin; y <= ymax; ++y) {
        pf_color_t* ptr = fb->buffer + y * fb->w + xmin;
        for (int x = xmin; x <= xmax; ++x) {
            *ptr++ = color;
        }
    }
}

void
pf_framebuffer_map(
    pf_framebuffer_t* fb,
    const uint32_t rect[4],
    pf_framebuffer_map_fn func)
{
    int xmin, ymin, xmax, ymax;
    if (fb == NULL || fb->buffer == NULL) {
        return;
    }
    if (rect == NULL) {
        xmin = ymin = 0;
        xmax = fb->w - 1;
        ymax = fb->h - 1;
    } else {
        xmin = PF_MIN(rect[0], fb->w - 1);
        ymin = PF_MIN(rect[1], fb->h - 1);
        xmax = PF_MIN(rect[2], fb->w - 1);
        ymax = PF_MIN(rect[3], fb->h - 1);
        if (xmin > xmax) PF_SWAP(xmin, xmax);
        if (ymin > ymax) PF_SWAP(ymin, ymax);
    }
    for (int y = ymin; y <= ymax; ++y) {
        pf_color_t* ptr = fb->buffer + y * fb->w + xmin;
        for (int x = xmin; x <= xmax; ++x) {
            func(fb, ptr++, x, y);
        }
    }
}
