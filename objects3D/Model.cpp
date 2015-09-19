//TODO: animation layers

namespace fg {
    namespace object3d {
        Animator::Animator() {
        
        }

        Animator::~Animator() {
        
        }

        void Animator::getMatrix(const fg::string &boneName, math::m4x4 &oMatrix) {
            Layer &layer = _layers[0];
            
            math::quat resultRotation, tmpRotation;
            math::p3d  resultTranslation, tmpTranslation;
            math::p3d  resultScaling, tmpScaling;

            float curTimeKoeff = layer.curAnimTimePass / layer.curAnimTimeLen;
            if(layer.curAnimation->getTransform(boneName, curTimeKoeff, layer.curAnimCycled, resultTranslation, resultRotation, resultScaling)) {
                float nextAnimTime = layer.nextAnimTimePass - layer.nextAnimTimeOffset;
                
                if(layer.nextAnimation && layer.nextAnimation->valid() && nextAnimTime < layer.smoothTime) {
                    float nextTimeKoeff = layer.nextAnimTimePass / layer.nextAnimTimeLen;

                    if(nextTimeKoeff >= 1.0f) {
                        nextTimeKoeff -= 1.0f;
                    }

                    if(layer.nextAnimation->getTransform(boneName, nextTimeKoeff, layer.nextAnimCycled, tmpTranslation, tmpRotation, tmpScaling)) {
                        float smoothKoeff = nextAnimTime / layer.smoothTime;
                        
                        resultRotation.slerp(resultRotation, tmpRotation, smoothKoeff);
                        resultTranslation = resultTranslation + (tmpTranslation - resultTranslation) * smoothKoeff;
                        resultScaling = resultScaling + (tmpScaling - resultScaling) * smoothKoeff;
                    }
                }

                resultRotation.toMatrix(oMatrix);
                (*(math::p3d *)&oMatrix._11).toLength(resultScaling.x);
                (*(math::p3d *)&oMatrix._21).toLength(resultScaling.y);
                (*(math::p3d *)&oMatrix._31).toLength(resultScaling.z);

                oMatrix._41 = resultTranslation.x;
                oMatrix._42 = resultTranslation.y;
                oMatrix._43 = resultTranslation.z;
            }            
        }

        void Animator::updateAnimation(float frameTimeMs) {
            for(unsigned i = 0; i < _activeLayers; i++) {
                Layer &layer = _layers[i];

                if(layer.nextAnimation && layer.nextAnimation->valid()) {
                    layer.nextAnimTimePass += frameTimeMs;

                    if(layer.nextAnimTimePass - layer.nextAnimTimeOffset >= layer.smoothTime) {
                        layer.curAnimTimePass = layer.nextAnimTimePass;
                        layer.curAnimTimeLen = layer.nextAnimTimeLen;
                        layer.curAnimResourcePath = std::move(layer.nextAnimResourcePath);
                        layer.curAnimation = layer.nextAnimation;
                        layer.curAnimCycled = layer.nextAnimCycled;
                        layer.nextAnimation = nullptr;
                    }
                }

                layer.curAnimTimePass = std::fmod(layer.curAnimTimePass, layer.curAnimTimeLen);
                layer.curAnimTimePass += frameTimeMs;

                if(layer.curAnimTimePass >= layer.curAnimTimeLen) {
                    if(layer.curAnimCycled) {
                        layer.curAnimTimePass = 0.0f;
                    }
                    else {
                        layer.curAnimTimePass = layer.curAnimTimeLen - 1.0f;
                    }

                    if(layer.nextAnimation == nullptr && layer.animFinishCallback.isBinded()) {
                        layer.animFinishCallback();
                    }
                }
            }
        }

        void Animator::updateResources(resources::ResourceManagerInterface &resMan) {
            _activeLayers = 0;
            
            for(unsigned i = 0; i < FG_ANIM_LAYERS_MAX; i++) {
                Layer &layer = _layers[i];

                if(layer.curAnimResourcePath.empty() == false) {
                    if(layer.curAnimation == nullptr) {
                        layer.curAnimation = resMan.getResource(layer.curAnimResourcePath);

                        if(layer.curAnimation == nullptr) {
                            break;
                        }
                    }

                    if(layer.curAnimation->valid()) {
                        if(layer.nextAnimResourcePath.empty() == false) {
                            if(layer.nextAnimation == nullptr) {
                                layer.nextAnimation = resMan.getResource(layer.nextAnimResourcePath);

                                if(layer.nextAnimation == nullptr) { // || layer.nextAnimation->valid() == false
                                    break;
                                }
                            }
                        }
                    }
                    else break;
                }
                else break;                

                _activeLayers++;
            }

            _isDirty = false;
        }
        
        void Animator::playAnim(const fg::string &animResourcePath, float animLenMs, float animOffsetMs, float smoothTimeMs, bool cycled, AnimationLayer ilayer) {
            Layer &layer = _layers[unsigned(ilayer)];

            if(layer.curAnimResourcePath.empty() == true) {
                layer.curAnimation = nullptr;
                layer.curAnimResourcePath = animResourcePath;
                layer.curAnimTimeLen = animLenMs;
                layer.curAnimTimePass = animOffsetMs;
                layer.nextAnimation = nullptr;
                layer.nextAnimResourcePath = fg::string();
                layer.nextAnimTimeLen = 0.0f;
                layer.nextAnimTimePass = 0.0f;
                layer.nextAnimTimeOffset = 0.0f;
                layer.smoothTime = 0.0f;
                layer.curAnimCycled = cycled;
            }
            else { 
                layer.nextAnimation = nullptr;
                layer.nextAnimResourcePath = animResourcePath;
                layer.nextAnimTimeLen = animLenMs;
                layer.nextAnimTimePass = animOffsetMs;
                layer.nextAnimTimeOffset = animOffsetMs;
                layer.smoothTime = smoothTimeMs;
                layer.nextAnimCycled = cycled;
            }

            _isDirty = true;
        }

        void Animator::setAnimFinishCallback(const callback <void()> &cb, AnimationLayer ilayer) {
            Layer &layer = _layers[unsigned(ilayer)];
            layer.animFinishCallback = cb;
        }

        bool Animator::isDirtyResources() const {
            return _isDirty;
        }

        unsigned Animator::getActiveLayersCount() const {
            return _activeLayers;
        }

        float Animator::getAnimKoeff(AnimationLayer ilayer) const {
            const Layer &layer = _layers[unsigned(ilayer)];
            return layer.curAnimTimePass / layer.curAnimTimeLen;
        }

        //---

        Model3D::MeshData::MeshData() :
            _addtrfm(false),
            _skinned(false),
            _visible(true),
            _childs(nullptr),
            _childCount(0),
            _mesh(nullptr),
            _shader(nullptr),
            _materialParams(nullptr),
            _skinMatrixes(nullptr),
            _skinMatrixCount(0)
        {
            for(unsigned i = 0; i < resources::FG_MATERIAL_TEXTURE_MAX; i++) {
                _textureBinds[i] = nullptr;
            }
        }

        Model3D::MeshData::~MeshData() {
            for(unsigned i = 0; i < _childCount; i++) {
                delete _childs[i];
            }

            delete [] _childs;
            delete [] _skinMatrixes;
		}
        
        const math::m4x4 &Model3D::MeshData::getFullTransform() const {
            return _fullTransform;
		}

        const math::m4x4 *Model3D::MeshData::getSkinMatrixArray() const {
            return _skinMatrixes;
		}
        
        unsigned Model3D::MeshData::getSkinMatrixCount() const {
            return _skinMatrixCount;
		}

        unsigned Model3D::MeshData::getTextureBindCount() const {
            return _materialParams->textureBindCount;
		}
        
        const resources::Texture2DResourceInterface *Model3D::MeshData::getTextureBind(unsigned bindIndex) const {
            return _textureBinds[bindIndex];
		}

        const resources::ShaderResourceInterface *Model3D::MeshData::getShader() const {
            return _shader;
		}

        const resources::MeshInterface *Model3D::MeshData::getMesh() const {
            return _mesh;
        }
        
        bool Model3D::MeshData::isSkinned() const {
            return _skinned;
		}

        bool Model3D::MeshData::isVisible() const {
            return _visible;
        }
        
        bool Model3D::MeshData::isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) {
            if(_materialParams == nullptr) {
                return false;
            } 

            if(_shader == nullptr) {
                if((_shader = resMan.getResource(_materialParams->shaderPath)) == nullptr) {
                    return false;
                }
            }

            for(unsigned i = 0; i < _materialParams->textureBindCount; i++) {
                if(_textureBinds[i] == nullptr) {
                    if((_textureBinds[i] = resMan.getResource(_materialParams->textureBinds[i])) == nullptr) {
                        return false;
                    }
                }

                if(_textureBinds[i]->valid() == false) {
                    return false;
                }
            }

            return _shader->valid();
		}
        
        //---

        Model3D::Model3D() : _model(nullptr), _material(nullptr), _root(nullptr), _meshes(nullptr), _meshCount(0), _modelReady(false) {
            _type = RenderObjectType::MODEL;
        }

        Model3D::~Model3D() {
            for(unsigned i = 0; i < _meshCount; i++) {
                if(_meshes[i]->_skinned) {
                    delete _meshes[i];
                }
            }

            delete [] _meshes;
            delete _root;
		}
        
        void Model3D::setModelAndMaterial(const fg::string &mdlResourcePath, const fg::string &materialResourcePath) {
            _modelResourcePath = mdlResourcePath;
            _materialResourcePath = materialResourcePath;
            _model = nullptr;
            _material = nullptr;
		}

        void Model3D::setMaterial(const fg::string &materialResourcePath) {
            _materialResourcePath = materialResourcePath;
            _material = nullptr;
		}

        void Model3D::setMeshVisible(const fg::string &meshName, bool visible) {
            MeshData *mesh = _getOrCreateMeshByName(meshName);
            mesh->_visible = visible;
		}

        void Model3D::setMeshAdditionalTransform(const fg::string &meshName, const math::m4x4 &transform) {
            MeshData *mesh = _getOrCreateMeshByName(meshName);
            mesh->_additionalTransform = transform;
            mesh->_addtrfm = true;
		}
        
        const math::m4x4 *Model3D::getMeshTransform(const fg::string &meshName) const {
            MeshData *mesh = _meshesByName.get(meshName);

            if(mesh && mesh->_mesh) {
                return &mesh->_fullTransform;
            }
            return nullptr;
		}

        const math::m4x4 *Model3D::getMeshAdditionalTransform(const fg::string &meshName) const {
            MeshData *mesh = _getOrCreateMeshByName(meshName);
            return &mesh->_additionalTransform;
		}

        const math::m4x4 *Model3D::getHelperTransform(const fg::string &helperName) const {
            MeshData *mesh = _meshesByName.get(helperName);

            if(mesh && mesh->_mesh) {
                return &mesh->_fullTransform;
            }
            return nullptr;
		}

        const math::p3d *Model3D::getMeshBBoxMinPoint(const fg::string &meshName) const {
            MeshData *mesh = _meshesByName.get(meshName);

            if(mesh && mesh->_mesh) {
                return &mesh->_mesh->getMinBBoxPoint();
            }
            return nullptr;
		}

        const math::p3d *Model3D::getMeshBBoxMaxPoint(const fg::string &meshName) const {
            MeshData *mesh = _meshesByName.get(meshName);

            if(mesh && mesh->_mesh) {
                return &mesh->_mesh->getMaxBBoxPoint();
            }
            return nullptr;
		}
        
        bool Model3D::isMeshVisible(const fg::string &meshName) {
            MeshData *mesh = _getOrCreateMeshByName(meshName);
            return mesh->_visible;
		}
        
        void Model3D::playAnim(const fg::string &animResourcePath, float animLenMs, float animOffsetMs, float smoothTimeMs, bool cycled, AnimationLayer layer) {
            _animator.playAnim(animResourcePath, animLenMs, animOffsetMs, smoothTimeMs, cycled, layer);
		}

        void Model3D::setAnimFinishCallback(const callback <void()> &cb, AnimationLayer layer) {
            _animator.setAnimFinishCallback(cb, layer);
        }

        void Model3D::setAnimLayerKoeff(AnimationLayer layer, float koeff) const {
            //!!!
        }

        float Model3D::getAnimLayerKoeff(AnimationLayer layer) const {
            return _animator.getAnimKoeff(layer);
		}
        
        void Model3D::updateCoordinates(float frameTimeMs, resources::ResourceManagerInterface &resMan) {
            RenderObject::updateCoordinates(frameTimeMs, resMan);
            _animator.updateAnimation(frameTimeMs);

            struct fn {
                static void recursiveMeshUpdate(MeshData *curMesh, const math::m4x4 &parentFullTransform) {
                    const resources::MeshInterface *rmesh = curMesh->_mesh;

                    math::m4x4 localTransform = rmesh->getLocalTransform();
                    curMesh->_fullTransform = localTransform * parentFullTransform;

                    if(curMesh->_addtrfm) {
                        curMesh->_fullTransform = curMesh->_additionalTransform * curMesh->_fullTransform;
                    }

                    rmesh->tempMatrix = rmesh->getInvStartTransform() * curMesh->_fullTransform;

                    for(unsigned i = 0; i < curMesh->_childCount; i++) {
                        recursiveMeshUpdate(curMesh->_childs[i], curMesh->_fullTransform);
                    }
                }

                static void recursiveMeshAnimationUpdate(Animator &animator, MeshData *curMesh, const math::m4x4 &parentFullTransform) {
                    const resources::MeshInterface *rmesh = curMesh->_mesh;

                    math::m4x4 localTransform = rmesh->getLocalTransform();
                    animator.getMatrix(rmesh->getName(), localTransform);
                    curMesh->_fullTransform = localTransform * parentFullTransform;

                    if(curMesh->_addtrfm) {
                        curMesh->_fullTransform = curMesh->_additionalTransform * curMesh->_fullTransform;
                    }

                    rmesh->tempMatrix = rmesh->getInvStartTransform() * curMesh->_fullTransform;

                    for(unsigned i = 0; i < curMesh->_childCount; i++) {
                        recursiveMeshAnimationUpdate(animator, curMesh->_childs[i], curMesh->_fullTransform);
                    }
                }
            };

            if(_modelReady) {
                if(_animator.isDirtyResources()) {
                    _animator.updateResources(resMan);
                }
                
                if(_animator.getActiveLayersCount()) {
                    fn::recursiveMeshAnimationUpdate(_animator, _root, _fullTransform);
                }
                else {
                    fn::recursiveMeshUpdate(_root, _fullTransform);
                }

                for(unsigned i = 0; i < _meshCount; i++) {
                    MeshData *cur = _meshes[i];

                    if(cur->_skinned) {
                        for(unsigned c = 0; c < cur->_mesh->getSkinnedBoneCount(); c++) {
                            cur->_skinMatrixes[c] = cur->_mesh->getSkinnedBone(c)->tempMatrix;
                        }
                    }
                }
            }
		}

        bool Model3D::isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) {
            _animator.updateResources(resMan);
            _modelReady = false;

            struct fn { 
                static void createMeshesRecursive(Model3D &mdl, MeshData *current, const resources::MeshInterface *mesh, const resources::MaterialResourceInterface *material) {
                    if(mesh->isVisible()) {
                        mdl._meshes[mdl._meshCount++] = current;
                    }
                    
                    current->_mesh = mesh;
                    current->_materialParams = material->getMeshParams(mesh->getName());

                    if(current->_materialParams == nullptr) {
                        current->_materialParams = material->getMeshParams("any");
                    }

                    current->_childCount = mesh->getChildCount();

                    if(current->_childCount) {
                        current->_childs = new MeshData *[current->_childCount];

                        for(unsigned i = 0; i < current->_childCount; i++) {
                            const resources::MeshInterface *child = mesh->getChild(i);
                            current->_childs[i] = mdl._getOrCreateMeshByName(child->getName());
                            createMeshesRecursive(mdl, current->_childs[i], child, material);
                        }
                    }                    
                }
            };
            
            if(_model == nullptr) {
                if((_model = resMan.getResource(_modelResourcePath)) == nullptr) {
                    return false;
                }
            }
            if(_material == nullptr) {
                if((_material = resMan.getResource(_materialResourcePath)) == nullptr) {
                    return false;
                }
            }
            
            if(_model->valid() && _material->valid()) {
                if(_root == nullptr) {
                    _root = _getOrCreateMeshByName(_model->getRoot()->getName());
                    _meshes = new MeshData * [_model->getTotalMeshCount()];
                    
                    fn::createMeshesRecursive(*this, _root, _model->getRoot(), _material);

                    for(unsigned i = 0; i < _model->getSkinMeshCount(); i++) {
                        const resources::MeshInterface *rmesh = _model->getSkinMesh(i);

                        if(rmesh->isVisible()) {
                            MeshData *current = _getOrCreateMeshByName(rmesh->getName());
                            _meshes[_meshCount++] = current;
                            
                            current->_skinMatrixCount = rmesh->getSkinnedBoneCount();
                            current->_skinMatrixes = new math::m4x4 [current->_skinMatrixCount];
                            current->_materialParams = _material->getMeshParams(rmesh->getName());

                            if(current->_materialParams == nullptr) {
                                current->_materialParams = _material->getMeshParams("any");
                            }

                            current->_mesh = rmesh;
                            current->_skinned = true;
                            current->_visible = true;
                        }
                    }
                }

                _modelReady = true;
                return true;
            }

            if(_root) {
                for(unsigned i = 0; i < _meshCount; i++) {
                    if(_meshes[i]->_skinned) {
                        delete _meshes[i];
                    }
                }

                delete[] _meshes;
                delete _root;

                _meshes = nullptr;
                _root = nullptr;
                _meshCount = 0;
                _meshesByName.clear();
            }
            
            return false;
		}
        
        unsigned Model3D::getComponentCount() const {
            return _meshCount;
		}

        RenderObjectInterface::ComponentInterface *Model3D::getComponentInterface(unsigned index) {
            return _meshes[index];
		}

        Model3D::MeshData *Model3D::_getOrCreateMeshByName(const fg::string &meshName) const {
            MeshData *mesh = _meshesByName.get(meshName);

            if(mesh == nullptr) {
                mesh = new MeshData ();
                _meshesByName.add(meshName, mesh);
                return mesh;
            }

            return mesh;
        }

    }
}




//