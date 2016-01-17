
namespace fg {
    namespace object3d {
        class RenderObjectComponentCmp final {
        public:
            RenderObjectComponentCmp(render::CameraInterface *camera) : _camera(camera) {}
            inline bool operator()(const object3d::RenderObjectComponentInterface *left, const object3d::RenderObjectComponentInterface *right) const {
                float ldx = left->_fullTransform._41 - _camera->_position.x;
                float ldy = left->_fullTransform._42 - _camera->_position.y;
                float ldz = left->_fullTransform._43 - _camera->_position.z;
                float rdx = right->_fullTransform._41 - _camera->_position.x;
                float rdy = right->_fullTransform._42 - _camera->_position.y;
                float rdz = right->_fullTransform._43 - _camera->_position.z;
                return (ldx * ldx + ldy * ldy + ldz * ldz) < (rdx * rdx + rdy * rdy + rdz * rdz);
            }

        private:
            render::CameraInterface *_camera;
        };
    }

    namespace render {
        SceneComposition::SceneComposition(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan, CameraInterface *cam)
            : _platform(platform)
            , _resMan(resMan)
            , _camera(cam)
            , _regularMeshes(platform, resMan)
            , _transparentMeshes(platform, resMan)
            , _particleEmitters(platform, resMan)
            , _pointLights(platform, resMan)
            , _sprites(platform, resMan)
            , _textFields(platform, resMan)
        {}

        SceneComposition::~SceneComposition() {

        }

        OrderedCollection <object3d::Model3DInterface::MeshComponentInterface> &SceneComposition::getRegularMeshEnumerator() {
            _regularMeshes.resetIteration();
            return _regularMeshes;
        }

        OrderedCollection <object3d::Model3DInterface::MeshComponentInterface> &SceneComposition::getTransparentMeshEnumerator() {
            _transparentMeshes.resetIteration();
            return _transparentMeshes;
        }

        OrderedCollection <object3d::Particles3DInterface::EmitterComponentInterface> &SceneComposition::getParticleEmitterEnumerator() {
            _particleEmitters.resetIteration();
            return _particleEmitters;
        }

        OrderedCollection <object3d::PointLightInterface> &SceneComposition::getPointLightEnumerator() {
            _pointLights.resetIteration();
            return _pointLights;
        }

        OrderedCollection <object2d::Sprite2DInterface> &SceneComposition::getSprite2DEnumerator() {
            _sprites.resetIteration();
            return _sprites;
        }

        OrderedCollection <object2d::TextFieldInterface> &SceneComposition::getTextFieldEnumerator() {
            _textFields.resetIteration();
            return _textFields;
        }

        void SceneComposition::addRenderObject(object3d::RenderObjectInterface *ptr) {
            _renderObjects.emplace(ptr, false);
        }

        void SceneComposition::addDisplayObject(object2d::DisplayObjectInterface *ptr) {
            _displayObjects.emplace(ptr, false);
        }

        void SceneComposition::removeRenderObject(object3d::RenderObjectInterface *ptr) {
            _renderObjects.erase(ptr);
            _removeRenderObject(ptr);

            // remove from sorted array
        }

        void SceneComposition::removeDisplayObject(object2d::DisplayObjectInterface *ptr) {
            _displayObjects.erase(ptr);

            // remove from sorted array
        }

        void SceneComposition::update(float frameTimeMs) {
            for (auto index = std::begin(_renderObjects); index != std::end(_renderObjects); ++index) {
                if (index->first->isResourcesReady(_platform, _resMan)) {
                    index->first->updateCoordinates(frameTimeMs, _resMan);

                    if (index->second == false) { // add to sorted array                        
                        object3d::RenderObjectInterface *cur = index->first;

                        if (cur->getType() == object3d::RenderObjectType::MODEL) {
                            for (unsigned i = 0, cnt = cur->getComponentCount(); i < cnt; i++) {
                                object3d::Model3DInterface::MeshComponentInterface *comp = static_cast <object3d::Model3DInterface::MeshComponentInterface *> (cur->getComponentInterface(i));
                                _regularMeshes.add(comp, object3d::RenderObjectComponentCmp(_camera));
                            }
                        }

                        index->second = true;
                    }
                }
                else {
                    if (index->second == true) { // remove from sorted array                        
                        object3d::RenderObjectInterface *cur = index->first;
                        index->second = false;

                        _removeRenderObject(cur);
                    }
                }
            }

            //---

            for (auto index = std::begin(_displayObjects); index != std::end(_displayObjects); ++index) {
                index->first->updateCoordinates(frameTimeMs, _resMan);
                
                if (index->first->isResourcesReady(_platform, _resMan)) {                    
                    if (index->second == false) {
                        // add to sorted array

                        index->second = true;
                    }
                }
                else {
                    if (index->second == true) {
                        // remove from sorted array

                        index->second = false;
                    }
                }
            }

            //---

            _regularMeshes.sort(object3d::RenderObjectComponentCmp(_camera));
        }


        void SceneComposition::_removeRenderObject(object3d::RenderObjectInterface *ptr) {
            if (ptr->getType() == object3d::RenderObjectType::MODEL) {
                for (unsigned i = 0, cnt = ptr->getComponentCount(); i < cnt; i++) {
                    object3d::Model3DInterface::MeshComponentInterface *comp = static_cast <object3d::Model3DInterface::MeshComponentInterface *> (ptr->getComponentInterface(i));
                    _regularMeshes.remove(comp, object3d::RenderObjectComponentCmp(_camera));
                }
            }
        }

        void SceneComposition::_removeDisplayObject(object2d::DisplayObjectInterface *ptr) {

        }
    }
}


