#include "Approx.h"
#include "Constants.h"

namespace RAYX {

// double pow simple
RAYX_FUNC
double RAYX_API dpow(double a, int b) {
    double res = 1;
    for (int i = 1; i <= b; i++) {
        res *= a;
    }
    return res;
}

} // namespace RAYX
