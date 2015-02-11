
quat::quat(const m4x4 &mat){
  mat.toQuaternion(*this);
}

quat &quat::rotationAxis(float angleRad, const p3d &axis){
  float sina = sinf(angleRad * 0.5f);
  float cosa = cosf(angleRad * 0.5f);

  w = cosa;
  x = axis.x * sina;
  y = axis.y * sina;
  z = axis.z * sina;
  return *this;
}

void quat::slerp(const quat &q1, const quat &q2, float koeff){
  quat tmpq = q2;

  float cosT = q1.x * tmpq.x + q1.y * tmpq.y + q1.z * tmpq.z + q1.w * tmpq.w;
  float theta, sinT, k1, k2;

  if(cosT < 0.0f){
    tmpq.negative();
    cosT = -cosT;
  } 
  if(cosT > 0.9999f) cosT = 0.9999f;

  theta = acosf(cosT);
  sinT = sqrtf(1.0f - cosT * cosT);
  k1 = sin((1.0f - koeff) * theta) / sinT;
  k2 = sin(koeff * theta) / sinT;

  x = q1.x * k1 + tmpq.x * k2;
  y = q1.y * k1 + tmpq.y * k2;
  z = q1.z * k1 + tmpq.z * k2;
  w = q1.w * k1 + tmpq.w * k2;
}   

void quat::toMatrix(m4x4 &out) const{
  float xx = x * x;
  float xy = x * y;
  float xz = x * z;
  float xw = x * w;
  float yy = y * y;
  float yz = y * z;
  float yw = y * w;
  float zz = z * z;
  float zw = z * w;

  out._11 = 1.0f - 2.0f * (yy + zz);
  out._12 = 2.0f * (xy + zw);
  out._13 = 2.0f * (xz - yw);
  out._14 = 0.0f;

  out._21 = 2.0f * (xy - zw);
  out._22 = 1.0f - 2.0f * (xx + zz);
  out._23 = 2.0f * (yz + xw);
  out._24 = 0.0f;

  out._31 = 2.0f * (xz + yw);
  out._32 = 2.0f * (yz - xw);
  out._33 = 1.0f - 2.0f * (xx + yy);
  out._34 = 0.0f;

  out._41 = 0.0f;
  out._42 = 0.0f;
  out._43 = 0.0f;
  out._44 = 1.0f;
}

