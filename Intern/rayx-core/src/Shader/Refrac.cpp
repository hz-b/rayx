#include "Refrac.h"

#include "EventType.h"
#include "Helper.h"

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
Ray refrac2D(Ray r, glm::dvec3 normal, double density_z, double density_x, InvState& inv) {
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
        recordFinalEvent(r, EventType::BeyondHorizon, inv);
    }
    return r;
}

}  // namespace RAYX
