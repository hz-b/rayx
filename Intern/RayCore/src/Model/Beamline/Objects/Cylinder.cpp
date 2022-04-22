#include "Cylinder.h"

#include "Debug.h"

namespace RAYX {
/**
 * @brief Construct a new Cylinder object
 *
 * @param name              Cylinder name
 * @param geometricalShape
 * @param radius            Cylinder radius (if 0, radius auto
 * calculate)
 * @param direction         Direction/Bending radius of Cylinder,
 * (LONG_RADIUS/SHORT_RADIUS)
 * @param width             width of the mirror (x-dimension)
 * @param height            height of the mirror (z-dimension)
 * @param azimuthalAngle        azimuthal angle of object (rotation in xy plane
 * with respect to previous element) in rad
 * @param position          position of the element in world coordinates
 * @param orientation       orientation of the element in world coordinates
 * @param grazingIncidence  desired incidence angle of the main ray, in deg
 * @param entranceArmLength length of entrance arm
 * @param exitArmLength     length of exit arm
 * @param slopeError        7 slope error parameters: x-y sagittal (0), y-z
 * meridional (1), thermal distortion: x (2),y (3),z (4), cylindrical bowing
 * amplitude y(5) and radius (6)
 * @param mat               material (See Material.h)
 *
 */
Cylinder::Cylinder(const char* name,
                   Geometry::GeometricalShape geometricalShape,
                   const double radius, CylinderDirection direction,
                   const double width, const double height,
                   const double azimuthalAngle, glm::dvec4 position,
                   glm::dmat4x4 orientation, const double grazingIncidence,
                   const double entranceArmLength, const double exitArmLength,
                   const std::array<double, 7> slopeError, Material mat)
    : OpticalElement(name, geometricalShape, width, height, azimuthalAngle,
                     position, orientation, slopeError),
      m_direction(direction),
      m_radius(radius),
      m_incidence(degToRad(grazingIncidence)),
      m_entranceArmLength(entranceArmLength),
      m_exitArmLength(exitArmLength) {
    RAYX_LOG << name;
    RAYX_LOG << ((m_direction == CylinderDirection::LongRadiusR)
                     ? "LONG RADIUS"
                     : "SHORT RADIUS");
    if (m_direction == CylinderDirection::LongRadiusR) {  // X-DIR
        m_a11 = 0;
        m_a33 = 1;
        m_a24 = -radius;
    } else {  // Z_DIR/ SHORT_RADIUS_RHO
        m_a11 = 1;
        m_a33 = 0;
        m_a24 = -radius;
    }
    double icurv = 1;
    if (m_a24 > 0) icurv = -1;  // Translated from RAY.FOR
    if (m_radius == 0) {
        setRadius();
    }

    double matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Quadric>(std::array<double, 4 * 4>{
        m_a11, 0, 0, 0, icurv, 1, 0, m_a24, 0, 0, m_a33, 0, 0, 0, matd, 0}));
    // TODO (OS): Add Element parameters?
    setElementParameters({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
}
Cylinder::~Cylinder() {}

/**
 * @brief Call when m_radius is 0 and needs to auto-calculate
 *
 */
void Cylinder::setRadius() {
    double theta = radToDeg(m_incidence) * M_PI / 180.0;
    if (m_direction == CylinderDirection::LongRadiusR) {
        m_radius = 2.0 / sin(theta) /
                   (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength);
    } else {
        if (m_entranceArmLength == 0.0 || m_exitArmLength == 0.0 ||
            theta == 0.0) {
            m_radius = 0.0;
        } else {
            m_radius = 2.0 * sin(theta) /
                       (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength);
        }
    }

    RAYX_LOG << "Radius: " << m_radius;
}

std::shared_ptr<Cylinder> Cylinder::createFromXML(
    rapidxml::xml_node<>* node, const std::vector<xml::Group>& group_context) {
    const char* name = node->first_attribute("name")->value();

    int gs;
    if (!xml::paramInt(node, "geometricalShape", &gs)) {
        return nullptr;
    }
    Geometry::GeometricalShape geometricalShape =
        static_cast<Geometry::GeometricalShape>(
            gs);  // HACK(Jannis): convert to enum

    double width;
    if (!xml::paramDouble(node, "totalWidth", &width)) {
        return nullptr;
    }

    double height;
    if (!xml::paramDouble(node, "totalLength", &height)) {
        return nullptr;
    }

    glm::dvec4 position;
    glm::dmat4x4 orientation;
    if (!xml::paramPositionAndOrientation(node, group_context, &position,
                                          &orientation)) {
        return nullptr;
    }

    double incidenceAngle;
    if (!xml::paramDouble(node, "grazingIncAngle", &incidenceAngle)) {
        return nullptr;
    }

    double mEntrance;
    if (!xml::paramDouble(node, "entranceArmLength", &mEntrance)) {
        return nullptr;
    }

    double mExit;
    if (!xml::paramDouble(node, "exitArmLength", &mExit)) {
        return nullptr;
    }

    double mRadius;
    if (!xml::paramDouble(node, "radius", &mRadius)) {
        return nullptr;
    }

    int mBendingRadius;
    if (!xml::paramInt(node, "bendingRadius", &mBendingRadius)) {
        return nullptr;
    }

    double azimuthalAngle;
    if (!xml::paramDouble(node, "azimuthalAngle", &azimuthalAngle)) {
        return nullptr;
    }

    std::array<double, 7> slopeError;
    if (!xml::paramSlopeError(node, &slopeError)) {
        return nullptr;
    }

    Material mat;
    if (!xml::paramMaterial(node, &mat)) {
        mat = Material::Cu;  // default to copper
    }

    return std::make_shared<Cylinder>(
        name, geometricalShape, mRadius,
        static_cast<CylinderDirection>(mBendingRadius), width, height,
        degToRad(azimuthalAngle), position, orientation, incidenceAngle,
        mEntrance, mExit, slopeError, mat);
}

CylinderDirection Cylinder::getDirection() const { return m_direction; }
double Cylinder::getRadius() const { return m_radius; }
double Cylinder::getIncidenceAngle() const { return m_incidence; }
double Cylinder::getExitArmLength() const { return m_exitArmLength; }
double Cylinder::getEntranceArmLength() const { return m_entranceArmLength; }
}  // namespace RAYX
