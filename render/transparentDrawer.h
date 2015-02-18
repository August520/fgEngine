
namespace fg {
    namespace render {
        enum class BillboardType {
            BILL       = 0,
            AXISBILL   = 1,
            PPVELOCITY = 2,
        };

        template <unsigned TRIANGLES_MAX> class TransparentDrawer {
        public:
            TransparentDrawer() : _ivb(nullptr) {}
            virtual ~TransparentDrawer();

            void drawMesh(platform::PlatformInterface &platform, render::RenderSupportInterface &rendering, const math::m4x4 &trfm, const resources::MeshInterface *mesh);

        protected:       
            struct SortElement {
                unsigned value;
                unsigned primStartIndex;
            };

            platform::IndexedVertexBufferInterface *_ivb;
            
            unsigned short       _sortIndexes[3 * TRIANGLES_MAX];
            SortElement          _sortArray[TRIANGLES_MAX];
                                    
        private:
            TransparentDrawer(const TransparentDrawer &);
            TransparentDrawer &operator =(const TransparentDrawer &);
        };
        
        //-------------------------------------------------------------------------

        template <unsigned TRIANGLES_MAX> TransparentDrawer <TRIANGLES_MAX>::~TransparentDrawer() {
            if(_ivb) {
                _ivb->release();
            }
        }

        template <unsigned TRIANGLES_MAX> void TransparentDrawer <TRIANGLES_MAX>::drawMesh(platform::PlatformInterface &platform, render::RenderSupportInterface &rendering, const math::m4x4 &trfm, const resources::MeshInterface *mesh) {
            if(_ivb == nullptr) {
                _ivb = platform.rdCreateIndexedVertexBuffer(platform::VertexType::NORMAL, 2 * TRIANGLES_MAX, 3 * TRIANGLES_MAX, true);
            }

            unsigned vcount = 0;
            unsigned icount = 0;
            unsigned scount = 0;

            VertexNormal *_vertexes = (VertexNormal *)_ivb->lockVertices();
            unsigned short *_indexes = (unsigned short *)_ivb->lockIndices();

            for(unsigned i = 0; i < mesh->getGeometryVertexCount(); i++) {
                const VertexSkinnedNormal &mshVertex = mesh->getGeometryVertexes()[i];
                VertexNormal &target = _vertexes[vcount++];

                *(math::p3d *)(&target.x) = math::p3d(mshVertex.x, mshVertex.y, mshVertex.z).transform(trfm, true);
                *(math::p2d *)(&target.tu) = math::p2d(mshVertex.tu, mshVertex.tv);
                *(math::p3d *)(&target.nx) = math::p3d(mshVertex.nx, mshVertex.ny, mshVertex.nz).transform(trfm, false);
                *(math::p3d *)(&target.bx) = math::p3d(mshVertex.bx, mshVertex.by, mshVertex.bz).transform(trfm, false);
                *(math::p3d *)(&target.tx) = math::p3d(mshVertex.tx, mshVertex.ty, mshVertex.tz).transform(trfm, false);
            }

            for(unsigned i = 0; i < mesh->getGeometryIndexCount(); i += 3) {
                unsigned startIndex = icount;

                unsigned index0 = _sortIndexes[icount++] = mesh->getGeometryIndexes()[i + 0];
                unsigned index1 = _sortIndexes[icount++] = mesh->getGeometryIndexes()[i + 1];
                unsigned index2 = _sortIndexes[icount++] = mesh->getGeometryIndexes()[i + 2];

                math::p3d &tp1 = *(math::p3d *)(&_vertexes[index0].x);
                math::p3d &tp2 = *(math::p3d *)(&_vertexes[index1].x);
                math::p3d &tp3 = *(math::p3d *)(&_vertexes[index2].x);

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

            _ivb->unlockVertices();
            _ivb->unlockIndices();

            rendering.defDrawConst().modelTransform.identity();
            rendering.defDrawConstApplyChanges();
            platform.rdDrawIndexedGeometry(_ivb, platform::PrimitiveTopology::TRIANGLE_LIST, icount);
        }
    }
}