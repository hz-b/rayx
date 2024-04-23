#pragma once

#include "Common.h"

namespace RAYX {

/**
 * DOUBLE PRECISION APPROXIMATION FOR ARCTAN AND COSINUS
 * source: https://people.sc.fsu.edu/~jburkardt/c_src/fn/fn.html
 */

RAYX_FUNC double RAYX_API r8_aint(double x);
RAYX_FUNC int    RAYX_API r8_inits(double dos[16], int nos, double eta);
RAYX_FUNC void   RAYX_API r8_upak(double x, double& y, int& n);
RAYX_FUNC int    RAYX_API i4_mach(int i);
RAYX_FUNC double RAYX_API r8_mach(int i);
RAYX_FUNC double RAYX_API r8_pak(double y, int n);
RAYX_FUNC double RAYX_API r8_csevl(double x, double a[16], int n);
RAYX_FUNC double RAYX_API r8_cos(const double x);
RAYX_FUNC double RAYX_API r8_atan(const double x);
RAYX_FUNC double RAYX_API r8_atan2(double sn, double cs);
RAYX_FUNC double RAYX_API r8_log(double x);
RAYX_FUNC double RAYX_API r8_exp(double x);
RAYX_FUNC double RAYX_API r8_sin(const double x);
RAYX_FUNC double RAYX_API r8_asin(const double x);
RAYX_FUNC double RAYX_API r8_acos(const double x);
RAYX_FUNC double RAYX_API r8_sinh(const double x);
RAYX_FUNC double RAYX_API r8_cosh(const double x);
RAYX_FUNC double RAYX_API dpow(double a, int b);

} // namespace RAYX
