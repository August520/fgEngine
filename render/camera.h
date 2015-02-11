
namespace fg {
    namespace render {
        class Camera : public CameraInterface{
        public:
            Camera(const platform::EnginePlatformInterface &iplatform);
            virtual ~Camera();
            
            void  set(const CameraInterface &cam) override;
            void  setLookAtByUp(const math::p3d &pos, const math::p3d &target, const math::p3d &upVector) override;
            void  setLookAtByRight(const math::p3d &pos, const math::p3d &target, const math::p3d &rightVector) override;
            void  setOrientation(const math::quat &q) override;
            void  setTransform(const math::m4x4 &transform) override;
            void  setPerspectiveProj(float fov, float zNear, float zFar) override;

            const math::p3d &getPosition() const override;
            const math::p3d &getTarget() const override;
            
            math::m4x4  getVPMatrix() const override;
            math::m4x4  getOrientedVPMatrix() const override;
            math::p3d   screenToWorld(const math::p2d &screenCoord) const override;
            math::p2d   worldToScreen(const math::p3d &pointInWorld) const override;

        protected:
            const platform::EnginePlatformInterface  &_platform;

            math::p3d   _position;
            math::p3d   _target;
            math::p3d   _upVector;
            float       _fov;
            float       _zNear;
            float       _zFar;
            math::m4x4  _viewMatrix;
            math::m4x4  _projMatrix;

            void _updateMatrix();
                        
        private:
            Camera(const Camera &);
            Camera &operator =(const Camera &);
        };
    }
}
