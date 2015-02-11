
namespace fg {
    namespace object2d {
        class DisplayObjectIterator final : public object2d::DisplayObjectIteratorInterface {
        public:
            DisplayObjectIterator(DisplayObjectInterface *root, platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan, float frameTimeMs);
            ~DisplayObjectIterator() override;

            DisplayObjectType      type() const override;
            DisplayObjectConstPtr  object() const override;

            bool next() override;
        
        private:
            resources::ResourceManagerInterface  &_resMan;
            platform::PlatformInterface          &_platform;
            DisplayObjectInterface               *_current;

            float _frameTimeMs;

            DisplayObjectIterator(const DisplayObjectIterator &);
            DisplayObjectIterator &operator =(const DisplayObjectIterator &);
        };
    }
}




//