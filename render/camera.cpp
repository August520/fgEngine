
namespace fg {
    namespace render {
        Camera::Camera(const platform::EnginePlatformInterface &iplatform) : _platform(iplatform) {
            _fov = 90.0f;
            _zNear = 0.1f;
            _zFar = 1000.0f;

            setLookAtByRight(math::p3d(8, 8, 8), math::p3d(3, 3, 3), math::p3d(1, 0, -1));
        }

        Camera::~Camera() {
        
        }

        void Camera::set(const CameraInterface &cam) {
            const Camera &ncam = static_cast <const Camera &> (cam);

            _position = ncam._position;
            _target = ncam._target;
            _upVector = ncam._upVector;
            _fov = ncam._fov;
            _zNear = ncam._zNear;
            _zFar = ncam._zFar;
            _viewMatrix = ncam._viewMatrix;
            _projMatrix = ncam._projMatrix;
        }

        void Camera::setLookAtByUp(const math::p3d &pos, const math::p3d &target, const math::p3d &upVector) {
            _position = pos;
            _target = target;
            _upVector = upVector;
            _upVector.normalize();
            _updateMatrix();
        }

        void Camera::setLookAtByRight(const math::p3d &pos, const math::p3d &target, const math::p3d &rightVector) {
            _position = pos;
            _target = target;

            math::p3d look = target - pos;
            math::p3d right = rightVector;
            look.normalize();
            right.normalize();

            _upVector.cross(right, look);
            _updateMatrix();
        }

        void Camera::setOrientation(const math::quat &q) {
            math::p3d look(0, 0, -1);
            math::p3d up(0, 1, 0);

            look.transform(q);
            up.transform(q);

            _target = _position + look;
            _upVector = up;
            _updateMatrix();
        }

        void Camera::setTransform(const math::m4x4 &transform) {
            math::p3d look(0, 0, -1);
            math::p3d up(0, 1, 0);
            math::p3d pos(0, 0, 0);

            look.transform(transform, false);
            up.transform(transform, false);
            pos.transform(transform, true);

            _target = _position + look;
            _upVector = up;
            _position = pos;
            _updateMatrix();
        }
        
        void Camera::setPerspectiveProj(float fov, float zNear, float zFar) {
            _fov = fov;
            _zNear = zNear;
            _zFar = zFar;
            _updateMatrix();
        }

        const math::p3d &Camera::getPosition() const {
            return _position;
        }

        const math::p3d &Camera::getTarget() const {
            return _target;
        }

        math::m4x4 Camera::getVPMatrix() const {
            return _viewMatrix * _projMatrix;
        }

        math::m4x4 Camera::getOrientedVPMatrix() const {
            return _viewMatrix * _projMatrix * _platform.getOrientationTransform3D();
        }

        math::p3d Camera::screenToWorld(const math::p2d &screenCoord) const {
            math::m4x4 tv = _viewMatrix;
            tv._41 = 0.0f;
            tv._42 = 0.0f;
            tv._43 = 0.0f;

            math::m4x4 tvp = tv * _projMatrix;
            tvp.inverse();

            math::p3d tcoord = screenCoord;
            tcoord.x = 2.0f * tcoord.x / _platform.getCurrentRTWidth() - 1.0f;
            tcoord.y = 1.0f - 2.0f * tcoord.y / _platform.getCurrentRTHeight();
            tcoord.transform(tvp, true);
            tcoord.normalize();
            return tcoord;
        }

        math::p2d Camera::worldToScreen(const math::p3d &pointInWorld) const {
            math::p3d tpos = pointInWorld;

            tpos.transform(_viewMatrix * _projMatrix, true);
            tpos.x /= tpos.z;
            tpos.y /= tpos.z;
            tpos.x = (tpos.x + 1.0f) * 0.5f * _platform.getCurrentRTWidth();
            tpos.y = (1.0f - tpos.y) * 0.5f * _platform.getCurrentRTHeight();
            return math::p2d(tpos.x, tpos.y);
        }

        void Camera::_updateMatrix() {
            float aspect = _platform.getCurrentRTWidth() / _platform.getCurrentRTHeight();
            _viewMatrix.lookAt(_position, _target, _upVector);
            _projMatrix.perspectiveFov(_fov / 180.0f * float(M_PI) * 0.5f, aspect, _zNear, _zFar);
        }
    }
}



//