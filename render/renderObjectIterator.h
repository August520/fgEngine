
namespace fg {
    namespace object3d {
        class RenderObjectIterator final : public RenderObjectIteratorInterface {
        public:
            RenderObjectIterator(RenderObjectInterface *root, platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan, float frameTimeMs);
            ~RenderObjectIterator() override;

            RenderObjectType          type() const override;
            RenderObjectConstPtr      object() const override;            
            RenderObjectComponentPtr  component() const override;

            bool next() override;

        private:
            resources::ResourceManagerInterface  &_resMan;
            platform::PlatformInterface          &_platform;
            
            RenderObjectInterface  *_current;            
            RenderObjectInterface::ComponentInterface *_curComponent;

            unsigned  _componentCount;
            unsigned  _componentIndex;
            float     _frameTimeMs;

            RenderObjectIterator(const RenderObjectIterator &);
            RenderObjectIterator &operator =(const RenderObjectIterator &);
        };
    }
}



//