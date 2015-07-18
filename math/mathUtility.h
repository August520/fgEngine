
namespace fg {
    namespace math {
        static inline float fmin(float v1, float v2) {
            return v1 < v2 ? v1 : v2;
        }

        static inline float fmax(float v1, float v2) {
            return v1 > v2 ? v1 : v2;
        }

        static inline float clamp(float value, float vmin, float vmax) {
            return fmax(fmin(value, vmax), vmin);
        }

        static inline float flerp(float v1, float v2, float koeff) {
            return v1 + (v2 - v1) * koeff;
        }

        static inline float fsign(float value) {
            return ((unsigned &)value) & 0x80000000 ? -1.0f : 1.0f;
        }

        namespace utility {
            static bool getLinesIntersect2D(const p2d &lineA0, const p2d &lineA1, const p2d &lineB0, const p2d &lineB1, p2d &out);
            static bool getCurvePoint(const p2d &p0, const p2d &p1, const p2d &p2, float koeff, p2d &pOut);
            static bool getCurvePoint(const p3d &p0, const p3d &p1, const p3d &p2, float koeff, p3d &pOut);

            // todo: math with directional, distance 
        }
    }
}