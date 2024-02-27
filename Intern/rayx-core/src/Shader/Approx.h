#ifndef APPROX_H
#define APPROX_H

#include "Common.h"

/**
 * DOUBLE PRECISION APPROXIMATION FOR ARCTAN AND COSINUS
 * source: https://people.sc.fsu.edu/~jburkardt/c_src/fn/fn.html
 */

double RAYX_API r8_aint(double x);
int RAYX_API r8_inits(double dos[16], int nos, double eta);
void RAYX_API r8_upak(double x, RAYX_INOUT(double) y, RAYX_INOUT(int) n);
int i4_mach(int i);
double RAYX_API r8_mach(int i);
double RAYX_API r8_pak(double y, int n);
double RAYX_API r8_csevl(double x, double a[16], int n);
double RAYX_API r8_cos(const double x);
double RAYX_API r8_atan(const double x);
double RAYX_API r8_atan2(double sn, double cs);
double RAYX_API r8_log(double x);
double RAYX_API r8_exp(double x);
double RAYX_API r8_sin(const double x);
double RAYX_API r8_asin(const double x);
double RAYX_API r8_acos(const double x);
double RAYX_API r8_sinh(const double x);
double RAYX_API r8_cosh(const double x);
double RAYX_API dpow(double a, int b);

#endif
