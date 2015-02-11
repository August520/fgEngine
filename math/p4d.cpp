
p4d::p4d(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
p4d::p4d(const p2d &v) : x(v.x), y(v.y), z(0.0f), w(0.0f) {}
p4d::p4d(const p3d &v) : x(v.x), y(v.y), z(v.z), w(0.0f) {}
p4d::p4d(const p4d &v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

p4d &p4d::transform(const m4x4 &im){
    float tw = w;
    float tx = x;
    float ty = y;
    float tz = z;

    x = tx * im._11 + ty * im._21 + tz * im._31 + tw * im._41;
    y = tx * im._12 + ty * im._22 + tz * im._32 + tw * im._42;
    z = tx * im._13 + ty * im._23 + tz * im._33 + tw * im._43;
    w = tx * im._14 + ty * im._24 + tz * im._34 + tw * im._44;
    return *this;
}