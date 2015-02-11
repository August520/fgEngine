
struct m3x3{
    union {
        struct {
            float _11, _12, _13;
            float _21, _22, _23;
            float _31, _32, _33;
        };
        float m[3][3];
    };

    m3x3();
    m3x3(const m3x3 &mat);
    m3x3(float m11, float m12, float m13, 
         float m21, float m22, float m23,
         float m31, float m32, float m33); 

    void identity();
    void setTranslate(const p2d &pos);
    void setScaling(float x, float y);
    void setRotate(float angleRad);

    void inverse(const m3x3 &src);
    m3x3 operator *(const m3x3 &m) const;
};