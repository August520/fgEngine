
// surface description flags
const uint32_t DDSF_CAPS = 0x00000001;
const uint32_t DDSF_HEIGHT = 0x00000002;
const uint32_t DDSF_WIDTH = 0x00000004;
const uint32_t DDSF_PITCH = 0x00000008;
const uint32_t DDSF_PIXELFORMAT = 0x00001000;
const uint32_t DDSF_MIPMAPCOUNT = 0x00020000;
const uint32_t DDSF_LINEARSIZE = 0x00080000;
const uint32_t DDSF_DEPTH = 0x00800000;

// pixel format flags
const uint32_t DDSF_ALPHAPIXELS = 0x00000001;
const uint32_t DDSF_FOURCC = 0x00000004;
const uint32_t DDSF_RGB = 0x00000040;
const uint32_t DDSF_RGBA = 0x00000041;

// dwCaps1 flags
const uint32_t DDSF_COMPLEX = 0x00000008;
const uint32_t DDSF_TEXTURE = 0x00001000;
const uint32_t DDSF_MIPMAP = 0x00400000;

// dwCaps2 flags
const uint32_t DDSF_CUBEMAP = 0x00000200;
const uint32_t DDSF_CUBEMAP_POSITIVEX = 0x00000400;
const uint32_t DDSF_CUBEMAP_NEGATIVEX = 0x00000800;
const uint32_t DDSF_CUBEMAP_POSITIVEY = 0x00001000;
const uint32_t DDSF_CUBEMAP_NEGATIVEY = 0x00002000;
const uint32_t DDSF_CUBEMAP_POSITIVEZ = 0x00004000;
const uint32_t DDSF_CUBEMAP_NEGATIVEZ = 0x00008000;
const uint32_t DDSF_CUBEMAP_ALL_FACES = 0x0000FC00;
const uint32_t DDSF_VOLUME = 0x00200000;

// compressed texture types
const uint32_t FOURCC_DXT1 = 0x31545844; //(MAKEFOURCC('D','X','T','1'))
const uint32_t FOURCC_DXT3 = 0x33545844; //(MAKEFOURCC('D','X','T','3'))
const uint32_t FOURCC_DXT5 = 0x35545844; //(MAKEFOURCC('D','X','T','5'))

struct DDS_PIXELFORMAT {
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwFourCC;
    uint32_t dwRGBBitCount;
    uint32_t dwRBitMask;
    uint32_t dwGBitMask;
    uint32_t dwBBitMask;
    uint32_t dwABitMask;
};

struct DDS_HEADER {
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwHeight;
    uint32_t dwWidth;
    uint32_t dwPitchOrLinearSize;
    uint32_t dwDepth;
    uint32_t dwMipMapCount;
    uint32_t dwReserved1[11];
    DDS_PIXELFORMAT ddspf;
    uint32_t dwCaps1;
    uint32_t dwCaps2;
    uint32_t dwReserved2[3];
};

struct DXTColBlock {
    uint16_t col0;
    uint16_t col1;
    uint8_t  row[4];
};

struct DXT3AlphaBlock {
    uint16_t row[4];
};

struct DXT5AlphaBlock {
    uint8_t alpha0;
    uint8_t alpha1;
    uint8_t row[6];
};

fg::platform::TextureFormat getDDSFormat(const DDS_HEADER &header);
unsigned getDDSImageSize(fg::platform::TextureFormat fmt, unsigned w, unsigned h);