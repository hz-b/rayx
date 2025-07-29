#pragma once

#include "ElectricField.h"
#include "Rand.h"

namespace RAYX {

struct FresnelCoeffs {
    double s;
    double p;
};

struct ComplexFresnelCoeffs {
    complex::Complex s;
    complex::Complex p;
};

/// calculates the angle in rad, between two unit vectors
RAYX_FN_ACC inline double angleBetweenUnitVectors(glm::dvec3 a, glm::dvec3 b) { return glm::acos(glm::dot(a, b)); }

RAYX_FN_ACC
inline complex::Complex calcRefractAngle(const complex::Complex incidentAngle, const complex::Complex iorI, const complex::Complex iorT) {
    return complex::asin((iorI / iorT) * complex::sin(incidentAngle));
}

RAYX_FN_ACC
inline complex::Complex calcBrewstersAngle(const complex::Complex iorI, const complex::Complex iorT) { return complex::atan(iorT / iorI); }

RAYX_FN_ACC
inline complex::Complex calcCriticalAngle(const complex::Complex iorI, const complex::Complex iorT) { return complex::asin(iorT / iorI); }

RAYX_FN_ACC
inline ComplexFresnelCoeffs calcReflectAmplitude(const complex::Complex incidentAngle, const complex::Complex refractAngle,
                                                 const complex::Complex iorI, const complex::Complex iorT) {
    const auto cos_i = complex::cos(incidentAngle);
    const auto cos_t = complex::cos(refractAngle);

    const auto s = (iorI * cos_i - iorT * cos_t) / (iorI * cos_i + iorT * cos_t);
    const auto p = (iorT * cos_i - iorI * cos_t) / (iorT * cos_i + iorI * cos_t);

    return {
        .s = s,
        .p = p,
    };
}

RAYX_FN_ACC
inline ComplexFresnelCoeffs calcRefractAmplitude(const complex::Complex incidentAngle, const complex::Complex refractAngle,
                                                 const complex::Complex iorI, const complex::Complex iorT) {
    const auto cos_i = complex::cos(incidentAngle);
    const auto cos_t = complex::cos(refractAngle);

    const auto s = (2.0 * iorI * cos_i) / (iorI * cos_i + iorT * cos_t);
    const auto p = (2.0 * iorI * cos_i) / (iorT * cos_i + iorI * cos_t);

    return {
        .s = s,
        .p = p,
    };
}

RAYX_FN_ACC
inline FresnelCoeffs calcReflectIntensity(const ComplexFresnelCoeffs reflectAmplitude) {
    const auto s = (reflectAmplitude.s * complex::conj(reflectAmplitude.s)).real();
    const auto p = (reflectAmplitude.p * complex::conj(reflectAmplitude.p)).real();

    return {
        .s = s,
        .p = p,
    };
}

RAYX_FN_ACC
inline FresnelCoeffs calcRefractIntensity(const ComplexFresnelCoeffs refract_amplitude, const complex::Complex incidentAngle,
                                          const complex::Complex refractAngle, const complex::Complex iorI, const complex::Complex iorT) {
    const auto r = ((iorT * complex::cos(refractAngle)) / (iorI * complex::cos(incidentAngle))).real();

    const auto s = r * (refract_amplitude.s * complex::conj(refract_amplitude.s)).real();
    const auto p = r * (refract_amplitude.p * complex::conj(refract_amplitude.p)).real();

    return {
        .s = s,
        .p = p,
    };
}

RAYX_FN_ACC
inline cmat3 calcJonesMatrix(const ComplexFresnelCoeffs amplitude) {
    return {
        amplitude.s, 0, 0, 0, amplitude.p, 0, 0, 0, 1,
    };
}

RAYX_FN_ACC
inline cmat3 calcPolaririzationMatrix(const glm::dvec3 incidentVec, const glm::dvec3 reflectOrRefractVec, const glm::dvec3 normalVec,
                                      const ComplexFresnelCoeffs amplitude) {
    // TODO: cross product is not numerically stable here, if indicentVec == reflectOrRefractVec
    // we dont need the lenght of the vector resulting from the cross product, but only the direciton.
    // this may could be done by calculation angles instead of a vector.
    // fixing this removes the check in interceptReflect, that specializes for normal incidence
    glm::dvec3 s0;
    if (incidentVec == glm::dvec3(0.0, 0.0, 1.0)) {
        s0 = glm::dvec3(1.0, 0.0, 0.0);  // beliebig orthogonal zu (0,0,-1)
    } else {
        s0 = glm::normalize(glm::cross(incidentVec, -normalVec));
    }
    const auto s1 = s0;
    const auto p0 = glm::cross(incidentVec, s0);
    const auto p1 = glm::cross(reflectOrRefractVec, s0);

    const auto out = glm::dmat3(s1, p1, reflectOrRefractVec);

    const auto in = glm::dmat3(s0.x, p0.x, incidentVec.x, s0.y, p0.y, incidentVec.y, s0.z, p0.z, incidentVec.z);

    const auto jonesMatrix = calcJonesMatrix(amplitude);

    return out * jonesMatrix * in;
}

RAYX_FN_ACC
inline cmat3 calcPolaririzationMatrixFoil(const glm::dvec3 incidentVec, const glm::dvec3 normalVec, const ComplexFresnelCoeffs amplitude) {
    glm::dvec3 s0;
    if (glm::length(glm::cross(incidentVec, normalVec)) < 1e-10) {
        if (std::abs(incidentVec.x) > 1e-6) {
            s0 = glm::normalize(glm::dvec3(-incidentVec.y, incidentVec.x, 0.0));
        } else {
            s0 = glm::normalize(glm::dvec3(0.0, -incidentVec.z, incidentVec.y));
        }
    } else {
        s0 = glm::normalize(glm::cross(incidentVec, normalVec));
    }

    const glm::dvec3 p0 = glm::normalize(glm::cross(incidentVec, s0));

    const glm::dmat3 in = glm::dmat3(s0.x, p0.x, incidentVec.x, s0.y, p0.y, incidentVec.y, s0.z, p0.z, incidentVec.z);

    const cmat3 jonesMatrix = calcJonesMatrix(amplitude);

    const glm::dmat3 out = glm::transpose(in);

    return out * jonesMatrix * in;
}

RAYX_FN_ACC
inline cmat3 calcReflectPolarizationMatrixAtNormalIncidence(const ComplexFresnelCoeffs amplitude) {
    // since no plane of incidence is defined at normal incidence,
    // s and p components are equal and only contain the base reflectivity and a phase shift of 180 degrees
    // here we apply the base reflectivity and phase shift independent of the ray direction to all components
    return {
        amplitude.s, 0, 0, 0, amplitude.s, 0, 0, 0, amplitude.s,
    };
}

// intercept reflection with one coating layer
//RAYX_FN_ACC
//inline ElectricField interceptReflectOneLayer(const ElectricField incidentElectricField, const glm::dvec3 incidentVec, const glm::dvec3 reflectVec,
//                                      const glm::dvec3 normalVec, const complex::Complex vacuum_ior, const complex::Complex coating_iorT, const complex::Complex substrate_ior) {
//    const auto incidentAngle = complex::Complex(angleBetweenUnitVectors(incidentVec, -normalVec), 0);
//    const auto refractAngle = calcRefractAngle(incidentAngle, vacuum_ior, coating_iorT);
//
//    // calculate the transmittance from vacuum to coating
//    const auto transmittance = calcRefractAmplitude(incidentAngle, refractAngle,
//                                                    vacuum_ior, coating_iorT);
//
//    const auto reflectAmplitude = calcReflectAmplitude(incidentAngle, refractAngle, vacuum_ior, coating_iorT);
//
//    const auto reflectElectricField = reflectAmplitude * incidentElectricField;
//
//    return reflectElectricField;
//
//}

// Berechnet die komplexe Reflexionsamplitude für eine einzelne dünne Schicht (ohne Rauigkeit)
RAYX_FN_ACC
inline ComplexFresnelCoeffs computeSingleCoatingReflectance(
    const complex::Complex incidentAngle,
    const double wavelength,
    const double thickness,
    const complex::Complex iorI,   // n0: z. B. Vakuum
    const complex::Complex iorC,   // n1: Beschichtung
    const complex::Complex iorS    // n2: Substrat
) {
    // Winkel in der Beschichtung und im Substrat
    const auto theta1 = calcRefractAngle(incidentAngle, iorI, iorC);
    const auto theta2 = calcRefractAngle(theta1, iorC, iorS);

    // Fresnel‑Reflexion an beiden Grenzflächen
    auto r01 = calcReflectAmplitude(incidentAngle, theta1, iorI, iorC);
    auto r12 = calcReflectAmplitude(theta1, theta2, iorC, iorS);

    // Phasenverschiebung im Film
    const auto delta = (2.0 * M_PI / wavelength) * iorC * complex::cos(theta1) * thickness;
    const auto phase = complex::exp(complex::Complex(0.0, 2.0) * delta);

    // Interferenzformel (Parratt für 1 Schicht)
    const auto r_s = (r01.s + r12.s * phase) / (complex::Complex(1.0) + r01.s * r12.s * phase);
    const auto r_p = (r01.p + r12.p * phase) / (complex::Complex(1.0) + r01.p * r12.p * phase);

    return {r_s, r_p};
}



//RAYX_FN_ACC
//inline ElectricField interceptTransmit(const ElectricField incidentElectricField, const glm::dvec3 incidentVec, const glm::dvec3 normalVec,
//                                       const complex::Complex iorI, const complex::Complex iorT) {
//    const auto incidentAngle = complex::Complex(angleBetweenUnitVectors(incidentVec, -normalVec), 0);
//    const auto refractAngle = calcRefractAngle(incidentAngle, iorI, iorT);
//
//    const auto transmitAmplitude = calcTransmitAmplitude(incidentAngle, refractAngle, iorI, iorT);
//
//    const auto isNormalIncidence = incidentVec == -normalVec;
//    const auto transmitPolarizationMatrix = isNormalIncidence ? calcReflectPolarizationMatrixAtNormalIncidence(transmitAmplitude)
//                                                               : calcPolaririzationMatrix(incidentVec, incidentVec, normalVec, transmitAmplitude);
//
//    return transmitPolarizationMatrix * incidentElectricField;
//}

RAYX_FN_ACC
inline ElectricField interceptReflect(const ElectricField incidentElectricField, const glm::dvec3 incidentVec, const glm::dvec3 reflectVec,
                                      const glm::dvec3 normalVec, const complex::Complex iorI, const complex::Complex iorT) {
    const auto incidentAngle = complex::Complex(angleBetweenUnitVectors(incidentVec, -normalVec), 0);
    const auto refractAngle  = calcRefractAngle(incidentAngle, iorI, iorT);

    const auto reflectAmplitude = calcReflectAmplitude(incidentAngle, refractAngle, iorI, iorT);

    // TODO: make this more robust
    const auto isNormalIncidence         = incidentVec == -normalVec;
    const auto reflectPolarizationMatrix = isNormalIncidence ? calcReflectPolarizationMatrixAtNormalIncidence(reflectAmplitude)
                                                             : calcPolaririzationMatrix(incidentVec, reflectVec, normalVec, reflectAmplitude);

    const auto reflectElectricField = reflectPolarizationMatrix * incidentElectricField;
    return reflectElectricField;
}

RAYX_FN_ACC
inline ElectricField interceptReflectCrystal(const ElectricField incidentElectricField, const glm::dvec3 incidentVec, const glm::dvec3 reflectVec,
                                             const glm::dvec3 normalVec, ComplexFresnelCoeffs reflectAmplitude) {
    // TODO: make this more robust
    const auto reflectPolarizationMatrix = calcPolaririzationMatrix(incidentVec, reflectVec, normalVec, reflectAmplitude);

    const auto reflectElectricField = reflectPolarizationMatrix * incidentElectricField;
    return reflectElectricField;
}

RAYX_FN_ACC
inline ElectricField interceptFoil(const ElectricField incidentElectricField, const glm::dvec3 incidentVec, const glm::dvec3 normalVec,
                                   ComplexFresnelCoeffs transCoeffs) {
    // TODO: make this more robust
    const auto transmittPolarizationMatrix = calcPolaririzationMatrixFoil(incidentVec, normalVec, transCoeffs);

    const auto transmittElectricField = transmittPolarizationMatrix * incidentElectricField;
    return transmittElectricField;
}

}  // namespace RAYX
