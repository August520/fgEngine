
m4x4::m4x4() :
    _11(1.0f), _12(0.0f), _13(0.0f), _14(0.0f), 
    _21(0.0f), _22(1.0f), _23(0.0f), _24(0.0f), 
    _31(0.0f), _32(0.0f), _33(1.0f), _34(0.0f), 
    _41(0.0f), _42(0.0f), _43(0.0f), _44(1.0f){}

m4x4::m4x4(const m4x4 &mat){
    memcpy(this, &mat, sizeof(m4x4));
}

m4x4::m4x4(
    float m11, float m12, float m13, float m14, 
    float m21, float m22, float m23, float m24,
    float m31, float m32, float m33, float m34, 
    float m41, float m42, float m43, float m44) :
    _11(m11), _12(m12), _13(m13), _14(m14), 
    _21(m21), _22(m22), _23(m23), _24(m24), 
    _31(m31), _32(m32), _33(m33), _34(m34), 
    _41(m41), _42(m42), _43(m43), _44(m44) {}

m4x4::m4x4(const quat &q){
    q.toMatrix(*this);
}

void m4x4::inverse(){
    struct inl{
        inline static float det3x3(float a11, float a12, float a13, float a21, float a22, float a23, float a31, float a32, float a33){
            return a11 * (a22 * a33 - a23 * a32) - a12 * (a21 * a33 - a23 * a31) + a13 * (a21 * a32 - a22 * a31);
        }
    };

    float det, invDet;
    det  = _11 * inl::det3x3(_22, _23, _24, _32, _33, _34, _42, _43, _44);
    det -= _12 * inl::det3x3(_21, _23, _24, _31, _33, _34, _41, _43, _44);
    det += _13 * inl::det3x3(_21, _22, _24, _31, _32, _34, _41, _42, _44);
    det -= _14 * inl::det3x3(_21, _22, _23, _31, _32, _33, _41, _42, _43);

    invDet = 1.0f / det;

    float a11 = +inl::det3x3(_22, _23, _24, _32, _33, _34, _42, _43, _44) * invDet;
    float a12 = -inl::det3x3(_21, _23, _24, _31, _33, _34, _41, _43, _44) * invDet;
    float a13 = +inl::det3x3(_21, _22, _24, _31, _32, _34, _41, _42, _44) * invDet;
    float a14 = -inl::det3x3(_21, _22, _23, _31, _32, _33, _41, _42, _43) * invDet;

    float a21 = -inl::det3x3(_12, _13, _14, _32, _33, _34, _42, _43, _44) * invDet;
    float a22 = +inl::det3x3(_11, _13, _14, _31, _33, _34, _41, _43, _44) * invDet;
    float a23 = -inl::det3x3(_11, _12, _14, _31, _32, _34, _41, _42, _44) * invDet;
    float a24 = +inl::det3x3(_11, _12, _13, _31, _32, _33, _41, _42, _43) * invDet;

    float a31 = +inl::det3x3(_12, _13, _14, _22, _23, _24, _42, _43, _44) * invDet;
    float a32 = -inl::det3x3(_11, _13, _14, _21, _23, _24, _41, _43, _44) * invDet;
    float a33 = +inl::det3x3(_11, _12, _14, _21, _22, _24, _41, _42, _44) * invDet;
    float a34 = -inl::det3x3(_11, _12, _13, _21, _22, _23, _41, _42, _43) * invDet;

    float a41 = -inl::det3x3(_12, _13, _14, _22, _23, _24, _32, _33, _34) * invDet;
    float a42 = +inl::det3x3(_11, _13, _14, _21, _23, _24, _31, _33, _34) * invDet;
    float a43 = -inl::det3x3(_11, _12, _14, _21, _22, _24, _31, _32, _34) * invDet;
    float a44 = +inl::det3x3(_11, _12, _13, _21, _22, _23, _31, _32, _33) * invDet;

    _11 = a11; _12 = a21; _13 = a31; _14 = a41;
    _21 = a12; _22 = a22; _23 = a32; _24 = a42;
    _31 = a13; _32 = a23; _33 = a33; _34 = a43;
    _41 = a14; _42 = a24; _43 = a34; _44 = a44;
}

void m4x4::identity(){
    _12 = _13 = _14 = _23 = _24 = _34 = 0.0f;
    _41 = _42 = _43 = _32 = _31 = _21 = 0.0f;
    _11 = _22 = _33 = _44 = 1.0f;
}

void m4x4::setTranslate(const p3d &pos){
    _12 = _13 = _14 = _23 = _24 = _34 = 0.0f;
    _32 = _31 = _21 = 0.0f;
    _11 = _22 = _33 = _44 = 1.0f;
    _41 = pos.x;
    _42 = pos.y;
    _43 = pos.z;
}

void m4x4::setScaling(float x, float y, float z){
    _12 = _13 = _14 = _23 = _24 = _34 = 0.0f;
    _41 = _42 = _43 = _32 = _31 = _21 = 0.0f;
    _44 = 1.0f;
    _11 = x;
    _22 = y;
    _33 = z;
}

void m4x4::setRotateX(float angleRad){
    _22 = _33 = cosf(angleRad);
    _23 = sinf(angleRad);
    _32 = -_23;
    _11 = _44 = 1.0f;
    _12 = _13 = _14 = _24 = _34 = 0.0f;
    _21 = _31 = _41 = _42 = _43 = 0.0f;
}

void m4x4::setRotateY(float angleRad){
    _11 = _33 = cosf(angleRad);
    _13 = sinf(angleRad);
    _31 = -_13;
    _22 = _44 = 1.0f;
    _12 = _14 = _24 = _34 = _32 = _41 = _42 = _43 = 0.0f;
}

void m4x4::setRotateZ(float angleRad){
    _11 = _22 = cosf(angleRad);
    _12 = sinf(angleRad);
    _21 = -_12;
    _33 = _44 = 1.0f;
    _13 = _14 = _23 = _24 = _34 = 0.0f;
    _31 = _32 = _41 = _42 = _43 = 0.0f;
}

void m4x4::toQuaternion(quat &out) const{
    float	s, q[4];
    int		i, j, k;
    int		nxt[3] = {1, 2, 0};

    float	trace = m[0][0] + m[1][1] + m[2][2];

    if(trace > 0.0f){
        s = (float)sqrtf(trace + 1.0f);
        out.w = s / 2.0f;
        s = 0.5f / s;
        out.x = (m[1][2] - m[2][1]) * s;
        out.y = (m[2][0] - m[0][2]) * s;
        out.z = (m[0][1] - m[1][0]) * s;
    }
    else{
        i = 0;

        if(m[1][1] > m[0][0]) i = 1;
        if(m[2][2] > m[i][i]) i = 2;

        j = nxt[i];
        k = nxt[j];

        s = (float)sqrtf((m[i][i] - (m[j][j] + m[k][k])) + 1.0f);

        q[i] = s * 0.5f;

        if(s != 0.0f) s = 0.5f / s;

        q[3] = (m[j][k] - m[k][j]) * s;
        q[j] = (m[i][j] + m[j][i]) * s;
        q[k] = (m[i][k] + m[k][i]) * s;

        out.x = q[0];
        out.y = q[1];
        out.z = q[2];
        out.w = q[3];
    }
}

void m4x4::rotationAxis(float angleRad, const p3d &axis){
    quat().rotationAxis(angleRad, axis).toMatrix(*this);
}

void m4x4::lookAt(const p3d &eye, const p3d &at, const p3d &up){
    p3d tmpx, tmpy;
    p3d tmpz (eye.x - at.x, eye.y - at.y, eye.z - at.z);
    tmpz.normalize();
    tmpx.cross(up, tmpz);
    tmpy.cross(tmpz, tmpx);

    _11 = tmpx.x; _12 = tmpy.x; _13 = tmpz.x; _14 = 0.0f;
    _21 = tmpx.y; _22 = tmpy.y; _23 = tmpz.y; _24 = 0.0f;
    _31 = tmpx.z; _32 = tmpy.z; _33 = tmpz.z; _34 = 0.0f;
    _41 = -tmpx.dot(eye), _42 = -tmpy.dot(eye), _43 = -tmpz.dot(eye), _44 = 1.0f;
}

void m4x4::perspectiveFovRH(float fovY, float aspect, float zNear, float zFar){
    float yS = 1.0f / tanf(fovY * 0.5f); 
    float xS = yS / aspect;

    _11 = xS; _12 = _13 = _14 = 0.0f;
    _22 = yS; _21 = _23 = _24 = 0.0f;
    _31 = _32 = 0.0f; _33 = zFar / (zNear - zFar); _34 = -1.0f;
    _41 = _42 = 0.0f; _43 = (zNear * zFar) / (zNear - zFar); _44 = 0.0f; 
}

void m4x4::perspectiveFovLH(float fovY, float aspect, float zNear, float zFar) {
    float yS = 1.0f / tanf(fovY * 0.5f);
    float xS = yS / aspect;

    _11 = xS; _12 = _13 = _14 = 0.0f;
    _22 = yS; _21 = _23 = _24 = 0.0f;
    _31 = _32 = 0.0f; _33 = zFar / (zFar - zNear); _34 = 1.0f;
    _41 = _42 = 0.0f; _43 = -(zNear * zFar) / (zFar - zNear); _44 = 0.0f;
}

m4x4 m4x4::operator *(const m4x4 &m) const{
    int   i, j;
    const float *m1Ptr = (float *)this, *m2Ptr = (float *)&m;
    m4x4  dst;
    float *dstPtr = (float *)&dst;

    for(i=0; i<4; i++){
        for(j=0; j<4; j++){
            *dstPtr = m1Ptr[0] * m2Ptr[0 * 4 + j] + m1Ptr[1] * m2Ptr[1 * 4 + j] + m1Ptr[2] * m2Ptr[2 * 4 + j] + m1Ptr[3] * m2Ptr[3 * 4 + j];
            dstPtr++;
        }
        m1Ptr += 4;
    }
    return dst;
}


