
namespace fg {
    namespace resources {
        void SoundResource::loaded(const diag::LogInterface &log) {

        }

        bool SoundResource::constructed(const diag::LogInterface &log, platform::PlatformInterface &api) {
            return true; // keep binary file in memory
        }

        void SoundResource::unloaded() {
            delete (char *)_binaryData;
            _binaryData = nullptr;
            _binarySize = 0;
        }

        unsigned SoundResource::getSize() const {
            return _binarySize;
        }

        const unsigned char *SoundResource::getChunkPtr(unsigned offset) const {
            return _binaryData ? (unsigned char *)_binaryData + offset : nullptr;
        }
    }
}


//