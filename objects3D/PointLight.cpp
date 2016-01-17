
namespace fg {
    namespace object3d {
        PointLight::PointLight() {
            _type = RenderObjectType::LIGHT;
        }

        PointLight::~PointLight() {

        }

        void PointLight::setDistance(float dist) {
            _distance = dist;
        }

        void PointLight::setColor(float r, float g, float b, float a) {
            _rgba.r = r;
            _rgba.g = g;
            _rgba.b = b;
            _rgba.a = a;
        }

        void PointLight::setColor(const fg::color &rgba) {
            _rgba = rgba;
        }

        float PointLight::getDistance() const {
            return _distance;
        }

        const fg::color &PointLight::getColor() const {
            return _rgba;
        }

        const math::m4x4 &PointLight::getFullTransform() const {
            return _fullTransform;
        }

        bool PointLight::isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) {
            return true;
        }

        unsigned PointLight::getComponentCount() const {
            return 0;
        }

        RenderObjectComponentInterface *PointLight::getComponentInterface(unsigned index) {
            return nullptr;
        }
    }
}

