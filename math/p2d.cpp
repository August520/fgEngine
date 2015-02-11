
p2d::p2d(float _x, float _y) : x(_x), y(_y) {}
p2d::p2d(const p2d &v) : x(v.x), y(v.y) {}

p2d &p2d::operator = (const p2d &v){
    x = v.x;
    y = v.y;
    return *this;
}

p2d &p2d::toLength(float ilen){
    float len = sqrt(x * x + y * y);
    x *= (ilen / len);
    y *= (ilen / len);
    return *this;
}

p2d &p2d::toLengthSafe(float ilen){
    float len = sqrt(x * x + y * y);
    if(fabs(len) > 0.0001f){
        x *= (ilen / len);
        y *= (ilen / len);
    }
    return *this;
}

float p2d::length() const{
    return sqrtf(x * x + y * y);
}

float p2d::lengthSqr() const{
    return x * x + y * y;
}

float p2d::distTo(const p2d &point) const{
    return (point - *this).length();
}

float p2d::distToSqr(const p2d &point) const{
    return (point - *this).lengthSqr();
}

float p2d::angleTo(const p2d &v) const{
    return acosf((x * v.x + y * v.y) / sqrtf((x * x + y * y) * (v.x * v.x + v.y * v.y)));
}

void p2d::rotate(float angleRad){
    float tx = x * cosf(angleRad) - y * sinf(angleRad);
    float ty = x * sinf(angleRad) + y * cosf(angleRad);
    x = tx;
    y = ty;
}

p2d p2d::operator +(const p2d &v) const{
    return p2d(x + v.x, y + v.y);
}

p2d p2d::operator -(const p2d &v) const{
    return p2d(x - v.x, y - v.y);
}

p2d p2d::operator *(float a) const{
    return p2d(x * a, y * a);
}

p2d p2d::operator /(float a) const{
    return p2d(x / a, y / a);
}

p2d &p2d::operator +=(const p2d &v){
    x += v.x;
    y += v.y;
    return *this;
}

p2d &p2d::operator -=(const p2d &v){
    x -= v.x;
    y -= v.y;
    return *this;
}

p2d &p2d::operator *=(float a){
    x *= a;
    y *= a;
    return *this;
}

p2d &p2d::operator /=(float a){
    x /= a;
    y /= a;
    return *this;
}

float p2d::dot(const p2d &v){
    return x * v.x + y * v.y;
}

float p2d::cross(const p2d &v){
    return x * v.y - y * v.x;
}

p2d &p2d::transform(const m3x3 &im, bool likePos){
    float w = likePos ? 1.0f : 0.0f;
    float tx = x;
    float ty = y;

    x = tx * im._11 + ty * im._21 + w * im._31;
    y = tx * im._12 + ty * im._22 + w * im._32;
    return *this;
}

p2d operator *(float a, const p2d &v){
    return p2d(v.x * a, v.y * a);
}

p2d operator /(float a, const p2d &v){
    return p2d(v.x / a, v.y / a);
}

