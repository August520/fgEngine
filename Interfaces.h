
namespace fg {
    struct color {
        float r, g, b, a;

        color(float _r = 1.0f, float _g = 1.0f, float _b = 1.0f, float _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {}
        color(const math::p4d &v) : r(v.x), g(v.y), b(v.z), a(v.w) {}

        color operator +(const color &v) const {
            return color(r + v.r, g + v.g, b + v.b, a + v.a);
        }

        color operator -(const color &v) const {
            return color(r - v.r, g - v.g, b - v.b, a - v.a);
        }

        color operator *(float v) const {
            return color(r * v, g * v, b * v, a * v);
        }

        color operator /(float v) const {
            return color(r / v, g / v, b / v, a / v);
        }

        operator math::p4d() const {
            return math::p4d(r, g, b, a);
        }
    };

    // p3d pos
    //
    struct VertexSimple {
        math::p3d position;
    };

    // p3d pos, p2d uv
    //
    struct VertexTextured {
        math::p3d position;
        math::p2d uv;
    };

    // p3d pos, p2d uv, p3d nrm, p3d binormal, p3d tangent
    //
    struct VertexNormal {
        math::p3d position;
        math::p2d uv;
        math::p3d normal;
        math::p3d binormal;
        math::p3d tangent;
    };

    // p3d pos, p2d uv, p3d nrm, p3d binormal, p3d tangent, p4d boneIndexes, p4d boneWeights
    //
    struct VertexSkinnedNormal {
        math::p3d position;
        math::p2d uv;
        math::p3d normal;
        math::p3d binormal;
        math::p3d tangent;
        math::p4d boneIndexes;
        math::p4d boneWeights;
    };

    // m4x4 modelTransform, p4d color
    //
    struct InstanceDataDefault {
        math::m4x4 modelTransform;
        color rgba;
    };

    // p3d pos, float isGrey, p4d color // isGrey > 0.0 texture have only R component
    //
    struct InstanceDataDisplayObject {
        math::p3d position;
        float isGrey;
        color primaryColor;
    };

}




