
// full name for resources
// FG_RESOURCE_LONGNAME

const unsigned FG_SKIN_MATRIX_MAX = 32;
const unsigned FG_DEFAULT_LIGHTS_MAX = 1;
const unsigned FG_DEFAULT_ENV_MIPS = 6;

const unsigned FG_ENV_TEXTURES_BASE = 2;
const unsigned FG_SHADOW_TEXTURES_BASE = 4;
const unsigned FG_SHADOW_DEFAULT_RESOLUTION = 256;
const float    FG_SHADOW_DEFAULT_SPREAD = 0.6f;

#ifdef FG_RESOURCE_LONGNAME
#define FG_SIMPLE_SHADER "bin/simpleModel.shader"
#define FG_IFACE_SHADER = "bin/displayObject.shader"
#else
#define FG_SIMPLE_SHADER "simpleModel.shader"
#define FG_IFACE_SHADER "displayObject.shader"
#endif