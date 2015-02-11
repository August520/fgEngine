
namespace fg {
    namespace resources {
        ModelResource::Mesh::SkinContainer::SkinContainer() : skinVertexes(nullptr), skinVertexCount(0) {

        }

        ModelResource::Mesh::SkinContainer::~SkinContainer() {
            unsigned int i;

            for(i = 0; i < skinVertexCount; i++) {
                delete[] skinVertexes[i].bones;
            }
            delete[] skinVertexes;
        }

        void ModelResource::Mesh::SkinContainer::load(ModelResource *mdl, ModelResource::Mesh *target, const byteform &data, unsigned vertexCount) {
            unsigned int i, c, k;
            skinVertexes = new SkinVertex [vertexCount];
            skinVertexCount = vertexCount;

            for(i = 0; i < vertexCount; i++) {
                unsigned int boneCount = data.readDword();
                skinVertexes[i].boneCount = boneCount;
                skinVertexes[i].bones = new Bone [boneCount];

                for(c = 0; c < boneCount; c++) {
                    data.readBytes(skinVertexes[i].bones[c].boneName, 32);

                    bool pushBoneName = true;
                    for(k = 0; k < target->_skinBoneTableSize; k++) {
                        if(target->_skinBoneTable[k]->getName() == skinVertexes[i].bones[c].boneName) {
                            pushBoneName = false;
                            break;
                        }
                    }
                    if(pushBoneName) {
                        if(target->_skinBoneTableSize < SKIN_MAX_BONES) {
                            target->_skinBoneTable[target->_skinBoneTableSize] = static_cast <const Mesh *> (mdl->getMesh(skinVertexes[i].bones[c].boneName));
                            target->_skinBoneTableSize++;
                        }
                        else {
                            printf("skin bones count > FG_SKIN_MAX_BONES\n");
                            //core.log.warning("%s : skin bones count > FG_SKIN_MAX_BONES", mdl->loadPath);
                        }
                    }

                    skinVertexes[i].bones[c].weight = data.readFloat();
                }
            }
        }

        ModelResource::Mesh::Mesh(ModelResource *container) : _container(container) {
            _flags = 0;
            _vertexArray = nullptr;
            _indexArray = nullptr;
            _vertexArraySize = 0;
            _indexArraySize = 0;
            _skinBoneTableSize = 0;
            _physicIndexes = nullptr;
            _physicVertexes = nullptr;
            _physicVertexCount = 0;
            _physicIndexCount = 0;
            _meshBuffer = nullptr;
            _skinBuffer = nullptr;
        }

        ModelResource::Mesh::~Mesh() {
            for(unsigned i = 0; i < _childs.size(); i++) {
                delete _childs[i];
            }

            _childs.clear();

            if(_physicIndexes) {
                delete[] _physicIndexes;
            }
            if(_physicVertexes) {
                delete[] _physicVertexes;
            }

            if(_meshBuffer) {
                _meshBuffer->release();
            }
            if(_skinBuffer) {
                _skinBuffer->release();
            }
        }

        void ModelResource::Mesh::load(const byteform &data, math::m4x4 *parentTransform, ModelResource::Mesh *root) {
            unsigned i, c, k;

            if((_flags = data.readDword()) == 0) {
                return;
            }

            char buffer[32];
            data.readBytes(buffer, 32);
            _name = buffer;

            if(_flags & MESHTYPE_USERDATA) {
                unsigned  sz = data.readDword();
                char      *tmem = new char[sz];
                
                data.readBytes(tmem, sz);
                delete[] tmem;
            }

            _matLocal._11 = data.readFloat();
            _matLocal._12 = data.readFloat();
            _matLocal._13 = data.readFloat();
            _matLocal._14 = data.readFloat();
            _matLocal._21 = data.readFloat();
            _matLocal._22 = data.readFloat();
            _matLocal._23 = data.readFloat();
            _matLocal._24 = data.readFloat();
            _matLocal._31 = data.readFloat();
            _matLocal._32 = data.readFloat();
            _matLocal._33 = data.readFloat();
            _matLocal._34 = data.readFloat();
            _matLocal._41 = data.readFloat();
            _matLocal._42 = data.readFloat();
            _matLocal._43 = data.readFloat();
            _matLocal._44 = data.readFloat();

            _localRotate.x = data.readFloat();
            _localRotate.y = data.readFloat();
            _localRotate.z = data.readFloat();
            _localRotate.w = data.readFloat();
            _localRotate.inverse();

            _localPosition.x = data.readFloat();
            _localPosition.y = data.readFloat();
            _localPosition.z = data.readFloat();

            _matCurrent = _matLocal;
            if(parentTransform) _matCurrent = _matLocal * (*parentTransform);

            _matInverse = _matCurrent;
            _matInverse.inverse();

            if(_flags & MESHTYPE_HELPER) {
                _container->_helpersCount++;
            }
            if((_flags & (MESHTYPE_GEOMETRY | MESHTYPE_SKIN)) && (_flags & MESHTYPE_HELPER) == 0) {
                _container->_totalMeshCount++;

                struct ImportVertex {
                    float x, y, z;
                    float nx, ny, nz;
                    float tx, ty, tz;
                    float bx, by, bz;
                    float u0, v0;
                    float u1, v1;
                };

                if((_vertexArraySize = data.readDword()) > 0) {
                    ImportVertex *importVArray = new ImportVertex[_vertexArraySize];
                    data.readBytes((char *)importVArray, sizeof(ImportVertex) * _vertexArraySize);

                    if(_flags & MESHTYPE_VISIBLE) {
                        _vertexArray = new VertexSkinnedNormal [_vertexArraySize];

                        for(i = 0; i < _vertexArraySize; i++) {
                            _vertexArray[i].x = importVArray[i].x;
                            _vertexArray[i].y = importVArray[i].y;
                            _vertexArray[i].z = importVArray[i].z;

                            _vertexArray[i].tu = importVArray[i].u0;
                            _vertexArray[i].tv = importVArray[i].v0;

                            _vertexArray[i].nx = importVArray[i].nx;
                            _vertexArray[i].ny = importVArray[i].ny;
                            _vertexArray[i].nz = importVArray[i].nz;

                            _vertexArray[i].bx = importVArray[i].bx;
                            _vertexArray[i].by = importVArray[i].by;
                            _vertexArray[i].bz = importVArray[i].bz;

                            _vertexArray[i].tx = importVArray[i].tx;
                            _vertexArray[i].ty = importVArray[i].ty;
                            _vertexArray[i].tz = importVArray[i].tz;
                        }
                    }
                    else _vertexArraySize = 0;

                    delete[] importVArray;
                }

                if((_indexArraySize = data.readDword()) > 0) {
                    _indexArray = new unsigned short[_indexArraySize];
                    data.readBytes((char *)_indexArray, sizeof(unsigned short) * _indexArraySize);

                    if((_flags & MESHTYPE_VISIBLE) == 0) {
                        delete _indexArray;
                        _indexArray = nullptr;
                        _indexArraySize = 0;
                    }
                }

                _minBB.x = data.readFloat();
                _minBB.y = data.readFloat();
                _minBB.z = data.readFloat();

                _maxBB.x = data.readFloat();
                _maxBB.y = data.readFloat();
                _maxBB.z = data.readFloat();

                if(_flags & MESHTYPE_SKIN) {
                    SkinContainer *skin = new SkinContainer();
                    skin->load(_container, this, data, _vertexArraySize);

                    for(i = 0; i < skin->skinVertexCount; i++) {
                        float toVertexIndexes[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                        float toVertexWeights[4] = {1.0f, 0.0f, 0.0f, 0.0f};

                        for(c = 0; c < skin->skinVertexes[i].boneCount; c++) {
                            const char *tname = skin->skinVertexes[i].bones[c].boneName;
                            for(k = 0; k < _skinBoneTableSize; k++) {
                                if(_skinBoneTable[k]->getName() == tname) {
                                    toVertexIndexes[c] = (float)k;
                                    toVertexWeights[c] = skin->skinVertexes[i].bones[c].weight;
                                    break;
                                }
                            }
                        }

                        _vertexArray[i].bI0 = toVertexIndexes[0];
                        _vertexArray[i].bI1 = toVertexIndexes[1];
                        _vertexArray[i].bI2 = toVertexIndexes[2];
                        _vertexArray[i].bI3 = toVertexIndexes[3];

                        _vertexArray[i].bW0 = toVertexWeights[0];
                        _vertexArray[i].bW1 = toVertexWeights[1];
                        _vertexArray[i].bW2 = toVertexWeights[2];
                        _vertexArray[i].bW3 = toVertexWeights[3];
                    }

                    delete skin;
                }
            }//end if not helper

            if(_flags & (MESHTYPE_PHYSIC | MESHTYPE_PHYSIC_OLD)) {
                _container->_physicMeshCount++;

                _physicVertexCount = data.readDword();
                _physicVertexes = new math::p3d [_physicVertexCount];
                data.readBytes((char *)_physicVertexes, sizeof(math::p3d) * _physicVertexCount);

                _physicIndexCount = data.readDword();
                _physicIndexes = new unsigned short [_physicIndexCount];
                data.readBytes((char *)_physicIndexes, sizeof(unsigned short) * _physicIndexCount);
            }

            unsigned int tchildCount = data.readDword();

            for(i = 0; i < tchildCount; i++) {
                Mesh *adding = new Mesh (_container);
                adding->load(data, &_matCurrent, root);
                _childs.push_back(adding);
            }
        }

        void ModelResource::Mesh::construct(platform::PlatformInterface &api) {
            if(_vertexArraySize > 0 && _indexArraySize > 0) {
                _meshBuffer = api.rdCreateIndexedVertexBuffer(platform::VertexType::SKIN_NORMAL, _vertexArraySize, _indexArraySize, false, _vertexArray, _indexArray);
                
                delete[] _vertexArray;
                _vertexArray = nullptr;
                //_vertexArraySize = 0;

                delete[] _indexArray;
                _indexArray = nullptr;
                //_indexArraySize = 0;
            }

            if(_skinBoneTableSize > 0) {
                _skinBuffer = api.rdCreateShaderConstantBuffer(platform::ShaderConstBufferUsing::SKIN_DATA, sizeof(math::m4x4) * _skinBoneTableSize);
            }

            for(unsigned int i = 0, cnt = _childs.size(); i < cnt; i++) {
                _childs[i]->construct(api);
            }
        }

        const fg::string &ModelResource::Mesh::getName() const {
            return _name;
        }

        const math::m4x4 &ModelResource::Mesh::getLocalTransform() const {
            return _matLocal;
        }

        const math::quat &ModelResource::Mesh::getLocalRotation() const {
            return _localRotate;
        }

        const math::p3d &ModelResource::Mesh::getLocalPosition() const {
            return _localPosition;
        }

        const math::m4x4 &ModelResource::Mesh::getStartTransform() const {
            return _matCurrent;
        }

        const math::m4x4 &ModelResource::Mesh::getInvStartTransform() const {
            return _matInverse;
        }

        unsigned ModelResource::Mesh::getPhysicVertexCount() const {
            return _physicVertexCount;
        }

        unsigned ModelResource::Mesh::getPhysicIndexCount() const {
            return _physicIndexCount;
        }

        unsigned ModelResource::Mesh::getPhysicTriangleCount() const {
            return _physicIndexCount / 3;
        }

        unsigned ModelResource::Mesh::getChildCount() const {
            return _childs.size();
        }

        const MeshInterface *ModelResource::Mesh::getChild(unsigned index) const {
            return _childs[index];
        }

        const math::p3d *ModelResource::Mesh::getPhysicVertexes() const {
            return _physicVertexes;
        }

        unsigned short *ModelResource::Mesh::getPhysicIndexes() const {
            return _physicIndexes;
        }

        void ModelResource::Mesh::getPhysicTriangle(unsigned int index, math::p3d &oVertex1, math::p3d &oVertex2, math::p3d &oVertex3) const {
            if(index < _physicIndexCount / 3) {
                oVertex1 = _physicVertexes[_physicIndexes[index * 3 + 0]];
                oVertex2 = _physicVertexes[_physicIndexes[index * 3 + 1]];
                oVertex3 = _physicVertexes[_physicIndexes[index * 3 + 2]];
            }
        }

        bool ModelResource::Mesh::isPhysicPresent() const {
            return (_flags & (MESHTYPE_PHYSIC | MESHTYPE_PHYSIC_OLD)) != 0;
        }

        bool ModelResource::Mesh::isVisible() const {
            return (_flags & MESHTYPE_VISIBLE) != 0;
        }

        bool ModelResource::Mesh::isSkinned() const {
            return (_flags & MESHTYPE_SKIN) != 0;
        }

        bool ModelResource::Mesh::isHelper() const {
            return (_flags & MESHTYPE_HELPER) != 0;
        }

        const MeshInterface *ModelResource::Mesh::getSkinnedBone(unsigned index) const {
            return _skinBoneTable[index];
        }

        unsigned ModelResource::Mesh::getSkinnedBoneCount() const {
            return _skinBoneTableSize;
        }

        const math::p3d &ModelResource::Mesh::getMinBBoxPoint() const {
            return _minBB;
        }

        const math::p3d &ModelResource::Mesh::getMaxBBoxPoint() const {
            return _maxBB;
        }
        
        const platform::IndexedVertexBufferInterface *ModelResource::Mesh::getMeshBuffer() const {
            return _meshBuffer;
        }

        const platform::ShaderConstantBufferInterface *ModelResource::Mesh::getSkinConstBuffer() const {
            return _skinBuffer;
        }

        unsigned ModelResource::Mesh::getGeometryVertexCount() const {
            return _vertexArraySize;
        }

        unsigned ModelResource::Mesh::getGeometryIndexCount() const {
            return _indexArraySize;
        }

        //--- Model -----------------------------------------------------------------

        ModelResource::~ModelResource() {
            delete _root;

            for(unsigned i = 0; i < _skinMeshes.size(); i++) {
                delete _skinMeshes[i];
            }
        }

        void ModelResource::loaded(const diag::LogInterface &log) {
            byteform data (_binaryData, _binarySize);

            if(data.readDword() == 0xf0ffaaf0) {
                _root = new Mesh(this);
                _root->load(data, nullptr, _root);

                while(data.getOffset() < data.getSize()) {
                    Mesh *tmpMesh = new Mesh(this);
                    tmpMesh->load(data, nullptr, nullptr);

                    if(tmpMesh->_flags & MESHTYPE_SKIN) {
                        _skinMeshes.push_back(tmpMesh);
                    }
                    else {
                        delete tmpMesh;
                    }
                }
            }
        }

        bool ModelResource::constructed(const diag::LogInterface &log, platform::PlatformInterface &api) {
            if(_root) {
                _root->construct(api);
            }

            for(unsigned i = 0, cnt = _skinMeshes.size(); i < cnt; i++) {
                _skinMeshes[i]->construct(api);
            }
            return false;
        }

        void ModelResource::unloaded() {
            delete _root;
            _root = nullptr;

            for(unsigned i = 0; i < _skinMeshes.size(); i++) {
                delete _skinMeshes[i];
            }

            _skinMeshes.clear();
        }

        const MeshInterface *ModelResource::getMesh(const fg::string &meshName) const {
            struct inl {
                static Mesh *getMeshRecursive(Mesh *root, const fg::string &meshName) {
                    if(root->getName() == meshName) {
                        return root;
                    }
                    else {
                        for(unsigned i = 0, cnt = root->getChildCount(); i < cnt; i++) {
                            Mesh *tres = getMeshRecursive(root->_childs[i], meshName);

                            if(tres) {
                                return tres;
                            }
                        }
                        return nullptr;
                    }
                }
            };

            for(unsigned int i = 0, cnt = _skinMeshes.size(); i < cnt; i++) {
                if(_skinMeshes[i]->getName() == meshName) {
                    return _skinMeshes[i];
                }
            }

            if(_root) return inl::getMeshRecursive(_root, meshName);
            return nullptr;
        }

        const MeshInterface *ModelResource::getRoot() const {
            return _root;
        }

        const MeshInterface *ModelResource::getSkinMesh(const fg::string &meshName) const {
            for(unsigned i = 0, cnt = _skinMeshes.size(); i < cnt; i++) {
                if(_skinMeshes[i]->getName() == meshName) {
                    return _skinMeshes[i];
                }
            }
            return nullptr;
        }

        const MeshInterface *ModelResource::getSkinMesh(unsigned index) const {
            return _skinMeshes[index];
        }

        unsigned ModelResource::getSkinMeshCount() const {
            return _skinMeshes.size();
        }

        unsigned ModelResource::getHelpersCount() const {
            return _helpersCount;
        }

        unsigned ModelResource::getTotalMeshCount() const {
            return _totalMeshCount;
        }

        unsigned ModelResource::getPhysicMeshCount() const {
            return _physicMeshCount;
        }

        void ModelResource::getHelpers(std::vector <const MeshInterface *> &out) const {
            struct inl {
                static void getHelpersRecursive(Mesh *cur, std::vector <const MeshInterface *> &out) {
                    if(cur->isHelper()) {
                        out.push_back(cur);
                    }
                    for(unsigned int i = 0, cnt = cur->_childs.size(); i < cnt; i++) {
                        getHelpersRecursive(cur->_childs[i], out);
                    }
                }
            };

            if(_root) inl::getHelpersRecursive(_root, out);
        }

        void ModelResource::getPhysicMeshes(std::vector <const MeshInterface *> &out) const {
            struct inl {
                static void getPhysicMeshesRecursive(Mesh *cur, std::vector <const MeshInterface *> &out) {
                    if(cur->isPhysicPresent()) {
                        out.push_back(cur);
                    }
                    for(unsigned int i = 0, cnt = cur->_childs.size(); i < cnt; i++) {
                        getPhysicMeshesRecursive(cur->_childs[i], out);
                    }
                }
            };

            if(_root) inl::getPhysicMeshesRecursive(_root, out);
        }
    }
}

