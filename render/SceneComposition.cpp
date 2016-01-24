
namespace fg {
    bool inline less(float a, float b) {
        return a < b;
    }

    bool inline greater(float a, float b) {
        return a > b;
    }

    namespace object3d {
        template <bool (*op)(float, float)> class RenderObjectComponentCmp final {
        public:
            RenderObjectComponentCmp(const render::CameraInterface *camera) : _camera(camera) {}
            inline bool operator()(const object3d::RenderObjectComponentInterface *left, const object3d::RenderObjectComponentInterface *right) const {
                float ldx = left->_fullTransform._41 - _camera->_position.x;
                float ldy = left->_fullTransform._42 - _camera->_position.y;
                float ldz = left->_fullTransform._43 - _camera->_position.z;
                float rdx = right->_fullTransform._41 - _camera->_position.x;
                float rdy = right->_fullTransform._42 - _camera->_position.y;
                float rdz = right->_fullTransform._43 - _camera->_position.z;
                return op((ldx * ldx + ldy * ldy + ldz * ldz), (rdx * rdx + rdy * rdy + rdz * rdz));
            }

        private:
            const render::CameraInterface *_camera;
        };

        class RenderObjectCmp final {
        public:
            RenderObjectCmp(const math::p3d &cameraPosition) : _cameraPosition(cameraPosition) {}
            inline bool operator()(const object3d::RenderObjectInterface *left, const object3d::RenderObjectInterface *right) const {
                auto &leftTransform = left->getFullTransform();
                auto &rightTransform = right->getFullTransform();

                float ldx = leftTransform._41 - _cameraPosition.x;
                float ldy = leftTransform._42 - _cameraPosition.y;
                float ldz = leftTransform._43 - _cameraPosition.z;
                float rdx = rightTransform._41 - _cameraPosition.x;
                float rdy = rightTransform._42 - _cameraPosition.y;
                float rdz = rightTransform._43 - _cameraPosition.z;
                return (ldx * ldx + ldy * ldy + ldz * ldz) < (rdx * rdx + rdy * rdy + rdz * rdz);
            }

        private:
            const math::p3d &_cameraPosition;
        };
    }

    namespace object2d {
        inline bool DisplayObjectCmp(const object2d::DisplayObjectInterface *left, const object2d::DisplayObjectInterface *right) {
            return left->getZ() < right->getZ() + FLT_MIN;
        }
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
        }

        void SceneComposition::removeDisplayObject(object2d::DisplayObjectInterface *ptr) {
            _displayObjects.erase(ptr);
            _removeDisplayObject(ptr);
        }

        void SceneComposition::update(float frameTimeMs) {
            for (auto index = std::begin(_renderObjects); index != std::end(_renderObjects); ++index) {
                object3d::RenderObjectInterface *cur = index->first;
                
                if (index->first->isResourcesReady(_platform, _resMan)) {
                    index->first->updateCoordinates(frameTimeMs, _resMan);
                    
                    if (index->second == false) { // add to sorted array                        
                        index->second = true;

                        if (cur->getType() == object3d::RenderObjectType::MODEL) {
                            for (unsigned i = 0, cnt = cur->getComponentCount(); i < cnt; i++) {
                                object3d::Model3DInterface::MeshComponentInterface *comp = static_cast <object3d::Model3DInterface::MeshComponentInterface *> (cur->getComponentInterface(i));
                                
                                if (comp->isTransparent()) {
                                    _transparentMeshes.add(comp, object3d::RenderObjectComponentCmp<greater>(_camera));
                                }
                                else {
                                    _regularMeshes.add(comp, object3d::RenderObjectComponentCmp<less>(_camera));
                                }
                            }
                        }
                        else if (cur->getType() == object3d::RenderObjectType::PARTICLES) {
                            for (unsigned i = 0, cnt = cur->getComponentCount(); i < cnt; i++) {
                                object3d::Particles3DInterface::EmitterComponentInterface *comp = static_cast <object3d::Particles3DInterface::EmitterComponentInterface *> (cur->getComponentInterface(i));
                                _particleEmitters.add(comp, object3d::RenderObjectComponentCmp<greater>(_camera));
                            }
                        }
                        else if (cur->getType() == object3d::RenderObjectType::LIGHT) {
                            _pointLights.add(static_cast<object3d::PointLightInterface *>(cur), object3d::RenderObjectCmp(_camera->getPosition()));
                        }
                    }
                }
                else {
                    if (index->second == true) { // remove from sorted array  
                        index->second = false;
                        _removeRenderObject(cur);
                    }
                }
            }

            //---

            for (auto index = std::begin(_displayObjects); index != std::end(_displayObjects); ++index) {
                index->first->updateCoordinates(frameTimeMs, _resMan);
                object2d::DisplayObjectInterface *cur = index->first;

                if (index->first->isResourcesReady(_platform, _resMan)) {                    
                    if (index->second == false) { // add to sorted array
                        index->second = true;

                        if (cur->getType() == object2d::DisplayObjectType::SPRITE) {
                            _sprites.add(static_cast<object2d::Sprite2DInterface *>(cur), object2d::DisplayObjectCmp);
                        }
                        else if (cur->getType() == object2d::DisplayObjectType::TEXTFIELD) {
                            _textFields.add(static_cast<object2d::TextFieldInterface *>(cur), object2d::DisplayObjectCmp);
                        }
                    }
                }
                else {
                    if (index->second == true) { // remove from sorted array
                        index->second = false;
                        _removeDisplayObject(cur);
                    }
                }
            }

            //---

            _regularMeshes.sort(object3d::RenderObjectComponentCmp<less>(_camera));
            _transparentMeshes.sort(object3d::RenderObjectComponentCmp<greater>(_camera));
            _particleEmitters.sort(object3d::RenderObjectComponentCmp<greater>(_camera));
            _pointLights.sort(object3d::RenderObjectCmp(_camera->getPosition()));
        }


        void SceneComposition::_removeRenderObject(object3d::RenderObjectInterface *ptr) {
            if (ptr->getType() == object3d::RenderObjectType::MODEL) {
                for (unsigned i = 0, cnt = ptr->getComponentCount(); i < cnt; i++) {
                    object3d::Model3DInterface::MeshComponentInterface *comp = static_cast <object3d::Model3DInterface::MeshComponentInterface *> (ptr->getComponentInterface(i));

                    if (comp->isTransparent()) {
                        _transparentMeshes.remove(comp, object3d::RenderObjectComponentCmp<greater>(_camera));
                    }
                    else {
                        _regularMeshes.remove(comp, object3d::RenderObjectComponentCmp<less>(_camera));
                    }
                }
            }
            else if (ptr->getType() == object3d::RenderObjectType::PARTICLES) {
                for (unsigned i = 0, cnt = ptr->getComponentCount(); i < cnt; i++) {
                    object3d::Particles3DInterface::EmitterComponentInterface *comp = static_cast <object3d::Particles3DInterface::EmitterComponentInterface *> (ptr->getComponentInterface(i));
                    _particleEmitters.remove(comp, object3d::RenderObjectComponentCmp<greater>(_camera));
                }
            }
            else if (ptr->getType() == object3d::RenderObjectType::LIGHT) {
                _pointLights.remove(static_cast<object3d::PointLightInterface *>(ptr), object3d::RenderObjectCmp(_camera->getPosition()));
            }
        }

        void SceneComposition::_removeDisplayObject(object2d::DisplayObjectInterface *ptr) {
            if (ptr->getType() == object2d::DisplayObjectType::SPRITE) {
                _sprites.remove(static_cast<object2d::Sprite2DInterface *>(ptr), object2d::DisplayObjectCmp);
            }
            else if (ptr->getType() == object2d::DisplayObjectType::TEXTFIELD) {
                _textFields.remove(static_cast<object2d::TextFieldInterface *>(ptr), object2d::DisplayObjectCmp);
            }
        }
    }
}


