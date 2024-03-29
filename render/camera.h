
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
            void  setInterestOffset(float offset) override;

            const math::p3d &getPosition() const override;
            const math::p3d &getTarget() const override;
            const math::p3d &getInterestPoint() const override;
            
            const math::p3d &getForwardDir() const override;
            const math::p3d &getRightDir() const override;
            const math::p3d &getUpDir() const override;
            
            float getZNear() const override;
            float getZFar() const override;

            math::m4x4  getVPMatrix() const override;
            math::p3d   screenToWorld(const math::p2d &screenCoord) const override;
            math::p2d   worldToScreen(const math::p3d &pointInWorld) const override;

            void updateMatrix() override;

        protected:
            const platform::EnginePlatformInterface  &_platform;

            math::p3d   _target;
            math::p3d   _pointOfInterest;
            math::p3d   _upVector;
            math::p3d   _rightVector;
            math::p3d   _forwardVector;
            
            float       _fov = 90.0f;
            float       _zNear = 0.1f;
            float       _zFar = 100.0f;
            float       _interestOffset = 0.0f;
            math::m4x4  _viewMatrix;
            math::m4x4  _projMatrix;
                        
        private:
            Camera(const Camera &);
            Camera &operator =(const Camera &);
        };
    }
}
