
namespace fg {
    namespace math {
        namespace utility {
            bool getLinesIntersect2D(const p2d &lineA0, const p2d &lineA1, const p2d &lineB0, const p2d &lineB1, p2d &out) {
                float k1 = ((lineB1.x - lineB0.x) * (lineA0.y - lineB0.y) - (lineB1.y - lineB0.y) * (lineA0.x - lineB0.x)) / ((lineB1.y - lineB0.y) * (lineA1.x - lineA0.x) - (lineB1.x - lineB0.x) * (lineA1.y - lineA0.y));
                float k2 = ((lineA1.x - lineA0.x) * (lineA0.y - lineB0.y) - (lineA1.y - lineA0.y) * (lineA0.x - lineB0.x)) / ((lineB1.y - lineB0.y) * (lineA1.x - lineA0.x) - (lineB1.x - lineB0.x) * (lineA1.y - lineA0.y));

                if(k1 >= 0.0f && k1 <= 1.0f) {
                    if(k2 >= 0.0f && k2 <= 1.0f) {
                        out = lineA0 + (lineA1 - lineA0) * k1;
                        return true;
                    }
                }
                return false;
            }

            bool getCurvePoint(const p2d &p0, const p2d &p1, const p2d &p2, float koeff, p2d &pOut) {
                if(koeff > 1.0f) {
                    pOut = p2;
                    return true;
                }

                float t = (1 - koeff);
                pOut.x = t * t * p0.x + 2 * koeff * t * p1.x + koeff * koeff * p2.x;
                pOut.y = t * t * p0.y + 2 * koeff * t * p1.y + koeff * koeff * p2.y;
                return false;
            }

            bool getCurvePoint(const p3d &p0, const p3d &p1, const p3d &p2, float koeff, p3d &pOut) {
                if(koeff > 1.0f) {
                    pOut = p2;
                    return true;
                }

                float t = (1 - koeff);
                pOut.x = t * t * p0.x + 2 * koeff * t * p1.x + koeff * koeff * p2.x;
                pOut.y = t * t * p0.y + 2 * koeff * t * p1.y + koeff * koeff * p2.y;
                pOut.z = t * t * p0.z + 2 * koeff * t * p1.z + koeff * koeff * p2.z;
                return false;
            }
        }
    }
}