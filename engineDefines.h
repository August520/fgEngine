
// full name for resources
// FG_RESOURCE_LONGNAME

// max texture slots
static const unsigned FG_TEXTURE_UNITS_MAX = 8;
//
static const unsigned FG_RENDERTARGETS_MAX = 4;
// max bones per mesh
static const unsigned FG_SKIN_MATRIX_MAX = 32;
// max default point lights
static const unsigned FG_DEFAULT_LIGHTS_MAX = 1;
// mip count in default cubemap
static const unsigned FG_DEFAULT_ENV_MIPS = 6;
// start texture slot for environment textures
static const unsigned FG_ENV_TEXTURES_BASE = 2;
// start texture slot for shadows
static const unsigned FG_SHADOW_TEXTURES_BASE = 4;
//
static const unsigned FG_SHADOW_DEFAULT_RESOLUTION = 256;
//
static const float    FG_SHADOW_DEFAULT_SPREAD = 0.1f;
// ~max meshes per model
static const unsigned FG_MESH_MAX = 128;
// ~max material entries per material resource
static const unsigned FG_MATERIAL_ENTRY_MAX = 32;
// max material texture binds
static const unsigned FG_MATERIAL_TEXTURE_MAX = 8;
// max emitters per particles source
static const unsigned FG_EMITTERS_MAX = 32;

#ifdef FG_RESOURCE_LONGNAME
#define FG_SIMPLE_SHADER "bin/simpleModel.shader"
#define FG_IFACE_SHADER = "bin/displayObject.shader"
#else
#define FG_SIMPLE_SHADER "simpleModel.shader"
#define FG_IFACE_SHADER "displayObject.shader"
#endif

