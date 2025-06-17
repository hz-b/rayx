#include "Refrac.h"

#include "Ray.h"

namespace RAYX {

/**
calculates refracted ray
@params: 	r: ray
            normal: normal at intersection point of ray and element
            az: linedensity in z direction varied spacing for different collision angles is already considered
            ax: linedensity in x direction
@returns: refracted ray (position unchanged, direction changed), weight = EventType::BeyondHorizon if
"ray beyond horizon"
*/
RAYX_FN_ACC
Ray refrac2D(Ray r, glm::dvec3 normal, double density_z, double density_x) {
    // Rotation to fit collision normal to element normal (see Wiki)
    double eps1 = -glm::atan(normal.x / normal.y);
    double del1 = glm::asin(normal.z);

    double cos_d = glm::cos(-del1);
    double sin_d = glm::sin(-del1);
    double cos_e = glm::cos(-eps1);
    double sin_e = glm::sin(-eps1);
    auto rot = glm::dmat3(cos_e, cos_d * sin_e, sin_d * sin_e, -sin_e, cos_d * cos_e, sin_d * cos_e, 0, -sin_d, cos_d);
    auto inv_rot = glm::dmat3(cos_e, -sin_e, 0, cos_d * sin_e, cos_d * cos_e, -sin_d, sin_d * sin_e, sin_d * cos_e, cos_d);
    r.m_direction =
        rot * r.m_direction;  // ! The rotation should not be applied if the normal is (0, 1, 0) but it is applied in RAY-UI so we do it too

    double x1 = r.m_direction.x - density_x;
    double z1 = r.m_direction.z - density_z;
    double y1 = 1 - x1 * x1 - z1 * z1;

    if (y1 > 0) {
        y1 = sqrt(y1);

        r.m_direction.x = x1;
        r.m_direction.y = y1;
        r.m_direction.z = z1;
        r.m_direction = inv_rot * r.m_direction;
    } else {  // beyond horizon - when divergence too large
        return terminateRay(r, EventType::BeyondHorizon);
    }
    return r;
}

/**
 * Simple refraction for double precision vectors
 * @params:  I: incident vector
 *           N: normal vector at the point of intersection
 *           eta: ratio of refractive indices (n1/n2)
 * @returns: refracted vector, or zero vector if total internal reflection occurs
 */
RAYX_FN_ACC
glm::dvec3 refract_dvec3(glm::dvec3 I, glm::dvec3 N, double eta) {
    double cosi = glm::dot(-I, N);
    double cost2 = 1.0 - eta * eta * (1.0 - cosi * cosi);
    if (cost2 < 0.0) return glm::dvec3(0.0); // Total internal reflection
    return eta * I + (eta * cosi - sqrt(cost2)) * N;
}



RAYX_FN_ACC
complex::Complex computeTransmittance(
    double wl,          // Wellenlänge in nm
    double alpha_deg,   // Einfallswinkel in Grad
    const complex::Complex& n1,  // Brechungsindex Eintrittsmedium (komplex)
    const complex::Complex& n2,  // Brechungsindex Folie (komplex)
    double d            // Schichtdicke Folie in nm
) {
    constexpr double PI = 3.14159265358979323846;
    const complex::Complex I(0.0, 1.0);  // imaginäre Einheit

    // Umrechnung Winkel in Bogenmaß
    double theta1 = alpha_deg * PI / 180.0;

    // Snellius-Gesetz: n1*sin(theta1) = n2*sin(theta2) => Berechne theta2
    complex::Complex sin_theta2 = (n1 / n2) * std::sin(theta1);
    complex::Complex theta2 = asin(sin_theta2); // Complex asin wird in deinem Typ vorausgesetzt

    // Wellenzahl im Medium 2
    complex::Complex k0 = complex::Complex(2.0 * PI / wl, 0.0); // k0 = 2π/λ

    // Phasenverschiebung durch die Schicht
    complex::Complex delta = k0 * n2 * d * std::cos(theta2);

    // Fresnel Koeffizienten (für s-Polarisation als Beispiel)
    complex::Complex rs = (n1 * std::cos(theta1) - n2 * std::cos(theta2)) /
                 (n1 * std::cos(theta1) + n2 * std::cos(theta2));
    complex::Complex ts = complex::Complex(1.0, 0.0) + rs; // Transmissionsamplitude an erster Grenzfläche

    // Transmission durch die Schicht mit Interferenz
    complex::Complex numerator = ts * std::exp(-I * delta);
    complex::Complex denominator = complex::Complex(1.0, 0.0) + rs * std::exp(-complex::Complex(2.0, 0.0) * I * delta);

    complex::Complex T = numerator / denominator; // Komplexe Transmissionsamplitude

    return T;
}


}  // namespace RAYX
