#include "Refrac.h"

#include "Ray.h"

namespace RAYX {

/**
calculates refracted ray
@params: 	ray: Ray
            normal: normal at intersection point of ray and element
            az: linedensity in z direction varied spacing for different collision angles is already considered
            ax: linedensity in x direction
@returns: refracted ray (position unchanged, direction changed), weight = EventType::BeyondHorizon if
"ray beyond horizon"
*/
RAYX_FN_ACC
void refrac2D(detail::Ray& __restrict ray, glm::dvec3 normal, double density_z, double density_x) {
    // Rotation to fit collision normal to element normal (see Wiki)
    double eps1 = -glm::atan(normal.x / normal.y);
    double del1 = glm::asin(normal.z);

    double cos_d = glm::cos(-del1);
    double sin_d = glm::sin(-del1);
    double cos_e = glm::cos(-eps1);
    double sin_e = glm::sin(-eps1);
    auto rot = glm::dmat3(cos_e, cos_d * sin_e, sin_d * sin_e, -sin_e, cos_d * cos_e, sin_d * cos_e, 0, -sin_d, cos_d);
    auto inv_rot = glm::dmat3(cos_e, -sin_e, 0, cos_d * sin_e, cos_d * cos_e, -sin_d, sin_d * sin_e, sin_d * cos_e, cos_d);
    ray.direction =
        rot * ray.direction;  // ! The rotation should not be applied if the normal is (0, 1, 0) but it is applied in RAY-UI so we do it too

    double x1 = ray.direction.x - density_x;
    double z1 = ray.direction.z - density_z;
    double y1 = 1 - x1 * x1 - z1 * z1;

    if (y1 > 0) {
        y1 = sqrt(y1);

        ray.direction.x = x1;
        ray.direction.y = y1;
        ray.direction.z = z1;
        ray.direction = inv_rot * ray.direction;
    } else {  // beyond horizon - when divergence too large
        terminateRay(ray.event_type, EventType::BeyondHorizon);
    }
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
    if (cost2 < 0.0) return glm::dvec3(0.0);  // Total internal reflection
    return eta * I + (eta * cosi - sqrt(cost2)) * N;
}

}  // namespace RAYX
