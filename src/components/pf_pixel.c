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

#include "pixelfactory/components/pf_pixel.h"

/* Internal convert functions */

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC push_options
#pragma GCC optimize ("no-strict-aliasing")
#endif

static uint16_t FloatToHalf(float x)
{
    const uint32_t b = (*(uint32_t*)&x)+0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
    const uint32_t e = (b&0x7F800000)>>23; // exponent
    const uint32_t m = b&0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
    return (b&0x80000000)>>16 | (e>112)*((((e-112)<<10)&0x7C00)|m>>13) | ((e<113)&(e>101))*((((0x007FF000+m)>>(125-e))+1)>>1) | (e>143)*0x7FFF; // sign : normalized : denormalized : saturate
}

float HalfToFloat(uint16_t x)
{
    const uint32_t e = (x&0x7C00)>>10; // exponent
    const uint32_t m = (x&0x03FF)<<13; // mantissa
    const float fm = (float)m;
    const uint32_t v = (*(uint32_t*)&fm)>>23; // evil log2 bit hack to count leading zeros in denormalized format
    const uint32_t r = (x&0x8000)<<16 | (e!=0)*((e+112)<<23|m) | ((e==0)&(m!=0))*((v-37)<<23|((m<<(150-v))&0x007FE000)); // sign : normalized : denormalized
    return *(float*)&r;
}

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC pop_options
#endif

pf_color_t
pf_pixel_get_gray(
    const void* pixels,
    size_t offset)
{
    pf_color_t result;
    uint8_t gray = ((uint8_t*)pixels)[offset];
    result.c.r = result.c.g = result.c.b = gray;
    result.c.a = 255;
    return result;
}

pf_color_t
pf_pixel_get_gray_alpha(
    const void* pixels,
    size_t offset)
{
    pf_color_t result;
    uint8_t *pixel = (uint8_t*)pixels + 2 * offset;
    result.c.r = result.c.g = result.c.b = *pixel;
    result.c.a = *(pixel + 1);
    return result;
}

pf_color_t
pf_pixel_get_rgb565(
    const void* pixels,
    size_t offset)
{
    pf_color_t result;
    uint16_t pixel = ((uint16_t*)pixels)[offset];
    result.c.r = (uint8_t)((float)((pixel & 0xF800) >> 11) * (255.0f/31));  // 0b1111100000000000
    result.c.g = (uint8_t)((float)((pixel & 0x7E0) >> 5) * (255.0f/63));    // 0b0000011111100000
    result.c.b = (uint8_t)((float)(pixel & 0x1F) * (255.0f/31));            // 0b0000000000011111
    result.c.a = 255;
    return result;
}

pf_color_t
pf_pixel_get_rgb888(
    const void* pixels,
    size_t offset)
{
    pf_color_t result;
    const uint8_t* pixel = (uint8_t*)pixels + offset*3;
    result.c.r = pixel[0];
    result.c.g = pixel[1];
    result.c.b = pixel[2];
    result.c.a = 255;
    return result;
}

pf_color_t
pf_pixel_get_rgba5551(
    const void* pixels,
    size_t offset)
{
    pf_color_t result;
    uint16_t pixel = ((uint16_t*)pixels)[offset];
    result.c.r = (uint8_t)((float)((pixel & 0xF800) >> 11) * (255.0f/31));  // 0b1111100000000000
    result.c.g = (uint8_t)((float)((pixel & 0x7C0) >> 6) * (255.0f/31));    // 0b0000011111000000
    result.c.b = (uint8_t)((float)((pixel & 0x3E) >> 1) * (255.0f/31));     // 0b0000000000111110
    result.c.a = (uint8_t)((pixel & 0x1) * 255);                            // 0b0000000000000001
    return result;
}

pf_color_t
pf_pixel_get_rgba4444(
    const void* pixels,
    size_t offset)
{
    pf_color_t result;
    uint16_t pixel = ((uint16_t*)pixels)[offset];
    result.c.r = (uint8_t)((float)((pixel & 0xF000) >> 12) * (255.0f/15));  // 0b1111000000000000
    result.c.g = (uint8_t)((float)((pixel & 0xF00) >> 8) * (255.0f/15));    // 0b0000111100000000
    result.c.b = (uint8_t)((float)((pixel & 0xF0) >> 4) * (255.0f/15));     // 0b0000000011110000
    result.c.a = (uint8_t)((float)(pixel & 0xF) * (255.0f/15));             // 0b0000000000001111
    return result;
}

pf_color_t
pf_pixel_get_rgba8888(
    const void* pixels,
    size_t offset)
{
    return ((pf_color_t*)pixels)[offset];
}

pf_color_t
pf_pixel_get_r32(
    const void* pixels,
    size_t offset)
{
    pf_color_t result;
    result.c.r = (uint8_t)(((float*)pixels)[offset] * 255.0f);
    result.c.g = result.c.b = 0;
    result.c.a = 255;
    return result;
}

pf_color_t
pf_pixel_get_rgb323232(
    const void* pixels,
    size_t offset)
{
    pf_color_t result;
    const float *pixel = (float*)pixels + 3 * offset;
    result.c.r = pixel[0] * 255.0f;
    result.c.g = pixel[1] * 255.0f;
    result.c.b = pixel[2] * 255.0f;
    result.c.a = 255;
    return result;
}

pf_color_t
pf_pixel_get_rgba32323232(
    const void* pixels,
    size_t offset)
{
    pf_color_t result;
    const float *pixel = (float*)pixels + 4 * offset;
    result.c.r = (uint8_t)(pixel[0] * 255.0f);
    result.c.g = (uint8_t)(pixel[1] * 255.0f);
    result.c.b = (uint8_t)(pixel[2] * 255.0f);
    result.c.a = (uint8_t)(pixel[3] * 255.0f);
    return result;
}

pf_color_t
pf_pixel_get_r16(
    const void* pixels,
    size_t offset)
{
    pf_color_t result;
    result.c.r = (uint8_t)(HalfToFloat(((uint16_t*)pixels)[offset]) * 255.0f);
    result.c.g = result.c.b = 0;
    result.c.a = 255;
    return result;
}

pf_color_t
pf_pixel_get_rgb161616(
    const void* pixels,
    size_t offset)
{
    pf_color_t result;
    const uint16_t *pixel = (uint16_t*)pixels + 3 * offset;
    result.c.r = (uint8_t)(HalfToFloat(pixel[0]) * 255.0f);
    result.c.g = (uint8_t)(HalfToFloat(pixel[1]) * 255.0f);
    result.c.b = (uint8_t)(HalfToFloat(pixel[2]) * 255.0f);
    result.c.a = 255;
    return result;
}

pf_color_t
pf_pixel_get_rgba16161616(
    const void* pixels,
    size_t offset)
{
    pf_color_t result;
    const uint16_t *pixel = (uint16_t*)pixels + 4 * offset;
    result.c.r = (uint8_t)(HalfToFloat(pixel[0]) * 255.0f);
    result.c.g = (uint8_t)(HalfToFloat(pixel[1]) * 255.0f);
    result.c.b = (uint8_t)(HalfToFloat(pixel[2]) * 255.0f);
    result.c.a = (uint8_t)(HalfToFloat(pixel[3]) * 255.0f);
    return result;
}

/* Pixel Setters */

void
pf_pixel_set_gray(
    void* pixels,
    size_t offset,
    pf_color_t color)
{
    // NOTE: Calculate grayscale equivalent color
    float r = color.c.r * (1.0f/255);
    float g = color.c.g * (1.0f/255);
    float b = color.c.b * (1.0f/255);

    ((uint8_t*)pixels)[offset] = (uint8_t)(255.0f * (
        r * 0.299f + g * 0.587f + b * 0.114f));
}

void
pf_pixel_set_gray_alpha(
    void* pixels,
    size_t offset,
    pf_color_t color)
{
    // NOTE: Calculate grayscale equivalent color
    float r = color.c.r * (1.0f/255);
    float g = color.c.g * (1.0f/255);
    float b = color.c.b * (1.0f/255);

    uint8_t *pixel = (uint8_t*)pixels + offset*2;
    pixel[0] = (uint8_t)(255.0f * (r * 0.299f + g * 0.587f + b * 0.114f));
    pixel[1] = color.c.a;
}

void
pf_pixel_set_rgb565(
    void* pixels,
    size_t offset,
    pf_color_t color)
{
    // NOTE: Calculate RGB565 equivalent color
    float rf = color.c.r * (1.0f/255);
    float gf = color.c.g * (1.0f/255);
    float bf = color.c.b * (1.0f/255);

    uint8_t r = (uint8_t)(roundf(rf * 31.0f));
    uint8_t g = (uint8_t)(roundf(gf * 63.0f));
    uint8_t b = (uint8_t)(roundf(bf * 31.0f));

    ((uint16_t*)pixels)[offset] = (uint16_t)r << 11 | (uint16_t)g << 5 | (uint16_t)b;
}

void
pf_pixel_set_rgb888(
    void* pixels,
    size_t offset,
    pf_color_t color)
{
    uint8_t* pixel = (uint8_t*)pixels + 3 * offset;
    pixel[0] = color.c.r;
    pixel[1] = color.c.g;
    pixel[2] = color.c.b;
}

void
pf_pixel_set_rgba5551(
    void* pixels,
    size_t offset,
    pf_color_t color)
{
    // NOTE: Calculate RGB5551 equivalent color
    float rf = color.c.r * (1.0f/255);
    float gf = color.c.g * (1.0f/255);
    float bf = color.c.b * (1.0f/255);
    float af = color.c.a * (1.0f/255);

    uint8_t r = (uint8_t)(roundf(rf * 31.0f));
    uint8_t g = (uint8_t)(roundf(gf * 31.0f));
    uint8_t b = (uint8_t)(roundf(bf * 31.0f));
    uint8_t a = (af > 0.5f) ? 1 : 0;

    ((uint16_t*)pixels)[offset] = (uint16_t)r << 11 | (uint16_t)g << 6 | (uint16_t)b << 1 | (uint16_t)a;
}

void
pf_pixel_set_rgba4444(
    void* pixels,
    size_t offset,
    pf_color_t color)
{
    // NOTE: Calculate RGB4444 equivalent color
    float rf = color.c.r * (1.0f/255);
    float gf = color.c.g * (1.0f/255);
    float bf = color.c.b * (1.0f/255);
    float af = color.c.a * (1.0f/255);

    uint8_t r = (uint8_t)(roundf(rf * 15.0f));
    uint8_t g = (uint8_t)(roundf(gf * 15.0f));
    uint8_t b = (uint8_t)(roundf(bf * 15.0f));
    uint8_t a = (uint8_t)(roundf(af * 15.0f));

    ((uint16_t*)pixels)[offset] = (uint16_t)r << 12 | (uint16_t)g << 8 | (uint16_t)b << 4 | (uint16_t)a;
}

void
pf_pixel_set_rgba8888(
    void* pixels,
    size_t offset,
    pf_color_t color)
{
    ((uint32_t*)pixels)[offset] = *(uint32_t*)(&color);
}

void
pf_pixel_set_r32(
    void* pixels,
    size_t offset,
    pf_color_t color)
{
    // NOTE: Calculate grayscale equivalent color (normalized to 32bit)
    float rf = color.c.r * (1.0f/255);
    float gf = color.c.g * (1.0f/255);
    float bf = color.c.b * (1.0f/255);

    ((float*)pixels)[offset] = rf * 0.299f + gf * 0.587f + bf * 0.114f;
}

void
pf_pixel_set_rgb323232(
    void* pixels,
    size_t offset,
    pf_color_t color)
{
    // NOTE: Calculate RGB323232 equivalent color (normalized to 32bit)
    float nCol[3] = {
        color.c.r * (1.0f/255),
        color.c.g * (1.0f/255),
        color.c.b * (1.0f/255)
    };

    memcpy((float*)pixels + 3 * offset, nCol, sizeof(nCol));
}

void
pf_pixel_set_rgba32323232(
    void* pixels,
    size_t offset,
    pf_color_t color)
{
    // NOTE: Calculate RGBA32323232 equivalent color (normalized to 32bit)
    float nCol[4] = {
        color.c.r * (1.0f/255),
        color.c.g * (1.0f/255),
        color.c.b * (1.0f/255),
        color.c.a * (1.0f/255)
    };

    memcpy((float*)pixels + 4 * offset, nCol, sizeof(nCol));
}

void
pf_pixel_set_r16(
    void* pixels,
    size_t offset,
    pf_color_t color)
{
    // NOTE: Calculate grayscale equivalent color (normalized to 32bit)
    ((uint16_t*)pixels)[offset] = FloatToHalf(
        color.c.r * (1.0f/255) * 0.299f +
        color.c.g * (1.0f/255) * 0.587f +
        color.c.b * (1.0f/255) * 0.114f);
}

void
pf_pixel_set_rgb161616(
    void* pixels,
    size_t offset,
    pf_color_t color)
{
    // NOTE: Calculate RGB161616 equivalent color (normalized to 32bit)
    uint16_t *pixel = (uint16_t*)pixels + offset*3;
    pixel[0] = FloatToHalf(color.c.r * (1.0f/255));
    pixel[1] = FloatToHalf(color.c.g * (1.0f/255));
    pixel[2] = FloatToHalf(color.c.b * (1.0f/255));
}

void
pf_pixel_set_rgba16161616(
    void* pixels,
    size_t offset,
    pf_color_t color)
{
    // NOTE: Calculate RGBA16161616 equivalent color (normalized to 32bit)
    uint16_t *pixel = (uint16_t*)pixels + offset*4;
    pixel[0] = FloatToHalf(color.c.r * (1.0f/255));
    pixel[1] = FloatToHalf(color.c.g * (1.0f/255));
    pixel[2] = FloatToHalf(color.c.b * (1.0f/255));
    pixel[3] = FloatToHalf(color.c.a * (1.0f/255));
}


/* Helper Functions */

void
pf_pixel_default_getter_setter(
    pf_pixel_getter_fn* getter,
    pf_pixel_setter_fn* setter,
    pf_pixelformat_t format)
{
    switch (format)
    {
        case PF_PIXELFORMAT_GRAY:
            if (getter) *getter = pf_pixel_get_gray;
            if (setter) *setter = pf_pixel_set_gray;
            break;

        case PF_PIXELFORMAT_GRAY_ALPHA:
            if (getter) *getter = pf_pixel_get_gray_alpha;
            if (setter) *setter = pf_pixel_set_gray_alpha;
            break;

        case PF_PIXELFORMAT_RGB565:
            if (getter) *getter = pf_pixel_get_rgb565;
            if (setter) *setter = pf_pixel_set_rgb565;
            break;

        case PF_PIXELFORMAT_RGBA5551:
            if (getter) *getter = pf_pixel_get_rgba5551;
            if (setter) *setter = pf_pixel_set_rgba5551;
            break;

        case PF_PIXELFORMAT_RGBA4444:
            if (getter) *getter = pf_pixel_get_rgba4444;
            if (setter) *setter = pf_pixel_set_rgba4444;
            break;

        case PF_PIXELFORMAT_RGB888:
            if (getter) *getter = pf_pixel_get_rgb888;
            if (setter) *setter = pf_pixel_set_rgb888;
            break;

        case PF_PIXELFORMAT_RGBA8888:
            if (getter) *getter = pf_pixel_get_rgba8888;
            if (setter) *setter = pf_pixel_set_rgba8888;
            break;

        case PF_PIXELFORMAT_R32:
            if (getter) *getter = pf_pixel_get_r32;
            if (setter) *setter = pf_pixel_set_r32;
            break;

        case PF_PIXELFORMAT_RGB323232:
            if (getter) *getter = pf_pixel_get_rgb323232;
            if (setter) *setter = pf_pixel_set_rgb323232;
            break;

        case PF_PIXELFORMAT_RGBA32323232:
            if (getter) *getter = pf_pixel_get_rgba32323232;
            if (setter) *setter = pf_pixel_set_rgba32323232;
            break;

        case PF_PIXELFORMAT_R16:
            if (getter) *getter = pf_pixel_get_r16;
            if (setter) *setter = pf_pixel_set_r16;
            break;

        case PF_PIXELFORMAT_RGB161616:
            if (getter) *getter = pf_pixel_get_rgb161616;
            if (setter) *setter = pf_pixel_set_rgb161616;
            break;

        case PF_PIXELFORMAT_RGBA16161616:
            if (getter) *getter = pf_pixel_get_rgba16161616;
            if (setter) *setter = pf_pixel_set_rgba16161616;
            break;

        default:
            if (getter) *getter = NULL;
            if (setter) *setter = NULL;
            break;
    }
}

size_t
pf_pixel_get_bytes(
    pf_pixelformat_t format)
{
    switch (format)
    {
        case PF_PIXELFORMAT_GRAY:      return 1;
        case PF_PIXELFORMAT_GRAY_ALPHA:
        case PF_PIXELFORMAT_RGB565:
        case PF_PIXELFORMAT_RGBA5551:
        case PF_PIXELFORMAT_RGBA4444:       return 2;
        case PF_PIXELFORMAT_RGBA8888:       return 4;
        case PF_PIXELFORMAT_RGB888:         return 3;
        case PF_PIXELFORMAT_R32:            return 4;
        case PF_PIXELFORMAT_RGB323232:      return 4*3;
        case PF_PIXELFORMAT_RGBA32323232:   return 4*4;
        case PF_PIXELFORMAT_R16:            return 2;
        case PF_PIXELFORMAT_RGB161616:      return 2*3;
        case PF_PIXELFORMAT_RGBA16161616:   return 2*4;

        default:
            break;
    }

    return 0;
}
