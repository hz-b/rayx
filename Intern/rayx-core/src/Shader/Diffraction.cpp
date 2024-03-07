#include "Diffraction.h"

RAYX_FUNC
double RAYX_API fact(int a) {
    if (a < 0) { return a; }
    double f = 1;
    for (int i = 2; i <= a; i++) {
        f *= i;
    }
    return f;
}

/**returns first bessel function of parameter v*/
RAYX_FUNC
double RAYX_API bessel1(double v) {
    if (v < 0.0 || v > 20.0) {
        return 0.0;
    }

    double sum = 0;
    int large = 30;

    double PO1;
    double PO2;
    double FA1;
    for (int small = 0; small <= large; small++) {
        PO1 = dpow(-1.0, small);
        PO2 = dpow(v / 2.0, 2 * small + 1);
        FA1 = fact(small);
        sum += (PO1 / (FA1 * FA1 * (small + 1))) * PO2;
    }
    return sum;
}
