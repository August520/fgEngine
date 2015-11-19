#pragma once
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_DEF static

#ifdef STB_TRUETYPE_IMPLEMENTATION
// #define your own (u)stbtt_int8/16/32 before including to override this
#ifndef stbtt_uint8
typedef unsigned char   stbtt_uint8;
typedef signed   char   stbtt_int8;
typedef unsigned short  stbtt_uint16;
typedef signed   short  stbtt_int16;
typedef unsigned int    stbtt_uint32;
typedef signed   int    stbtt_int32;
#endif

typedef char stbtt__check_size32[sizeof(stbtt_int32) == 4 ? 1 : -1];
typedef char stbtt__check_size16[sizeof(stbtt_int16) == 2 ? 1 : -1];

// #define your own STBTT_ifloor/STBTT_iceil() to avoid math.h
#ifndef STBTT_ifloor
#include <math.h>
#define STBTT_ifloor(x)   ((int) floor(x))
#define STBTT_iceil(x)    ((int) ceil(x))
#endif

#ifndef STBTT_sqrt
#include <math.h>
#define STBTT_sqrt(x)      sqrt(x)
#endif

// #define your own functions "STBTT_malloc" / "STBTT_free" to avoid malloc.h
#ifndef STBTT_malloc
#include <stdlib.h>
#define STBTT_malloc(x,u)  ((void)(u),malloc(x))
#define STBTT_free(x,u)    ((void)(u),free(x))
#endif

#ifndef STBTT_assert
#include <assert.h>
#define STBTT_assert(x)    assert(x)
#endif

#ifndef STBTT_strlen
#include <string.h>
#define STBTT_strlen(x)    strlen(x)
#endif

#ifndef STBTT_memcpy
#include <memory.h>
#define STBTT_memcpy       memcpy
#define STBTT_memset       memset
#endif
#endif

// The following structure is defined publically so you can declare one on
// the stack or as a global or etc, but you should treat it as opaque.
typedef struct stbtt_fontinfo
{
    void           * userdata;
    unsigned char  * data;              // pointer to .ttf file
    int              fontstart;         // offset of start of font

    int numGlyphs;                     // number of glyphs, needed for range checking

    int loca, head, glyf, hhea, hmtx, kern; // table locations as offset from start of .ttf
    int index_map;                     // a cmap mapping for our chosen character encoding
    int indexToLocFormat;              // format needed to map from glyph index to glyph
} stbtt_fontinfo;

STBTT_DEF int   stbtt_InitFont(stbtt_fontinfo *info, const unsigned char *data, int offset);
STBTT_DEF int   stbtt_FindGlyphIndex(const stbtt_fontinfo *info, int unicode_codepoint);
STBTT_DEF float stbtt_ScaleForPixelHeight(const stbtt_fontinfo *info, float pixels);
STBTT_DEF void  stbtt_GetFontVMetrics(const stbtt_fontinfo *info, int *ascent, int *descent, int *lineGap);
STBTT_DEF void  stbtt_GetGlyphHMetrics(const stbtt_fontinfo *info, int glyph_index, int *advanceWidth, int *leftSideBearing);
STBTT_DEF void  stbtt_GetGlyphBitmapBoxSubpixel(const stbtt_fontinfo *font, int glyph, float scale_x, float scale_y, float shift_x, float shift_y, int *ix0, int *iy0, int *ix1, int *iy1);
STBTT_DEF void  stbtt_MakeGlyphBitmapSubpixel(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int glyph);

