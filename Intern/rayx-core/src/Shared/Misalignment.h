#ifndef MISALIGNMENT_H
#define MISALIGNMENT_H

#include "adapt.h"
#include "Ray.h"

struct Misalignment {
    double m_translationXerror;
    double m_translationYerror;
    double m_translationZerror;

    // these are in radians. Sadly, the Rad type cannot be used in the shader.
    double m_rotationXerror;
    double m_rotationYerror;
    double m_rotationZerror;
};

// TODO find adapt.h solution for this.
#ifndef GLSL
using Ray = RAYX::Ray;
#endif

INLINE dmat4 getRotationMatrix(Misalignment m) {
    double dpsi = m.m_rotationXerror; // TODO consider negating this
    double dphi = m.m_rotationYerror;
    double dchi = m.m_rotationZerror;
    dmat4 misalignmentMatrix = dmat4(
        r8_cos(dphi) * r8_cos(dchi), -r8_cos(dpsi) * r8_sin(dchi) - r8_sin(dpsi) * r8_sin(dphi) * r8_cos(dchi), -r8_sin(dpsi) * r8_sin(dchi) + r8_cos(dpsi) * r8_sin(dphi) * r8_cos(dchi), 0,
        r8_sin(dchi) * r8_cos(dphi), r8_cos(dpsi) * r8_cos(dchi) - r8_sin(dpsi) * r8_sin(dphi) * r8_sin(dchi), r8_sin(dpsi) * r8_cos(dchi) + r8_cos(dpsi) * r8_sin(dphi) * r8_sin(dchi), 0,
        -r8_sin(dphi), -r8_sin(dpsi) * r8_cos(dphi), r8_cos(dpsi) * r8_cos(dphi), 0,
        0, 0, 0, 1);
    return misalignmentMatrix;
}

// TODO correctly integrate rotation
INLINE Ray addMisalignment(Ray r, Misalignment m) {
    dmat4 mat = getRotationMatrix(m);

    r.m_position.x += m.m_translationXerror;
    r.m_position.y += m.m_translationYerror;
    r.m_position.z += m.m_translationZerror;

    r.m_position = dvec3(transpose(mat) * dvec4(r.m_position, 1));
    r.m_direction = dvec3(transpose(mat) * dvec4(r.m_direction, 0));
    return r;
}

INLINE Ray subtractMisalignment(Ray r, Misalignment m) {
    dmat4 mat = getRotationMatrix(m);

    r.m_position = dvec3(mat * dvec4(r.m_position, 1));
    r.m_direction = dvec3(mat * dvec4(r.m_direction, 0));

    r.m_position.x -= m.m_translationXerror;
    r.m_position.y -= m.m_translationYerror;
    r.m_position.z -= m.m_translationZerror;
    return r;
}

#endif
