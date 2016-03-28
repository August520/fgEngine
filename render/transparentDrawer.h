
// TODO: total remake, now it doesn't work for multiple meshes/particle sources
//
namespace fg {
    namespace render {
        template <unsigned PRIMITIVE_MAX> class TransparentDrawer {
        public:
            TransparentDrawer() {
                _indexedVertexBuffer = nullptr;
                _instanceBuffer = nullptr;
                _depthParams = nullptr;
                _lastIndexCount = 0;
                _lastVertexCount = 0;
                _lastInstanceCount = 0;
                _sortArray = nullptr;
                _sortData = nullptr;
                _sortIndexes = nullptr;
            }

            virtual ~TransparentDrawer();

            void drawParticles(render::RenderAPI &api, const object3d::Particles3DInterface::EmitterComponentInterface *emitterComponent);
            void drawMesh(render::RenderAPI &api, const math::m4x4 &trfm, const resources::MeshInterface *mesh);

        protected:
            struct SortElement {
                unsigned  value;
                unsigned  primStartIndex;
            };

            platform::IndexedVertexBufferInterface *_indexedVertexBuffer;
            platform::InstanceDataInterface        *_instanceBuffer;
            platform::DepthParamsInterface         *_depthParams;

            unsigned  _lastVertexCount;
            unsigned  _lastIndexCount;
            unsigned  _lastInstanceCount;
            
            InstanceDataDefault  *_sortData;
            SortElement          *_sortArray;
            unsigned short       *_sortIndexes;

            void _prepareDepthParams(render::RenderAPI &api);
            void _prepareGeometryBuffers(render::RenderAPI &api, unsigned vertexCount, unsigned indexCount);
            void _prepareInstanceBuffers(render::RenderAPI &api, unsigned instanceCount);

        private:
            TransparentDrawer(const TransparentDrawer &);
            TransparentDrawer &operator =(const TransparentDrawer &);
        };
        
        //-------------------------------------------------------------------------

        template <unsigned PRIMITIVE_MAX> TransparentDrawer <PRIMITIVE_MAX>::~TransparentDrawer() {
            if(_indexedVertexBuffer) {
                _indexedVertexBuffer->release();
            }
            if(_instanceBuffer) {
                _instanceBuffer->release();
            }
            if(_depthParams) {
                _depthParams->release();
            }
            if(_sortData) {
                delete []_sortData;
            }
            if(_sortArray) {
                delete []_sortArray;
            }
            if(_sortIndexes) {
                delete []_sortIndexes;
            }
        }

        template <unsigned PRIMITIVE_MAX> void TransparentDrawer <PRIMITIVE_MAX> ::_prepareDepthParams(render::RenderAPI &api) {
            if(_depthParams == nullptr) {
                _depthParams = api.platform.rdCreateDepthParams(true, platform::DepthFunc::LESS_EQUAL, false);                
            }

            api.platform.rdSetDepthParams(_depthParams);
        }

        template <unsigned PRIMITIVE_MAX> void TransparentDrawer <PRIMITIVE_MAX> ::_prepareGeometryBuffers(render::RenderAPI &api, unsigned vertexCount, unsigned indexCount) {
            if(_lastVertexCount < vertexCount || _lastIndexCount < indexCount) {
                if(_indexedVertexBuffer) {
                    _indexedVertexBuffer->release();
                }

                _indexedVertexBuffer = api.platform.rdCreateIndexedVertexBuffer(platform::VertexType::NORMAL, vertexCount, indexCount, true);
                _lastIndexCount = indexCount;
                _lastVertexCount = vertexCount;
            }
        }

        template <unsigned PRIMITIVE_MAX> void TransparentDrawer <PRIMITIVE_MAX> ::_prepareInstanceBuffers(render::RenderAPI &api, unsigned instanceCount) {
            if(_lastInstanceCount < instanceCount) {
                delete [] _sortArray;
                delete [] _sortData;
                delete [] _sortIndexes;

                _lastInstanceCount = instanceCount;
                _sortArray = new SortElement [instanceCount];
                _sortData = new InstanceDataDefault [instanceCount];
                _sortIndexes = new unsigned short [3 * instanceCount];

                if(_instanceBuffer) {
                    _instanceBuffer->release();
                }

                _instanceBuffer = api.platform.rdCreateInstanceData(platform::InstanceDataType::DEFAULT, _lastInstanceCount);
            }
        }

        template <unsigned PRIMITIVE_MAX> void TransparentDrawer <PRIMITIVE_MAX> ::drawParticles(render::RenderAPI &api, const object3d::Particles3DInterface::EmitterComponentInterface *emitterComponent) {
            const particles::EmitterInterface *emitter = emitterComponent->getEmitter();
            particles::ParticleType type = emitter->getType();

            _prepareDepthParams(api);
            _prepareGeometryBuffers(api, 4, 6);
                        
            CameraInterface &cam = api.rendering.getCamera();
            VertexNormal *_vertexes = (VertexNormal *)_indexedVertexBuffer->lockVertices();
            unsigned short *_indexes = (unsigned short *)_indexedVertexBuffer->lockIndices();

            if(type == particles::ParticleType::PPVELOCITY) {
                _vertexes[0].position = math::p3d(-0.5f, 0.0f, -0.5f);
                _vertexes[1].position = math::p3d(-0.5f, 0.0f, 0.5f);
                _vertexes[2].position = math::p3d(0.5f, 0.0f, 0.5f);
                _vertexes[3].position = math::p3d(0.5f, 0.0f, -0.5f);
            }
            else if(type == particles::ParticleType::BILL) {
                _vertexes[0].position = math::p3d(-0.5f, -0.5f, 0.0f);
                _vertexes[1].position = math::p3d(-0.5f, 0.5f, 0.0f);
                _vertexes[2].position = math::p3d(0.5f, 0.5f, 0.0f);
                _vertexes[3].position = math::p3d(0.5f, -0.5f, 0.0f);
            }
            else if(type == particles::ParticleType::AXISBILL) {
                _vertexes[0].position = math::p3d(-0.5f, 0.0f, 0.0f);
                _vertexes[1].position = math::p3d(-0.5f, 1.0f, 0.0f);
                _vertexes[2].position = math::p3d(0.5f, 1.0f, 0.0f);
                _vertexes[3].position = math::p3d(0.5f, 0.0f, 0.0f);
            }
            
            _vertexes[0].uv = math::p2d(1, 0);
            _vertexes[1].uv = math::p2d(1, 1);
            _vertexes[2].uv = math::p2d(0, 1);
            _vertexes[3].uv = math::p2d(0, 0);

            _indexes[0] = 0;
            _indexes[1] = 1;
            _indexes[2] = 2;
            _indexes[3] = 0;
            _indexes[4] = 2;
            _indexes[5] = 3;

            _indexedVertexBuffer->unlockVertices();
            _indexedVertexBuffer->unlockIndices();

            _prepareInstanceBuffers(api, emitter->getMaxParticleCount());
            
            unsigned    instanceCount = 0;
            float       particleLifeTime = 0.0f;
            math::m4x4  localTransform;

            if(type == particles::ParticleType::PPVELOCITY) {
                while(emitter->getNextParticleData(localTransform, _sortData[instanceCount].rgba, particleLifeTime)) {
                    _sortData[instanceCount].modelTransform = localTransform * (emitter->isWorldSpace() ? emitterComponent->getTransformHistory(particleLifeTime) : emitterComponent->getFullTransform());

                    InstanceDataDefault &cur = _sortData[instanceCount];
                    math::p3d dirToParticle = *(math::p3d *)(&cur.modelTransform._41) - api.rendering.getCamera().getPosition();
                    float distToParticle = dirToParticle.length();

                    dirToParticle = dirToParticle / distToParticle;

                    double range = api.rendering.getCamera().getZFar();
                    double v = double(distToParticle * dirToParticle.dot(api.rendering.getCamera().getForwardDir()));
                    
                    _sortArray[instanceCount].primStartIndex = instanceCount;
                    _sortArray[instanceCount].value = unsigned(v / (range * range) * 4294967295.0); //!!!

                    instanceCount++;
                }
            }
            else if(type == particles::ParticleType::BILL) {                
                while(emitter->getNextParticleData(localTransform, _sortData[instanceCount].rgba, particleLifeTime)) {
                    _sortData[instanceCount].modelTransform = localTransform * (emitter->isWorldSpace() ? emitterComponent->getTransformHistory(particleLifeTime) : emitterComponent->getFullTransform());

                    InstanceDataDefault &cur = _sortData[instanceCount];
                    math::p3d dirToParticle = *(math::p3d *)(&cur.modelTransform._41) - api.rendering.getCamera().getPosition();
                    float distToParticle = dirToParticle.length();

                    dirToParticle = dirToParticle / distToParticle;

                    double range = api.rendering.getCamera().getZFar();
                    double v = double(distToParticle * dirToParticle.dot(api.rendering.getCamera().getForwardDir()));
                    float  particleSize = ((math::p3d *)(&cur.modelTransform._11))->length();

                    *(math::p3d *)(&cur.modelTransform._11) = cam.getRightDir() * particleSize;
                    *(math::p3d *)(&cur.modelTransform._21) = cam.getUpDir() * particleSize;
                    *(math::p3d *)(&cur.modelTransform._31) = cam.getForwardDir() * particleSize;

                    _sortArray[instanceCount].primStartIndex = instanceCount;
                    _sortArray[instanceCount].value = unsigned(v / (range * range) * 4294967295.0); //!!!

                    instanceCount++;
                }
            }
            else if(type == particles::ParticleType::AXISBILL) {
                while(emitter->getNextParticleData(localTransform, _sortData[instanceCount].rgba, particleLifeTime)) {
                    _sortData[instanceCount].modelTransform = localTransform * (emitter->isWorldSpace() ? emitterComponent->getTransformHistory(particleLifeTime) : emitterComponent->getFullTransform());

                    InstanceDataDefault &cur = _sortData[instanceCount];
                    float distToParticleSqr = api.rendering.getCamera().getPosition().distanceToSqr(*(math::p3d *)(&cur.modelTransform._41));

                    double range = api.rendering.getCamera().getZFar();
                    float  particleSize = ((math::p3d *)(&cur.modelTransform._11))->length(); 
                    float  particleStretch = ((math::p3d *)(&cur.modelTransform._21))->length();

                    math::p3d vUp(localTransform._21 / particleStretch, localTransform._22 / particleStretch, localTransform._23 / particleStretch);
                    math::p3d vRight;
                    math::p3d vForward;
                    vRight.cross(cam.getForwardDir(), vUp);
                    vForward.cross(vUp, vRight);

                    *(math::p3d *)(&cur.modelTransform._11) = vRight * particleSize;
                    *(math::p3d *)(&cur.modelTransform._21) = vUp * particleStretch;
                    *(math::p3d *)(&cur.modelTransform._31) = vForward * particleSize;

                    _sortArray[instanceCount].primStartIndex = instanceCount;
                    _sortArray[instanceCount].value = unsigned(distToParticleSqr / (range * range) * 4294967295.0); //!!!

                    instanceCount++;
                }
            }

            qsort(_sortArray, instanceCount, sizeof(SortElement), [](const void *v1, const void *v2) {
                if(*(unsigned *)v1 <= *(unsigned *)v2) {
                    return 1;
                }

                return -1;
            });           

            InstanceDataDefault *_instances = (InstanceDataDefault *)_instanceBuffer->lock();

            for(unsigned i = 0; i < instanceCount; i++) {
                _instances[i] = _sortData[_sortArray[i].primStartIndex];
            }
            
            _instanceBuffer->unlock();
            api.platform.rdDrawIndexedGeometry(_indexedVertexBuffer, _instanceBuffer, platform::PrimitiveTopology::TRIANGLE_LIST, 6, instanceCount);            
            api.platform.rdSetDepthParams(api.rendering.getDefaultDepthParams());
        }

        // TODO: optimize matrix mul by using inverse cam matrix
        //
        template <unsigned TRIANGLES_MAX> void TransparentDrawer <TRIANGLES_MAX> ::drawMesh(render::RenderAPI &api, const math::m4x4 &trfm, const resources::MeshInterface *mesh) {
            _prepareDepthParams(api);
            _prepareGeometryBuffers(api, mesh->getGeometryVertexCount(), mesh->getGeometryIndexCount());
            _prepareInstanceBuffers(api, mesh->getGeometryIndexCount() / 3);
            
            unsigned vcount = 0;
            unsigned icount = 0;
            unsigned scount = 0;

            VertexNormal *_vertexes = (VertexNormal *)_indexedVertexBuffer->lockVertices();
            unsigned short *_indexes = (unsigned short *)_indexedVertexBuffer->lockIndices();

            for(unsigned i = 0; i < mesh->getGeometryVertexCount(); i++) {
                const VertexSkinnedNormal &mshVertex = mesh->getGeometryVertexes()[i];
                VertexNormal &target = _vertexes[vcount++];

                target.position = math::p3d(mshVertex.position).transform(trfm, true);
                target.uv = math::p2d(mshVertex.uv);
                target.normal = math::p3d(mshVertex.normal).transform(trfm, false);
                target.binormal = math::p3d(mshVertex.binormal).transform(trfm, false);
                target.tangent = math::p3d(mshVertex.tangent).transform(trfm, false);
            }

            for(unsigned i = 0; i < mesh->getGeometryIndexCount(); i += 3) {
                unsigned startIndex = icount;

                unsigned index0 = _sortIndexes[icount++] = mesh->getGeometryIndexes()[i + 0];
                unsigned index1 = _sortIndexes[icount++] = mesh->getGeometryIndexes()[i + 1];
                unsigned index2 = _sortIndexes[icount++] = mesh->getGeometryIndexes()[i + 2];

                math::p3d &tp1 = _vertexes[index0].position;
                math::p3d &tp2 = _vertexes[index1].position;
                math::p3d &tp3 = _vertexes[index2].position;

                double v = ((tp1 + tp2 + tp3) / 3.0f).distanceToSqr(api.rendering.getCamera().getPosition());
                double range = api.rendering.getCamera().getZFar();

                _sortArray[scount].value = unsigned(v / (range * range) * 4294967295.0);
                _sortArray[scount].primStartIndex = startIndex;
                scount++;
            }

            qsort(_sortArray, scount, sizeof(SortElement), [](const void *v1, const void *v2) {
                if(*(unsigned *)v1 <= *(unsigned *)v2) {
                    return 1;
                }

                return -1;
            });

            for(unsigned i = 0, off = 0; i < scount; i++) {
                SortElement &cur = _sortArray[i];

                _indexes[off++] = _sortIndexes[cur.primStartIndex + 0];
                _indexes[off++] = _sortIndexes[cur.primStartIndex + 1];
                _indexes[off++] = _sortIndexes[cur.primStartIndex + 2];
            }

            _indexedVertexBuffer->unlockVertices();
            _indexedVertexBuffer->unlockIndices();

            api.rendering.defInstanceData().modelTransform.identity();
            api.rendering.defInstanceDataApplyChanges();
            api.platform.rdDrawIndexedGeometry(_indexedVertexBuffer, api.rendering.getDefaultInstanceData(), platform::PrimitiveTopology::TRIANGLE_LIST, icount);
            api.platform.rdSetDepthParams(api.rendering.getDefaultDepthParams());
        }
    }
}