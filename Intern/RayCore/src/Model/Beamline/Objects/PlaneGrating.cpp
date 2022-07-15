#include "PlaneGrating.h"

#include "Debug.h"

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
 * @param azimuthalAngle        rotation of element in xy-plane, needed for
 * stokes vector. in rad
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
 * @param mat                   material (See Material.h)
 *
 */
PlaneGrating::PlaneGrating(
    const char* name, OpticalElement::GeometricalShape geometricalShape,
    const double width, const double height, const double azimuthalAngle,
    glm::dvec4 position, glm::dmat4x4 orientation, const double designEnergy,
    const double lineDensity, const double orderOfDiffraction,
    const int additionalZeroOrder, const std::array<double, 6> vls,
    const std::array<double, 7> slopeError, Material mat)
    : OpticalElement(name, slopeError),
      m_additionalOrder(additionalZeroOrder),
      m_designEnergyMounting(designEnergy),
      m_lineDensity(lineDensity),
      m_orderOfDiffraction(orderOfDiffraction),
      m_vls(vls) {
    // set geometry
    m_Geometry->m_geometricalShape = geometricalShape;
    m_Geometry->setHeightWidth(height, width);
    m_Geometry->m_azimuthalAngle = azimuthalAngle;
    m_Geometry->m_position = position;
    m_Geometry->m_orientation = orientation;
    updateObjectParams();
    
    RAYX_LOG << "design wavelength = " << abs(hvlam(m_designEnergyMounting));

    // set element specific parameters in Optical Element class. will be moved
    // to shader and are needed for tracing
    setElementParameters({0, 0, m_lineDensity, m_orderOfDiffraction,
                          abs(hvlam(m_designEnergyMounting)), 0, m_vls[0],
                          m_vls[1], m_vls[2], m_vls[3], m_vls[4], m_vls[5], 0,
                          0, 0, double(m_additionalOrder)});

    // parameters of quadric surface
    double matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Quadric>(std::array<double, 4 * 4>{
        0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 1, 0, matd, 0}));
}

std::shared_ptr<PlaneGrating> PlaneGrating::createFromXML(xml::Parser p) {
    return std::make_shared<PlaneGrating>(
        p.name(), p.parseGeometricalShape(), p.parseTotalWidth(),
        p.parseTotalLength(), p.parseAzimuthalAngle(), p.parsePosition(),
        p.parseOrientation(), p.parseDesignEnergyMounting(),
        p.parseLineDensity(), p.parseOrderDiffraction(),
        p.parseAdditionalOrder(), p.parseVls(), p.parseSlopeError(),
        p.parseMaterial());
}

double PlaneGrating::getDesignEnergyMounting() {
    return m_designEnergyMounting;
}
double PlaneGrating::getLineDensity() { return m_lineDensity; }
double PlaneGrating::getOrderOfDiffraction() { return m_orderOfDiffraction; }
std::array<double, 6> PlaneGrating::getVls() { return m_vls; }
}  // namespace RAYX
