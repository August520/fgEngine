
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
            const fg::color &getColor() const override;
        
            bool  isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) override;

        protected:
            float     _distance = 0.0f;
            fg::color _rgba;
        };
    }
}

