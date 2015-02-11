
#include <string>

namespace fg {
    // p3d pos
    //
    struct VertexSimple {
        float x, y, z;
    };

    // p3d pos, p2d uv
    //
    struct VertexTextured {
        float x, y, z;
        float tu, tv;
    };

    // p3d pos, p2d uv, p3d nrm, p3d binormal, p3d tangent
    //
    struct VertexNormal {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
        float bx, by, bz;
        float tx, ty, tz;
    };

    // p3d pos, p2d uv, p4d boneIndexes, p4d boneWeights
    //
    struct VertexSkinnedTextured {
        float x, y, z;
        float tu, tv;
        float bI0, bI1, bI2, bI3;
        float bW0, bW1, bW2, bW3;
    };

    // p3d pos, p2d uv, p3d nrm, p3d binormal, p3d tangent, p4d boneIndexes, p4d boneWeights
    //
    struct VertexSkinnedNormal {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
        float bx, by, bz;
        float tx, ty, tz;
        float bI0, bI1, bI2, bI3;
        float bW0, bW1, bW2, bW3;
    };

}




