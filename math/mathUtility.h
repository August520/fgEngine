
namespace fg {
    namespace math {
        namespace utility {
            static bool getLinesIntersect2D(const p2d &lineA0, const p2d &lineA1, const p2d &lineB0, const p2d &lineB1, p2d &out);
            static bool getCurvePoint(const p2d &p0, const p2d &p1, const p2d &p2, float koeff, p2d &pOut);
            static bool getCurvePoint(const p3d &p0, const p3d &p1, const p3d &p2, float koeff, p3d &pOut);

            // todo: math with directional, distance 
        }
    }
}