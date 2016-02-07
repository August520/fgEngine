
namespace fg {
    namespace object3d {
        class PointLight : public PointLightInterface, public RenderObject {
        public:
            PointLight();
            ~PointLight() override;

            void  setDistance(float dist) override;
            void  setColor(float r, float g, float b, float a) override;
            void  setColor(const fg::color &rgba) override;
            float getDistance() const override;
            
            const fg::color   &getColor() const override;
            const math::m4x4  &getFullTransform() const override;
        
            bool  isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) override;

            unsigned  getComponentCount() const override;
            RenderObjectComponentInterface *getComponentInterface(unsigned index) override;

        protected:
            float     _distance = 0.0f;
            fg::color _rgba;
        };
    }
}

