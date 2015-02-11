
struct p4d{
  float x, y, z, w;

  p4d(float _x = 0, float _y = 0, float _z = 0, float _w = 0);
  p4d(const p2d &v);
  p4d(const p3d &v);
  p4d(const p4d &v);

  p4d &transform(const m4x4 &im);
};


