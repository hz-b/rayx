#include "PlaneGrating.h"

namespace RAYX {

/**
 * constructor that assumes that incidence and exit angle are already calculated
 * (geometricUserParams.cpp) and the position and orientation has been derived
 * from them already (WorldUserParameters.cpp)
 *
 * @param name                  name of element
 * @param geometricalShape      0/1 rectangle/elliptical
 * @param width                 width of element (x dimension)
 * @param height                height of element (z dimension)
 * @param position              position in world coordinate system
 * @param orientation           orientation(rotation) of element in world
 * coordinate system
 * @param designEnergy          the energy for which the grating is designed.
 * design wavelength can be derived from this
 * @param lineDensity           line density of grating
 * @param orderOfDiffraction    the order in which the grating should refract
 * the ray
 * @param additionalZeroOrder   if true half of the rays will be refracted in
 * the 0th order (=reflection), if false all will be refracted according to
 * orderOfDiffraction Parameter
 * @param vls                   vls grating paramters (6) (variable line
 * spacing)
 * @param slopeError            7 slope error parameters: x-y sagittal (0), y-z
 * meridional (1), thermal distortion: x (2),y (3),z (4), cylindrical bowing
 * amplitude y(5) and radius (6)
 *
 */
PlaneGrating::PlaneGrating(const char* name,
                           Geometry::GeometricalShape geometricalShape,
                           const double width, const double height,
                           glm::dvec4 position, glm::dmat4x4 orientation,
                           const double designEnergy, const double lineDensity,
                           const double orderOfDiffraction,
                           const int additionalZeroOrder,
                           const std::vector<double> vls,
                           const std::vector<double> slopeError)
    : OpticalElement(name, geometricalShape, width, height, position,
                     orientation, slopeError),
      m_additionalOrder(additionalZeroOrder),
      m_designEnergyMounting(designEnergy),
      m_lineDensity(lineDensity),
      m_orderOfDiffraction(orderOfDiffraction),
      m_vls(vls) {
    std::cout << "[PlaneGrating]: design wavelength = "
              << abs(hvlam(m_designEnergyMounting)) << std::endl;

    // set element specific parameters in Optical Element class. will be moved
    // to shader and are needed for tracing
    setElementParameters({0, 0, m_lineDensity, m_orderOfDiffraction,
                          abs(hvlam(m_designEnergyMounting)), 0, m_vls[0],
                          m_vls[1], m_vls[2], m_vls[3], m_vls[4], m_vls[5], 0,
                          0, 0, double(m_additionalOrder)});

    // parameters of quadric surface
    setSurface(std::make_unique<Quadric>(
        std::vector<double>{0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0}));
}

PlaneGrating::~PlaneGrating() {}

std::shared_ptr<PlaneGrating> PlaneGrating::createFromXML(
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
    if (!xml::paramPosition(node, group_context, &position)) {
        return nullptr;
    }

    glm::dmat4x4 orientation;
    if (!xml::paramOrientation(node, group_context, &orientation)) {
        return nullptr;
    }

    double designEnergy;
    if (!xml::paramDouble(node, "designEnergyMounting", &designEnergy)) {
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

    double additionalZeroOrder = 0;
    xml::paramDouble(node, "additionalOrder",
                     &additionalZeroOrder);  // may be missing in some RML
                                             // files, that's fine though

    std::vector<double> vls;
    if (!xml::paramVls(node, &vls)) {
        return nullptr;
    }

    std::vector<double> slopeError;
    if (!xml::paramSlopeError(node, &slopeError)) {
        return nullptr;
    }

    return std::make_shared<PlaneGrating>(name, geometricalShape, width, height,
                                          position, orientation, designEnergy,
                                          lineDensity, orderOfDiffraction,
                                          additionalZeroOrder, vls, slopeError);
}

double PlaneGrating::getDesignEnergyMounting() {
    return m_designEnergyMounting;
}
double PlaneGrating::getLineDensity() { return m_lineDensity; }
double PlaneGrating::getOrderOfDiffraction() { return m_orderOfDiffraction; }
std::vector<double> PlaneGrating::getVls() { return m_vls; }
}  // namespace RAYX
