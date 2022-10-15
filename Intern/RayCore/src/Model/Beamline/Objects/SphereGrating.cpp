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
SphereGrating::SphereGrating(const char* name, GratingMount mount, GeometricalShape geometricalShape, double width, double height,
                             const double azimuthalAngle, double radius, glm::dvec4 position, glm::dmat4x4 orientation,
                             double designEnergyMounting, double lineDensity, double orderOfDiffraction, std::array<double, 6> vls,
                             std::array<double, 7> slopeError, Material mat)
    : OpticalElement(name, slopeError),
      m_designEnergyMounting(designEnergyMounting),
      m_lineDensity(lineDensity),
      m_orderOfDiffraction(orderOfDiffraction),
      m_vls(vls) {
    // set geometry
    m_Geometry->m_geometricalShape = geometricalShape;
    m_Geometry->setHeightWidth(height, width);
    m_Geometry->m_azimuthalAngle = azimuthalAngle;
    m_Geometry->m_position = position;
    m_Geometry->m_orientation = orientation;

    double icurv = 1;
    m_gratingMount = mount;
    auto matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Quadric>(glm::dmat4x4{1, 0, 0, 0, icurv, 1, 0, -radius, 0, 0, 1, 0, 2, 0, matd, 0}));
}

std::shared_ptr<SphereGrating> SphereGrating::createFromXML(const xml::Parser& p) {
    return std::make_shared<SphereGrating>(p.name(), p.parseGratingMount(), p.parseGeometricalShape(), p.parseTotalWidth(),
                                           p.parseTotalLength(), p.parseAzimuthalAngle(), p.parseRadius(), p.parsePosition(),
                                           p.parseOrientation(), p.parseDesignEnergy(), p.parseLineDensity(), p.parseOrderDiffraction(),
                                           p.parseVls(), p.parseSlopeError(), p.parseMaterial());
}

double SphereGrating::getRadius() const { return m_radius; }

double SphereGrating::getExitArmLength() const { return m_exitArmLength; }
double SphereGrating::getEntranceArmLength() const { return m_entranceArmLength; }

double SphereGrating::getDeviation() const { return m_deviation; }
GratingMount SphereGrating::getGratingMount() const { return m_gratingMount; }
double SphereGrating::getDesignEnergyMounting() const { return m_designEnergyMounting; }
double SphereGrating::getLineDensity() const { return m_lineDensity; }
double SphereGrating::getOrderOfDiffraction() const { return m_orderOfDiffraction; }
double SphereGrating::getA() const { return m_a; }

glm::dmat4x4 SphereGrating::getElementParameters() const {
    return {0,
            0,
            m_lineDensity,
            m_orderOfDiffraction,
            abs(hvlam(m_designEnergyMounting)),
            0,
            m_vls[0],
            m_vls[1],
            m_vls[2],
            m_vls[3],
            m_vls[4],
            m_vls[5],
            0,
            0,
            0,
            0};
}

}  // namespace RAYX