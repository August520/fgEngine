
p3d::p3d(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
p3d::p3d(const p2d &v) : x(v.x), y(v.y), z(0.0f) {}
p3d::p3d(const p3d &v) : x(v.x), y(v.y), z(v.z) {}

p3d &p3d::operator = (const p3d &v){
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

p3d &p3d::toLength(float ilen){
    float len = sqrtf(x * x + y * y + z * z);
    x *= (ilen / len);
    y *= (ilen / len);
    z *= (ilen / len);
    return *this;
}

p3d &p3d::toLengthSafe(float ilen){
    float len = sqrtf(x * x + y * y + z * z);
    if(len > 0.0001f){
        x *= (ilen / len);
        y *= (ilen / len);
        z *= (ilen / len);
    }
    return *this;
}

float p3d::length() const{
    return sqrtf(x * x + y * y + z * z);
}

float p3d::lengthSqr() const{
    return x * x + y * y + z * z;
}

p3d p3d::operator +(const p3d &v) const{
    return p3d(x + v.x, y + v.y, z + v.z);
}

p3d p3d::operator -(const p3d &v) const{
    return p3d(x - v.x, y - v.y, z - v.z);
}

p3d p3d::operator *(const p3d &v) const{
    return p3d(x * v.x, y * v.y, z * v.z);
}

p3d p3d::operator /(const p3d &v) const{
    return p3d(x / v.x, y / v.y, z / v.z);
}

p3d p3d::operator *(float a) const{
    return p3d(x * a, y * a, z * a);
}

p3d p3d::operator /(float a) const{
    return p3d(x / a, y / a, z / a);
}

p3d &p3d::operator +=(const p3d &v){
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

p3d &p3d::operator -=(const p3d &v){
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

p3d &p3d::transform(const m4x4 &im, bool likePos){
  float w = likePos ? 1.0f : 0.0f;
  float tx = x;
  float ty = y;
  float tz = z;

  x = tx * im._11 + ty * im._21 + tz * im._31 + w * im._41;
  y = tx * im._12 + ty * im._22 + tz * im._32 + w * im._42;
  z = tx * im._13 + ty * im._23 + tz * im._33 + w * im._43;
  return *this;
}

p3d &p3d::transform(const quat &q){
  quat p(x, y, z, 0.0f);
  quat invq (-q.x, -q.y, -q.z, q.w);

  p = invq * p * q;

  x = p.x;
  y = p.y;
  z = p.z;
  return *this;
}

p3d &p3d::cross(const p3d &A, const p3d &B){
    x = A.y * B.z - A.z * B.y;
    y = A.z * B.x - A.x * B.z;
    z = A.x * B.y - A.y * B.x;    
    toLength(1.0f);
    return *this;
}

float p3d::dot(const p3d &v) const{
    return x * v.x + y * v.y + z * v.z;
}

float p3d::distanceTo(const p3d &v) const{
    return sqrtf((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z));
}
float p3d::distanceToSqr(const p3d &v) const{
    return (x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z);
}

p3d &p3d::normalize(){
    float len = length();
    x /= len;
    y /= len;
    z /= len;
    return *this;
}

p3d &p3d::normalizeSafe(){
    float len = length();
    if(len > 0.00001f){
        x /= len;
        y /= len;
        z /= len;
    }
    else{
        x = y = z = 0.0f;
    }
    return *this;
}

p3d p3d::directionTo(const p3d &v, float len) const{
    return (v - *this).normalizeSafe() * len;
}

float p3d::angleTo(const p3d &v) const{
    return acosf((x * v.x + y * v.y + z * v.z) / sqrtf((x * x + y * y + z * z) * (v.x * v.x + v.y * v.y + v.z * v.z)));        
}

p3d operator *(float a, const p3d &v){
    return p3d(v.x * a, v.y * a, v.z * a);
}

p3d operator /(float a, const p3d &v){
    return p3d(v.x / a, v.y / a, v.z / a);
}
