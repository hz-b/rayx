#include "Cylinder.h"

namespace RAYX {
/**
 * @brief Construct a new Cylinder object
 *
 * @param name              Cylinder name
 * @param geometricalShape
 * @param radius            Cylinder radius
 * @param direction         Direction of Cylinder
 * @param width             width of the mirror (x-dimension)
 * @param height            height of the mirror (z-dimension)
 * @param position          position of the element in world coordinates
 * @param orientation       orientation of the element in world coordinates
 * @param grazingIncidence  desired incidence angle of the main ray
 * @param entranceArmLength length of entrance arm
 * @param exitArmLength     length of exit arm
 * @param slopeError        7 slope error parameters: x-y sagittal (0), y-z
 * meridional (1), thermal distortion: x (2),y (3),z (4), cylindrical bowing
 * amplitude y(5) and radius (6)
 */
Cylinder::Cylinder(const char* name,
                   Geometry::GeometricalShape geometricalShape,
                   const double radius, const int direction, const double width,
                   const double height, glm::dvec4 position,
                   glm::dmat4x4 orientation, const double grazingIncidence,
                   const double entranceArmLength, const double exitArmLength,
                   const std::vector<double> slopeError)
    : OpticalElement(name, geometricalShape, width, height, position,
                     orientation, slopeError),
      m_direction(CYLINDER_DIRECTION(direction)),
      m_radius(radius),
      m_incidence(degToRad(grazingIncidence)),
      m_entranceArmLength(entranceArmLength),
      m_exitArmLength(exitArmLength) {
    std::cout << name << std::endl;
    if (m_direction == X_DIR) {  // y² + z² = R²
        m_a11 = 0;
        m_a33 = 1;
        m_a24 = -radius;  // TODO (OS): always -raduis?
    } else {              // Z_DIR y² + z² = R²
        m_a11 = 1;
        m_a33 = 0;
        m_a24 = radius;
    }
    double icurv = 1;
    setSurface(std::make_unique<Quadric>(std::vector<double>{
        m_a11, 0, 0, 0, icurv, 1, 0, m_a24, 0, 0, m_a33, 0, 0, 0, 0, 0}));
    // TODO (OS): Add Element parameters?
    setElementParameters({radius, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
}
Cylinder::~Cylinder() {}

Cylinder::CYLINDER_DIRECTION Cylinder::getDirection() { return m_direction; }
double Cylinder::getRadius() { return m_radius; }
}  // namespace RAYX