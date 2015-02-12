
#include <math.h>

#ifndef NOMINMAX
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#endif

namespace fg {
    namespace math {
        #include "p2d.h"
        #include "p3d.h"
        #include "p4d.h"
        #include "quat.h"
        #include "m3x3.h"
        #include "m4x4.h"
    }
}