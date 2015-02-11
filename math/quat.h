
struct m4x4;

struct quat{
  float x, y, z, w;

  quat(float _x = 0, float _y = 0, float _z = 0, float _w = 1): x(_x), y(_y), z(_z), w(_w) {}
  quat(const quat &q){
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
  }
  quat(const m4x4 &mat);

  void operator = (const quat &q){
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
  }

  void identity(){
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    w = 1.0f;
  }

  void inverse(){
    x = -x;
    y = -y;
    z = -z;
  }

  void negative(){
    x = -x;
    y = -y;
    z = -z;
    w = -w;
  }

  void normalize(){
    float repLsq = 1.0f / sqrtf(x * x + y * y + z * z + w * w);
    x *= repLsq;
    y *= repLsq;
    z *= repLsq;
    w *= repLsq;      
  }

  quat &rotationAxis(float angleRad, const p3d &axis);
  void slerp(const quat &q1, const quat &q2, float koeff);

  quat operator *(const quat &q) const{
    return quat(
      q.y * z - q.z * y + q.w * x + q.x * w,
      q.z * x - q.x * z + q.w * y + q.y * w,
      q.x * y - q.y * x + q.w * z + q.z * w,
      q.w * w - q.x * x - q.y * y - q.z * z);
  }  

  void toMatrix(m4x4 &out) const;
};

