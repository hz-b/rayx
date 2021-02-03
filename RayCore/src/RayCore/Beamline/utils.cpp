#include "utils.h"

/**
 * calculates photon wavelength (nm) from its energy (eV) or vice verse
 * taken from RAYLIB.FOR
*/
double hvlam(double x) {
    if(x==0) {
        return 0.0;
    }
    return inm2eV / x;
}

/**
 * from degree to rad
*/
double rad(double degree) {
    return degree  * PI/180;
}

/**
 * from rad to degree
*/
double degree(double rad) {
    return rad *180/PI;
}