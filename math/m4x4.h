
struct quat;

#define M_EPSILON 0.00001f
#define M_PI 3.1415926535897932f

struct m4x4{
    union {
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };

    m4x4();
    m4x4(const m4x4 &mat);
    m4x4(float m11, float m12, float m13, float m14, 
         float m21, float m22, float m23, float m24,
         float m31, float m32, float m33, float m34, 
         float m41, float m42, float m43, float m44);
    m4x4(const quat &q);

    void inverse();

    void identity();
    void setTranslate(const p3d &pos);
    void setScaling(float x, float y, float z);
    void setRotateX(float angleRad);
    void setRotateY(float angleRad);
    void setRotateZ(float angleRad);

    void toQuaternion(quat &out) const;
    void rotationAxis(float angleRad, const p3d &axis);
    void lookAt(const p3d &eye, const p3d &at, const p3d &up);
    void perspectiveFov(float fovY, float aspect, float zNear, float zFar);
    m4x4 operator *(const m4x4 &m) const;
};

