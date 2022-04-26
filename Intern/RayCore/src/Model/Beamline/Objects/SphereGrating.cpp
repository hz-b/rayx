#include "SphereGrating.h"

#include "Debug.h"

namespace RAYX {

/**
 * Angles given in degree and stored in rad.
 * Initializes transformation matrices, and parameters for the quadric in super
 * class (optical element). Sets mirror-specific parameters in this class.
 *
 * Params
 * @param mount                         how angles of reflection are calculated:
 * constant deviation, constant incidence,...
 * @param width                         total width of the mirror (x dimension)
 * @param height                        total height of the mirror (z dimension)
 * @param azimuthalAngle                rotation of element in xy-plane, needed
 * for stokes vector, in rad
 * @param radius                        radius of sphere
 * @param position                      position of the element in world
 * coordinate system
 * @param orientation                   orientation of the element in world
 * coordinates
 * @param designEnergyMounting          energy, taken from source
 * @param lineDensity                   line density of the grating in lines/mm
 * @param orderOfDefraction             diffraction order that should be traced
 * @param vls                           vls grating paramters (6) (variable line
 * spacing)
 * @param slopeError                    7 slope error parameters: x-y sagittal
 * (0), y-z meridional (1), thermal distortion: x (2),y (3),z (4), cylindrical
 * bowing amplitude y(5) and radius (6)
 * @param mat                           material (See Material.h)
 *
 */
SphereGrating::SphereGrating(const char* name, GratingMount mount,
                             Geometry::GeometricalShape geometricalShape,
                             double width, double height,
                             const double azimuthalAngle, double radius,
                             glm::dvec4 position, glm::dmat4x4 orientation,
                             double designEnergyMounting, double lineDensity,
                             double orderOfDiffraction,
                             std::array<double, 6> vls,
                             std::array<double, 7> slopeError, Material mat)
    : OpticalElement(name, geometricalShape, width, height, azimuthalAngle,
                     position, orientation, slopeError),
      m_designEnergyMounting(designEnergyMounting),
      m_lineDensity(lineDensity),
      m_orderOfDiffraction(orderOfDiffraction),
      m_vls(vls) {
    RAYX_LOG << name;

    double icurv = 1;
    m_gratingMount = mount;
    double matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Quadric>(std::array<double, 4 * 4>{
        1, 0, 0, 0, icurv, 1, 0, -radius, 0, 0, 1, 0, 2, 0, matd, 0}));
    setElementParameters({0, 0, m_lineDensity, m_orderOfDiffraction,
                          abs(hvlam(m_designEnergyMounting)), 0, m_vls[0],
                          m_vls[1], m_vls[2], m_vls[3], m_vls[4], m_vls[5], 0,
                          0, 0, 0});
}

SphereGrating::~SphereGrating() {}

std::shared_ptr<SphereGrating> SphereGrating::createFromXML(
    rapidxml::xml_node<>* node, const std::vector<xml::Group>& group_context) {
    const char* name = node->first_attribute("name")->value();

    int mount_int;
    if (!xml::paramInt(node, "gratingMount", &mount_int)) {
        return nullptr;
    }
    GratingMount mount = static_cast<GratingMount>(mount_int);

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

    double radius;
    if (!xml::paramDouble(node, "radius", &radius)) {
        return nullptr;
    }

    glm::dvec4 position;
    glm::dmat4x4 orientation;
    if (!xml::paramPositionAndOrientation(node, group_context, &position,
                                          &orientation)) {
        return nullptr;
    }

    double designEnergyMounting;
    if (!xml::paramDouble(node, "designEnergy", &designEnergyMounting)) {
        return nullptr;
    }

    double lineDensity;
    if (!xml::paramDouble(node, "lineDensity", &lineDensity)) {
        return nullptr;
    }

    double orderOfDiffraction;
    if (!xml::paramDouble(node, "orderDiffraction", &orderOfDiffraction)) {
        return nullptr;
    }

    std::array<double, 6> vls;
    if (!xml::paramVls(node, &vls)) {
        return nullptr;
    }

    std::array<double, 7> slopeError;
    if (!xml::paramSlopeError(node, &slopeError)) {
        return nullptr;
    }

    double azimuthalAngle;
    if (!xml::paramDouble(node, "azimuthalAngle", &azimuthalAngle)) {
        return nullptr;
    }

    Material mat;
    if (!xml::paramMaterial(node, &mat)) {
        mat = Material::Cu;  // defaults to copper
    }

    return std::make_shared<SphereGrating>(
        name, mount, geometricalShape, width, height, degToRad(azimuthalAngle),
        radius, position, orientation, designEnergyMounting, lineDensity,
        orderOfDiffraction, vls, slopeError, mat);
}

/* TODO (Theresa): how to make radius calculation easier?
void SphereGrating::calcRadius() {
    if (m_gratingMount == GratingMount::Deviation) {
        double theta = m_deviation > 0 ? (PI - m_deviation) / 2 : PI / 2 +
m_deviation; m_radius = 2.0 / sin(theta) / (1.0 / m_entranceArmLength + 1.0 /
m_exitArmLength);
    }
    else if (m_gratingMount == GratingMount::Incidence) {
        double ca = cos(getAlpha());
        double cb = cos(getBeta());
        m_radius = (ca + cb) / ((ca * ca) / m_entranceArmLength + (cb * cb) /
m_exitArmLength);
    }
}

void SphereGrating::calcAlpha(double deviation, double normalIncidence) {
    double angle;
    if (m_gratingMount == GratingMount::Deviation) {
        angle = deviation;
    }
    else if (m_gratingMount == GratingMount::Incidence) {
        angle = -normalIncidence;
    }
    focus(angle);
}

void SphereGrating::focus(double angle) {
    // from routine "focus" in RAYX.FOR
    double theta = degToRad(abs(angle));
    double alph, bet;
    if (angle <= 0) { // constant alpha mounting
        double arg = m_a - sin(theta);
        if (abs(arg) >= 1) { // cannot calculate alpha & beta
            alph = 0;
            bet = 0;
        }
        else {
            alph = theta;
            bet = asin(arg);
        }
    }
    else {  // constant alpha & beta mounting
        theta = theta / 2;
        double arg = m_a / 2 / cos(theta);
        if (abs(arg) >= 1) {
            alph = 0;
            bet = 0;
        }
        else {
            bet = asin(arg) - theta;
            alph = 2 * theta + bet;
        }
    }
    // grazing incidence
    setAlpha((PI / 2) - alph);
    setBeta((PI / 2) - abs(bet));

}*/

double SphereGrating::getRadius() const { return m_radius; }

double SphereGrating::getExitArmLength() const { return m_exitArmLength; }
double SphereGrating::getEntranceArmLength() const {
    return m_entranceArmLength;
}

double SphereGrating::getDeviation() const { return m_deviation; }
GratingMount SphereGrating::getGratingMount() const { return m_gratingMount; }
double SphereGrating::getDesignEnergyMounting() const {
    return m_designEnergyMounting;
}
double SphereGrating::getLineDensity() const { return m_lineDensity; }
double SphereGrating::getOrderOfDiffraction() const {
    return m_orderOfDiffraction;
}
double SphereGrating::getA() const { return m_a; }

}  // namespace RAYX
