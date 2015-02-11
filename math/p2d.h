// TODO: 

struct m3x3;

struct p2d{
    float x, y;

    explicit p2d(float _x = 0, float _y = 0);
    p2d(const p2d &v);
    p2d &operator = (const p2d &v);
    
    p2d   &toLength(float ilen);
    p2d   &toLengthSafe(float ilen);
    float length() const;
    float lengthSqr() const;
    float distTo(const p2d &point) const;
    float distToSqr(const p2d &point) const;
    float angleTo(const p2d &v) const;
    
    void  rotate(float angleRad);

    p2d operator +(const p2d &v) const;
    p2d operator -(const p2d &v) const;
    p2d operator *(float a) const;
    p2d operator /(float a) const;

    p2d &operator +=(const p2d &v);
    p2d &operator -=(const p2d &v);
    p2d &operator *=(float a);
    p2d &operator /=(float a);

    float dot(const p2d &v);
    float cross(const p2d &v);

    p2d &transform(const m3x3 &im, bool likePos = false);
};

p2d operator *(float a, const p2d &v);
p2d operator /(float a, const p2d &v);

