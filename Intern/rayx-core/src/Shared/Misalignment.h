#ifndef MISALIGNMENT_H
#define MISALIGNMENT_H

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

// TODO also apply rotational parts of Misalignment.
INLINE Ray addMisalignment(Ray r, Misalignment m) {
    r.m_position.x += m.m_translationXerror;
    r.m_position.y += m.m_translationYerror;
    r.m_position.z += m.m_translationZerror;
    return r;
}

INLINE Ray subtractMisalignment(Ray r, Misalignment m) {
    r.m_position.x -= m.m_translationXerror;
    r.m_position.y -= m.m_translationYerror;
    r.m_position.z -= m.m_translationZerror;
    return r;
}

#endif
