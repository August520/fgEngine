
m3x3::m3x3() :
    _11(1.0f), _12(0.0f), _13(0.0f), 
    _21(0.0f), _22(1.0f), _23(0.0f), 
    _31(0.0f), _32(0.0f), _33(1.0f) {}

m3x3::m3x3(const m3x3 &mat){
    memcpy(this, &mat, sizeof(m3x3));
}

m3x3::m3x3(
    float m11, float m12, float m13, 
    float m21, float m22, float m23,
    float m31, float m32, float m33) :
    _11(m11), _12(m12), _13(m13), 
    _21(m21), _22(m22), _23(m23), 
    _31(m31), _32(m32), _33(m33) {}

void m3x3::identity(){
    _12 = _13 = _23 = 0.0f;
    _32 = _31 = _21 = 0.0f;
    _11 = _22 = _33 = 1.0f;
}

void m3x3::setTranslate(const p2d &pos){
    _12 = _13 = _23 = _21 = 0.0f;
    _11 = _22 = _33 = 1.0f;
    _31 = pos.x;
    _32 = pos.y;
}

void m3x3::setScaling(float x, float y){
    _12 = _13 = _23 = 0.0f;
    _32 = _31 = _21 = 0.0f;
    _11 = x;
    _22 = y;
    _33 = 1.0f;
}

void m3x3::setRotate(float angleRad){
    _11 = _22 = cosf(angleRad);
    _12 = sinf(angleRad);
    _21 = -_12;
    _33 = 1.0f;
    _13 = _23 = 0.0f;
    _31 = _32 = 0.0f;
}

/*
        | A B C |
    M = | D E F |
        | G H I |

    “о ее опеделитель считаетс€ так:
    det M = A * (EI - HF) - B * (DI - GF) + C * (DH - GE)
    ≈сли определитель не равен 0, то обратна€ считаетс€ так:
     -1     1     |   EI-FH  -(BI-HC)   BF-EC  |
    M   = ----- . | -(DI-FG)   AI-GC  -(AF-DC) |
          det M   |   DH-GE  -(AH-GB)   AE-BD  |
*/

void m3x3::inverse(const m3x3 &src){
    float det = 1.0f / (src._11 * (src._22 * src._33 - src._32 * src._23) - src._12 * (src._21 * src._33 - src._31 * src._23) + src._13 * (src._21 * src._32 - src._31 * src._22));

    _11 = (src._22 * src._33 - src._32 * src._23) * det;
    _12 = -(src._12 * src._33 - src._32 * src._13) * det;
    _13 = (src._12 * src._23 - src._22 * src._13) * det;
    _21 = -(src._21 * src._33 - src._31 * src._23) * det;
    _22 = (src._11 * src._33 - src._31 * src._13) * det;
    _23 = -(src._11 * src._23 - src._21 * src._13) * det;
    _31 = (src._21 * src._32 - src._31 * src._22) * det;
    _32 = -(src._11 * src._32 - src._31 * src._12) * det;
    _33 = (src._11 * src._22 - src._12 * src._21) * det;
}

m3x3 m3x3::operator *(const m3x3 &m) const{
    int   i, j;
    const float *m1Ptr = (float *)this, *m2Ptr = (float *)&m;
    m3x3  dst;
    float *dstPtr = (float *)&dst;

    for(i=0; i<3; i++){
        for(j=0; j<3; j++){
            *dstPtr = m1Ptr[0] * m2Ptr[0 * 3 + j] + m1Ptr[1] * m2Ptr[1 * 3 + j] + m1Ptr[2] * m2Ptr[2 * 3 + j];
            dstPtr++;
        }
        m1Ptr += 3;
    }
    return dst;
}