#ifndef MISALIGNMENT_H
#define MISALIGNMENT_H

struct Misalignment {
    double m_translationXerror;
    double m_translationYerror;
    double m_translationZerror;

    // these are in radians. Sadly, the Rad type cannot be used in the shader.
    double m_rotationXerror;
    double m_rotationYerror;
    double m_rotationZerror;
};

#endif
