#include "Cylinder.h"

#include <ext/scalar_constants.hpp>

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shared/Constants.h"
#include "utils.h"

namespace RAYX {
Cylinder::Cylinder(const DesignObject& dobj) : OpticalElement(dobj) {
    m_direction = dobj.parseBendingRadius();
    m_radius = dobj.parseRadius();
    m_incidence = dobj.parseGrazingIncAngle();
    m_entranceArmLength = dobj.parseEntranceArmLength();
    m_exitArmLength = dobj.parseExitArmLength();

    RAYX_VERB << ((m_direction == CylinderDirection::LongRadiusR) ? "LONG RADIUS" : "SHORT RADIUS");
    if (m_direction == CylinderDirection::LongRadiusR) {  // X-DIR
        m_a11 = 0;
        m_a33 = 1;
        m_a24 = -m_radius;
    } else {  // Z_DIR/ SHORT_RADIUS_RHO
        m_a11 = 1;
        m_a33 = 0;
        m_a24 = -m_radius;
    }
    int icurv = 1;
    if (m_a24 > 0) icurv = -1;  // Translated from RAY.FOR
    if (m_radius == 0) {
        setRadius();
    }

    m_surface = serializeQuadric({
        .m_icurv = icurv,
        .m_a11 = m_a11,
        .m_a12 = 0,
        .m_a13 = 0,
        .m_a14 = 0,
        .m_a22 = 1,
        .m_a23 = 0,
        .m_a24 = m_a24,
        .m_a33 = m_a33,
        .m_a34 = 0,
        .m_a44 = 0,
    });
    m_behaviour = serializeMirror();
}

/**
 * @brief Call when m_radius is 0 and needs to auto-calculate
 *
 */
void Cylinder::setRadius() {
    double theta = m_incidence.toDeg().deg * glm::pi<double>() / 180.0;
    if (m_direction == CylinderDirection::LongRadiusR) {
        m_radius = 2.0 / sin(theta) / (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength);
    } else {
        if (m_entranceArmLength == 0.0 || m_exitArmLength == 0.0 || theta == 0.0) {
            m_radius = 0.0;
        } else {
            m_radius = 2.0 * sin(theta) / (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength);
        }
    }

    RAYX_VERB << "Radius: " << m_radius;
}

CylinderDirection Cylinder::getDirection() const { return m_direction; }
double Cylinder::getRadius() const { return m_radius; }
Rad Cylinder::getIncidenceAngle() const { return m_incidence; }
double Cylinder::getExitArmLength() const { return m_exitArmLength; }
double Cylinder::getEntranceArmLength() const { return m_entranceArmLength; }
}  // namespace RAYX
