#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "Behaviour.h"
#include "Cutout.h"
#include "Surface.h"

// TODO extract this somewhere else
#ifndef GLSL

#include <glm.hpp>
using dmat4 = glm::dmat4;

#endif

struct Element {
    // for alignment reasons, the dmat4s are at the beginning of the struct.
    dmat4 m_inTrans;
    dmat4 m_outTrans;

    Behaviour m_behaviour;

    Surface m_surface;
    Cutout m_cutout;

    // general object information
    double m_slopeError[7];
    double m_azimuthalAngle;
    double m_material;

    // This field is unused, it's only there to guarantee that sizeof(Element) is divisible by sizeof(dmat4).
    // Should guarantee that std430 in GLSL and c++ have the same memory layout for `Element`.
    double m_padding[1];
};

#endif
