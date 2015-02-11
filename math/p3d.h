
struct m4x4;
struct quat;

//+ конструкторы от остальных p*

struct p3d{
    float x, y, z;

    p3d(float _x = 0, float _y = 0, float _z = 0);
    p3d(const p2d &v);
    p3d(const p3d &v);
    p3d &operator = (const p3d &v);

    p3d   &toLength(float ilen);
    p3d   &toLengthSafe(float ilen);
    float length() const;
    float lengthSqr() const;

    p3d operator +(const p3d &v) const;
    p3d operator -(const p3d &v) const;
    p3d operator *(const p3d &v) const;
    p3d operator /(const p3d &v) const;
    p3d operator *(float a) const;
    p3d operator /(float a) const;
    p3d &operator +=(const p3d &v);
    p3d &operator -=(const p3d &v);

    p3d &transform(const m4x4 &im, bool likePos = false);
    p3d &transform(const quat &q);
    p3d &cross(const p3d &A, const p3d &B);
 
    float dot(const p3d &v) const;
    float distanceTo(const p3d &v) const;
    float distanceToSqr(const p3d &v) const;
    
    p3d &normalize();
    p3d &normalizeSafe();
    p3d directionTo(const p3d &v, float len = 1.0f) const;
    
    float angleTo(const p3d &v) const;
};

p3d operator *(float a, const p3d &v);
p3d operator /(float a, const p3d &v);

