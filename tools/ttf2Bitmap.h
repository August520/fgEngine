
// stb_truetype.h - v0.6c - public domain
// authored from 2009-2012 by Sean Barrett / RAD Game Tools
//
//   This library processes TrueType files:
//        parse files
//        extract glyph metrics
//        extract glyph shapes
//        render glyphs to one-channel bitmaps with antialiasing (box filter)
//
//   Todo:
//        non-MS cmaps
//        crashproof on bad data
//        hinting? (no longer patented)
//        cleartype-style AA?
//        optimize: use simple memory allocator for intermediates
//        optimize: build edge-list directly from curves
//        optimize: rasterize directly from curves?
//
// ADDITIONAL CONTRIBUTORS
//
//   Mikko Mononen: compound shape support, more cmap formats
//   Tor Andersson: kerning, subpixel rendering
//
//   Bug/warning reports:
//       "Zer" on mollyrocket (with fix)
//       Cass Everitt
//       stoiko (Haemimont Games)
//       Brian Hook 
//       Walter van Niftrik
//
// VERSION HISTORY
//
//   0.6c (2012-07-24) improve documentation
//   0.6b (2012-07-20) fix a few more warnings
//   0.6  (2012-07-17) fix warnings; added stbtt_ScaleForMappingEmToPixels,
//                        stbtt_GetFontBoundingBox, stbtt_IsGlyphEmpty
//   0.5  (2011-12-09) bugfixes:
//                        subpixel glyph renderer computed wrong bounding box
//                        first vertex of shape can be off-curve (FreeSans)
//   0.4b (2011-12-03) fixed an error in the font baking example
//   0.4  (2011-12-01) kerning, subpixel rendering (tor)
//                    bugfixes for:
//                        codepoint-to-glyph conversion using table fmt=12
//                        codepoint-to-glyph conversion using table fmt=4
//                        stbtt_GetBakedQuad with non-square texture (Zer)
//                    updated Hello World! sample to use kerning and subpixel
//                    fixed some warnings
//   0.3  (2009-06-24) cmap fmt=12, compound shapes (MM)
//                    userdata, malloc-from-userdata, non-zero fill (STB)
//   0.2  (2009-03-11) Fix unsigned/signed char warnings
//   0.1  (2009-03-09) First public release
//
// LICENSE
//
//   This software is in the public domain. Where that dedication is not
//   recognized, you are granted a perpetual, irrevokable license to copy
//   and modify this file as you see fit.
//
// USAGE
//
//   Include this file in whatever places neeed to refer to it. In ONE C/C++
//   file, write:
//      #define STB_TRUETYPE_IMPLEMENTATION
//   before the #include of this file. This expands out the actual
//   implementation into that C/C++ file.
//
//   Simple 3D API (don't ship this, but it's fine for tools and quick start,
//                  and you can cut and paste from it to move to more advanced)
//           stbtt_BakeFontBitmap()               -- bake a font to a bitmap for use as texture
//           stbtt_GetBakedQuad()                 -- compute quad to draw for a given char
//
//   "Load" a font file from a memory buffer (you have to keep the buffer loaded)
//           stbtt_InitFont()
//           stbtt_GetFontOffsetForIndex()        -- use for TTC font collections
//
//   Render a unicode codepoint to a bitmap
//           stbtt_GetCodepointBitmap()           -- allocates and returns a bitmap
//           stbtt_MakeCodepointBitmap()          -- renders into bitmap you provide
//           stbtt_GetCodepointBitmapBox()        -- how big the bitmap must be
//
//   Character advance/positioning
//           stbtt_GetCodepointHMetrics()
//           stbtt_GetFontVMetrics()
//           stbtt_GetCodepointKernAdvance()
//
// ADDITIONAL DOCUMENTATION
//
//   Immediately after this block comment are a series of sample programs.
//
//   After the sample programs is the "header file" section. This section
//   includes documentation for each API function.
//
//   Some important concepts to understand to use this library:
//
//      Codepoint
//         Characters are defined by unicode codepoints, e.g. 65 is
//         uppercase A, 231 is lowercase c with a cedilla, 0x7e30 is
//         the hiragana for "ma".
//
//      Glyph
//         A visual character shape (every codepoint is rendered as
//         some glyph)
//
//      Glyph index
//         A font-specific integer ID representing a glyph
//
//      Baseline
//         Glyph shapes are defined relative to a baseline, which is the
//         bottom of uppercase characters. Characters extend both above
//         and below the baseline.
//
//      Current Point
//         As you draw text to the screen, you keep track of a "current point"
//         which is the origin of each character. The current point's vertical
//         position is the baseline. Even "baked fonts" use this model.
//
//      Vertical Font Metrics
//         The vertical qualities of the font, used to vertically position
//         and space the characters. See docs for stbtt_GetFontVMetrics.
//
//      Font Size in Pixels or Points
//         The preferred interface for specifying font sizes in stb_truetype
//         is to specify how tall the font's vertical extent should be in pixels.
//         If that sounds good enough, skip the next paragraph.
//
//         Most font APIs instead use "points", which are a common typographic
//         measurement for describing font size, defined as 72 points per inch.
//         stb_truetype provides a point API for compatibility. However, true
//         "per inch" conventions don't make much sense on computer displays
//         since they different monitors have different number of pixels per
//         inch. For example, Windows traditionally uses a convention that
//         there are 96 pixels per inch, thus making 'inch' measurements have
//         nothing to do with inches, and thus effectively defining a point to
//         be 1.333 pixels. Additionally, the TrueType font data provides
//         an explicit scale factor to scale a given font's glyphs to points,
//         but the author has observed that this scale factor is often wrong
//         for non-commercial fonts, thus making fonts scaled in points
//         according to the TrueType spec incoherently sized in practice.
//
// ADVANCED USAGE
//
//   Quality:
//
//    - Use the functions with Subpixel at the end to allow your characters
//      to have subpixel positioning. Since the font is anti-aliased, not
//      hinted, this is very import for quality. (This is not possible with
//      baked fonts.)
//
//    - Kerning is now supported, and if you're supporting subpixel rendering
//      then kerning is worth using to give your text a polished look.
//
//   Performance:
//
//    - Convert Unicode codepoints to glyph indexes and operate on the glyphs;
//      if you don't do this, stb_truetype is forced to do the conversion on
//      every call.
//
//    - There are a lot of memory allocations. We should modify it to take
//      a temp buffer and allocate from the temp buffer (without freeing),
//      should help performance a lot.
//
// NOTES
//
//   The system uses the raw data found in the .ttf file without changing it
//   and without building auxiliary data structures. This is a bit inefficient
//   on little-endian systems (the data is big-endian), but assuming you're
//   caching the bitmaps or glyph shapes this shouldn't be a big deal.
//
//   It appears to be very hard to programmatically determine what font a
//   given file is in a general way. I provide an API for this, but I don't
//   recommend it.
//
//
// SOURCE STATISTICS (based on v0.6c, 2050 LOC)
//
//   Documentation & header file        520 LOC  \___ 660 LOC documentation
//   Sample code                        140 LOC  /
//   Truetype parsing                   620 LOC  ---- 620 LOC TrueType
//   Software rasterization             240 LOC  \                           .
//   Curve tesselation                  120 LOC   \__ 550 LOC Bitmap creation
//   Bitmap management                  100 LOC   /
//   Baked bitmap interface              70 LOC  /
//   Font name matching & access        150 LOC  ---- 150 
//   C runtime library abstraction       60 LOC  ----  60
//   

#define STB_TRUETYPE_IMPLEMENTATION

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
////
////   INTEGRATION WITH YOUR CODEBASE
////
////   The following sections allow you to supply alternate definitions
////   of C library functions used by stb_truetype.

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

typedef char stbtt__check_size32[sizeof(stbtt_int32)==4 ? 1 : -1];
typedef char stbtt__check_size16[sizeof(stbtt_int16)==2 ? 1 : -1];

// #define your own STBTT_sort() to override this to avoid qsort
#ifndef STBTT_sort
#include <stdlib.h>
#define STBTT_sort(data,num_items,item_size,compare_func)   qsort(data,num_items,item_size,compare_func)
#endif

// #define your own STBTT_ifloor/STBTT_iceil() to avoid math.h
#ifndef STBTT_ifloor
#include <math.h>
#define STBTT_ifloor(x)   ((int) floor(x))
#define STBTT_iceil(x)    ((int) ceil(x))
#endif

// #define your own functions "STBTT_malloc" / "STBTT_free" to avoid malloc.h
#ifndef STBTT_malloc
#include <malloc.h>
#define STBTT_malloc(x,u)  malloc(x)//_staticAlloc(x)
#define STBTT_free(x,u)    free(x)//_staticFree(x)
#endif

#ifndef STBTT_assert
#include <assert.h>
#define STBTT_assert(x)    {}
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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////
////   INTERFACE
////
////

#ifndef __STB_INCLUDE_STB_TRUETYPE_H__
#define __STB_INCLUDE_STB_TRUETYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

  // The following structure is defined publically so you can declare one on
  // the stack or as a global or etc, but you should treat it as opaque.
  typedef struct stbtt_fontinfo
  {
    void           * userdata;
    unsigned char  * data;              // pointer to .ttf file
    int              fontstart;         // offset of start of font

    int numGlyphs;                     // number of glyphs, needed for range checking

    int loca,head,glyf,hhea,hmtx,kern; // table locations as offset from start of .ttf
    int index_map;                     // a cmap mapping for our chosen character encoding
    int indexToLocFormat;              // format needed to map from glyph index to glyph
  } stbtt_fontinfo;

  #ifndef STBTT_vmove // you can predefine these to use different values (but why?)
  enum {
    STBTT_vmove=1,
    STBTT_vline,
    STBTT_vcurve
  };
  #endif

  #ifndef stbtt_vertex // you can predefine this to use different values
  // (we share this with other code at RAD)
  #define stbtt_vertex_type short // can't use stbtt_int16 because that's not visible in the header file
  typedef struct
  {
    stbtt_vertex_type x,y,cx,cy;
    unsigned char type,padding;
  } stbtt_vertex;
  #endif

  // @TODO: don't expose this structure
  typedef struct
  {
    int w,h,stride;
    unsigned char *pixels;
  } stbtt__bitmap;

  extern int stbtt_InitFont(stbtt_fontinfo *info, const unsigned char *data, int offset);
  // Given an offset into the file that defines a font, this function builds
  // the necessary cached info for the rest of the system. You must allocate
  // the stbtt_fontinfo yourself, and stbtt_InitFont will fill it out. You don't
  // need to do anything special to free it, because the contents are pure
  // value data with no additional data structures. Returns 0 on failure.

  extern float stbtt_ScaleForPixelHeight(const stbtt_fontinfo *info, float pixels);
  // computes a scale factor to produce a font whose "height" is 'pixels' tall.
  // Height is measured as the distance from the highest ascender to the lowest
  // descender; in other words, it's equivalent to calling stbtt_GetFontVMetrics
  // and computing:
  //       scale = pixels / (ascent - descent)
  // so if you prefer to measure height by the ascent only, use a similar calculation.

  extern float stbtt_ScaleForMappingEmToPixels(const stbtt_fontinfo *info, float pixels);
  // computes a scale factor to produce a font whose EM size is mapped to
  // 'pixels' tall. This is probably what traditional APIs compute, but
  // I'm not positive.

  extern void stbtt_GetFontVMetrics(const stbtt_fontinfo *info, int *ascent, int *descent, int *lineGap);
  // ascent is the coordinate above the baseline the font extends; descent
  // is the coordinate below the baseline the font extends (i.e. it is typically negative)
  // lineGap is the spacing between one row's descent and the next row's ascent...
  // so you should advance the vertical position by "*ascent - *descent + *lineGap"
  //   these are expressed in unscaled coordinates, so you must multiply by
  //   the scale factor for a given size

  extern void stbtt_GetFontBoundingBox(const stbtt_fontinfo *info, int *x0, int *y0, int *x1, int *y1);
  // the bounding box around all possible characters

  extern void stbtt_GetGlyphHMetrics(const stbtt_fontinfo *info, int glyph_index, int *advanceWidth, int *leftSideBearing);
  extern int  stbtt_GetGlyphKernAdvance(const stbtt_fontinfo *info, int glyph1, int glyph2);
  // as above, but takes one or more glyph indices for greater efficiency


  //-------------------------------------------------------------------------


  int stbtt_FindGlyphIndex(const stbtt_fontinfo *info, int unicode_codepoint);
  // If you're going to perform multiple operations on the same character
  // and you want a speed-up, call this function with the character you're
  // going to process, then use glyph-based functions instead of the
  // codepoint-based functions.

  extern int stbtt_GetGlyphShape(const stbtt_fontinfo *info, int glyph_index, stbtt_vertex **vertices);
  // returns # of vertices and fills *vertices with the pointer to them
  //   these are expressed in "unscaled" coordinates

  extern void stbtt_MakeGlyphBitmapSubpixel(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int glyph, int x_off, int y_off);
  extern void stbtt_GetGlyphBitmapBoxSubpixel(const stbtt_fontinfo *font, int glyph, float scale_x, float scale_y,float shift_x, float shift_y, int *ix0, int *iy0, int *ix1, int *iy1);
  extern int  stbtt_GetGlyphBox(const stbtt_fontinfo *info, int glyph_index, int *x0, int *y0, int *x1, int *y1);
  extern void stbtt_Rasterize(stbtt__bitmap *result, float flatness_in_pixels, stbtt_vertex *vertices, int num_verts, float scale_x, float scale_y, float shift_x, float shift_y, int x_off, int y_off, int invert, void *userdata);

  enum { // platformID
    STBTT_PLATFORM_ID_UNICODE   =0,
    STBTT_PLATFORM_ID_MAC       =1,
    STBTT_PLATFORM_ID_ISO       =2,
    STBTT_PLATFORM_ID_MICROSOFT =3
  };

  enum { // encodingID for STBTT_PLATFORM_ID_UNICODE
    STBTT_UNICODE_EID_UNICODE_1_0    =0,
    STBTT_UNICODE_EID_UNICODE_1_1    =1,
    STBTT_UNICODE_EID_ISO_10646      =2,
    STBTT_UNICODE_EID_UNICODE_2_0_BMP=3,
    STBTT_UNICODE_EID_UNICODE_2_0_FULL=4
  };

  enum { // encodingID for STBTT_PLATFORM_ID_MICROSOFT
    STBTT_MS_EID_SYMBOL        =0,
    STBTT_MS_EID_UNICODE_BMP   =1,
    STBTT_MS_EID_SHIFTJIS      =2,
    STBTT_MS_EID_UNICODE_FULL  =10
  };

//   enum { // encodingID for STBTT_PLATFORM_ID_MAC; same as Script Manager codes
//     STBTT_MAC_EID_ROMAN        =0,   STBTT_MAC_EID_ARABIC       =4,
//     STBTT_MAC_EID_JAPANESE     =1,   STBTT_MAC_EID_HEBREW       =5,
//     STBTT_MAC_EID_CHINESE_TRAD =2,   STBTT_MAC_EID_GREEK        =6,
//     STBTT_MAC_EID_KOREAN       =3,   STBTT_MAC_EID_RUSSIAN      =7
//   };

//   enum { // languageID for STBTT_PLATFORM_ID_MICROSOFT; same as LCID...
//     // problematic because there are e.g. 16 english LCIDs and 16 arabic LCIDs
//     STBTT_MS_LANG_ENGLISH     =0x0409,   STBTT_MS_LANG_ITALIAN     =0x0410,
//     STBTT_MS_LANG_CHINESE     =0x0804,   STBTT_MS_LANG_JAPANESE    =0x0411,
//     STBTT_MS_LANG_DUTCH       =0x0413,   STBTT_MS_LANG_KOREAN      =0x0412,
//     STBTT_MS_LANG_FRENCH      =0x040c,   STBTT_MS_LANG_RUSSIAN     =0x0419,
//     STBTT_MS_LANG_GERMAN      =0x0407,   STBTT_MS_LANG_SPANISH     =0x0409,
//     STBTT_MS_LANG_HEBREW      =0x040d,   STBTT_MS_LANG_SWEDISH     =0x041D
//   };

//   enum { // languageID for STBTT_PLATFORM_ID_MAC
//     STBTT_MAC_LANG_ENGLISH      =0 ,   STBTT_MAC_LANG_JAPANESE     =11,
//     STBTT_MAC_LANG_ARABIC       =12,   STBTT_MAC_LANG_KOREAN       =23,
//     STBTT_MAC_LANG_DUTCH        =4 ,   STBTT_MAC_LANG_RUSSIAN      =32,
//     STBTT_MAC_LANG_FRENCH       =1 ,   STBTT_MAC_LANG_SPANISH      =6 ,
//     STBTT_MAC_LANG_GERMAN       =2 ,   STBTT_MAC_LANG_SWEDISH      =5 ,
//     STBTT_MAC_LANG_HEBREW       =10,   STBTT_MAC_LANG_CHINESE_SIMPLIFIED =33,
//     STBTT_MAC_LANG_ITALIAN      =3 ,   STBTT_MAC_LANG_CHINESE_TRAD =19
//   };

#ifdef __cplusplus
}
#endif

#endif // __STB_INCLUDE_STB_TRUETYPE_H__