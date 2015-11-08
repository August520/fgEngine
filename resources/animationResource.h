
namespace fg {
    namespace resources {
        class AnimationResource : public AnimationResourceInterface, public Resource {
        public:
            AnimationResource(const char *path, bool unloadable) : Resource(path, unloadable) {}
            ~AnimationResource() override;

            void  loaded(const diag::LogInterface &log) override;
            bool  constructed(const diag::LogInterface &log, platform::PlatformInterface &api) override;
            void  unloaded() override;

            bool getMatrix(const fg::string &boneName, float animKoeff, bool cycled, math::m4x4 &oMatrix) const override;
            bool getTransform(const fg::string &boneName, float animKoeff, bool cycled, math::p3d &oTranslation, math::quat &oRotation, math::p3d &oScaling) const override;
            bool getTransformWithoutInterpolate(const fg::string &boneName, float animKoeff, bool cycled, math::p3d &oTranslation, math::quat &oRotation, math::p3d &oScaling) const override;

        protected:
            struct AnimKey {
                math::quat  localRotation;
                math::p3d   localScale;
                math::p3d   localPosition;
            };

            struct BoneAnimation {
                unsigned  animKeyCount;
                AnimKey   *animationKeys;

                BoneAnimation(unsigned keyCount);
                ~BoneAnimation();
            };

            fg::StaticHash <FG_MESH_MAX, const BoneAnimation *> _boneAnims;
        };

    }
}

