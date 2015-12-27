
fg::platform::TextureFormat getDDSFormat(const DDS_HEADER &header) {
    if (header.ddspf.dwFlags & DDSF_FOURCC) {
        if (header.ddspf.dwFourCC == FOURCC_DXT1) {
            return fg::platform::TextureFormat::DXT1;
        }
        else if (header.ddspf.dwFourCC == FOURCC_DXT3) {
            return fg::platform::TextureFormat::DXT3;
        }
        else if (header.ddspf.dwFourCC == FOURCC_DXT5) {
            return fg::platform::TextureFormat::DXT5;
        }
        else {
            return fg::platform::TextureFormat::UNKNOWN;
        }
    }
    else if (header.ddspf.dwRGBBitCount == 32 &&
        header.ddspf.dwRBitMask == 0x000000FF &&
        header.ddspf.dwGBitMask == 0x0000FF00 &&
        header.ddspf.dwBBitMask == 0x00FF0000 &&
        header.ddspf.dwABitMask == 0xFF000000) {
        return fg::platform::TextureFormat::RGBA8;
    }
    else if (header.ddspf.dwRGBBitCount == 8) {
        return fg::platform::TextureFormat::RED8;
    }

    return fg::platform::TextureFormat::UNKNOWN;
}


unsigned getDDSImageSize(fg::platform::TextureFormat fmt, unsigned w, unsigned h) {
    switch (fmt)
    {
    case fg::platform::TextureFormat::RGBA8:
        return w * h * 4;
    case fg::platform::TextureFormat::RED8:
        return w * h;
    case fg::platform::TextureFormat::DXT1:
        return ((w + 3) / 4) * h * 8;
    case fg::platform::TextureFormat::DXT3:
    case fg::platform::TextureFormat::DXT5:
        return ((w + 3) / 4) * h * 16;
    }

    return 0;
}