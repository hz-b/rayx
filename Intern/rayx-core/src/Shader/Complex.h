#pragma once

#include "Common.h"

/* COMPLEX NUMBERS */
// make a struct for complex numbers or just use vec2?

// Complex Number math by julesb (but adapted for double precision)
// https://github.com/julesb/glsl-util

#define cx_mul(a, b)  dvec2(a.x* b.x - a.y * b.y, a.x * b.y + a.y * b.x)
#define cx_div(a, b)  dvec2(((a.x * b.x + a.y * b.y) / (b.x * b.x + b.y * b.y)), ((a.y * b.x - a.x * b.y) / (b.x * b.x + b.y * b.y)))
#define cx_modulus(a) length(a)
#define cx_conj(a)    dvec2(a.x, -a.y)
#define cx_arg(a)     r8_atan(a.y, a.x)
#define cx_sin(a)     dvec2(r8_sin(a.x) * r8_cosh(a.y), r8_cos(a.x) * r8_sinh(a.y))
#define cx_cos(a)     dvec2(r8_cos(a.x) * r8_cosh(a.y), -r8_sin(a.x) * r8_sinh(a.y))

RAYX_FUNC dvec2 cx_sqrt(dvec2 a);
RAYX_FUNC dvec2 cx_tan(dvec2 a);
RAYX_FUNC dvec2 cx_log(dvec2 a);
RAYX_FUNC dvec2 cx_mobius(dvec2 a);
RAYX_FUNC dvec2 cx_z_plus_one_over_z(dvec2 a);
RAYX_FUNC dvec2 cx_z_squared_plus_c(dvec2 z, dvec2 c);
RAYX_FUNC dvec2 cx_sin_of_one_over_z(dvec2 z);

// Additions by Johan Karlsson (DonKarlssonSan) https://gist.github.com/DonKarlssonSan
// https://gist.github.com/DonKarlssonSan/f87ba5e4e5f1093cb83e39024a6a5e72
#define cx_sub(a, b) dvec2(a.x - b.x, a.y - b.y)
#define cx_add(a, b) dvec2(a.x + b.x, a.y + b.y)
#define cx_abs(a)    length(a)
RAYX_FUNC dvec2 cx_to_polar(dvec2 a);

// Complex power
RAYX_FUNC dvec2 cx_pow(dvec2 a, int n);
