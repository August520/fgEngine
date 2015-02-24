
namespace fg {
    namespace render {
        template <unsigned PRIMITIVE_MAX> class TransparentDrawer {
        public:
            TransparentDrawer() {
                _indexedVertexBuffer = nullptr;
                _instanceBuffer = nullptr;
                _lastIndexCount = 0;
                _lastVertexCount = 0;
                _lastInstanceCount = 0;
                _sortArray = nullptr;
                _sortData = nullptr;
                _sortIndexes = nullptr;
            }

            virtual ~TransparentDrawer();

            void drawParticles(platform::PlatformInterface &platform, render::RenderSupportInterface &rendering, const math::m4x4 &trfm, const particles::ParticleEmitterInterface *emitter, particles::ParticleType type);
            void drawMesh(platform::PlatformInterface &platform, render::RenderSupportInterface &rendering, const math::m4x4 &trfm, const resources::MeshInterface *mesh);

        protected:
            struct SortElement {
                unsigned  value;
                unsigned  primStartIndex;
            };

            platform::IndexedVertexBufferInterface *_indexedVertexBuffer;
            platform::InstanceDataInterface        *_instanceBuffer;
            
            unsigned  _lastVertexCount;
            unsigned  _lastIndexCount;
            unsigned  _lastInstanceCount;
            
            InstanceDataDefault  *_sortData;
            SortElement          *_sortArray;
            unsigned short       *_sortIndexes;

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

        template <unsigned PRIMITIVE_MAX> void TransparentDrawer <PRIMITIVE_MAX> ::drawParticles(platform::PlatformInterface &platform, render::RenderSupportInterface &rendering, const math::m4x4 &trfm, const particles::ParticleEmitterInterface *emitter, particles::ParticleType type) {
            if(_lastVertexCount < 4 || _lastIndexCount < 6) {
                if(_indexedVertexBuffer) {
                    _indexedVertexBuffer->release();
                }
                
                _indexedVertexBuffer = platform.rdCreateIndexedVertexBuffer(platform::VertexType::NORMAL, 4, 6, true);
                _lastIndexCount = 6;
                _lastVertexCount = 4;
            }
            
            CameraInterface &cam = rendering.getCamera();
            VertexNormal *_vertexes = (VertexNormal *)_indexedVertexBuffer->lockVertices();
            unsigned short *_indexes = (unsigned short *)_indexedVertexBuffer->lockIndices();

            _vertexes[0].position = math::p3d(-0.5f, -0.5f, 0.0f);
            _vertexes[1].position = math::p3d(-0.5f, 0.5f, 0.0f);
            _vertexes[2].position = math::p3d(0.5f, 0.5f, 0.0f);
            _vertexes[3].position = math::p3d(0.5f, -0.5f, 0.0f);

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
            
            if(_lastInstanceCount < emitter->getMaxParticles()) {
                delete []_sortArray;
                delete []_sortData;
                delete []_sortIndexes;

                _lastInstanceCount = emitter->getMaxParticles();

                _sortArray = new SortElement[_lastInstanceCount];
                _sortData = new InstanceDataDefault[_lastInstanceCount];
                _sortIndexes = new unsigned short[3 * _lastInstanceCount];

                if(_instanceBuffer) {
                    _instanceBuffer->release();
                }

                _instanceBuffer = platform.rdCreateInstanceData(platform::InstanceDataType::DEFAULT, _lastInstanceCount);
            }

            unsigned instanceCount = 0;

            if(type == particles::ParticleType::BILL) {
                math::m4x4 localTransform;

                while(emitter->getNextParticleData(localTransform, _sortData[instanceCount].rgba)) {
                    _sortData[instanceCount].modelTransform = localTransform * trfm;
                    
                    InstanceDataDefault &cur = _sortData[instanceCount];
                    math::p3d dirToParticle = *(math::p3d *)(&cur.modelTransform._41) - rendering.getCamera().getPosition();
                    float distToParticle = dirToParticle.length();

                    dirToParticle = dirToParticle / distToParticle;

                    double range = rendering.getCamera().getZFar();
                    double v = double(distToParticle * dirToParticle.dot(rendering.getCamera().getForwardDir()));
                    float  particleSize = cur.modelTransform._11;

                    *(math::p3d *)(&cur.modelTransform._11) = cam.getRightDir() * particleSize;
                    *(math::p3d *)(&cur.modelTransform._21) = cam.getUpDir() * particleSize;
                    *(math::p3d *)(&cur.modelTransform._31) = cam.getForwardDir() * particleSize;
                                
                    _sortArray[instanceCount].primStartIndex = instanceCount;
                    _sortArray[instanceCount].value = unsigned(v / (range * range) * 4294967295.0); //!!!

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
            platform.rdDrawIndexedGeometry(_indexedVertexBuffer, _instanceBuffer, platform::PrimitiveTopology::TRIANGLE_LIST, 6, instanceCount);
        }

        template <unsigned TRIANGLES_MAX> void TransparentDrawer <TRIANGLES_MAX> ::drawMesh(platform::PlatformInterface &platform, render::RenderSupportInterface &rendering, const math::m4x4 &trfm, const resources::MeshInterface *mesh) {
            if(_lastIndexCount < mesh->getGeometryIndexCount() || _lastVertexCount < mesh->getGeometryVertexCount()) {
                _lastIndexCount = mesh->getGeometryIndexCount();
                _lastVertexCount = mesh->getGeometryVertexCount();
                
                if(_indexedVertexBuffer) {
                    _indexedVertexBuffer->release();
                }

                _indexedVertexBuffer = platform.rdCreateIndexedVertexBuffer(platform::VertexType::NORMAL, _lastVertexCount, _lastIndexCount, true);
            }

            if(_lastInstanceCount < mesh->getGeometryIndexCount() / 3) {
                delete []_sortArray;
                delete []_sortData;
                delete []_sortIndexes;

                _lastInstanceCount = mesh->getGeometryIndexCount() / 3;
                _sortArray = new SortElement[_lastInstanceCount];
                _sortData = new InstanceDataDefault [_lastInstanceCount];
                _sortIndexes = new unsigned short[mesh->getGeometryIndexCount()];

                if(_instanceBuffer) {
                    _instanceBuffer->release();
                }

                _instanceBuffer = platform.rdCreateInstanceData(platform::InstanceDataType::DEFAULT, _lastInstanceCount);
            }

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

                double v = ((tp1 + tp2 + tp3) / 3.0f).distanceToSqr(rendering.getCamera().getPosition());
                double range = rendering.getCamera().getZFar();

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

            rendering.defInstanceData().modelTransform.identity();
            rendering.defInstanceDataApplyChanges();
            platform.rdDrawIndexedGeometry(_indexedVertexBuffer, rendering.getDefaultInstanceData(), platform::PrimitiveTopology::TRIANGLE_LIST, icount);
        }
    }
}