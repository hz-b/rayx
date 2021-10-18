#include "SphereMirror.h"

namespace RAYX
{

    /**
     * Angles given in degree and stored in rad.
     * Initializes transformation matrices, and parameters for the quadric in super class (quadric).
     * Sets mirror-specific parameters in this class.
     *
     * @param name
     * @param width
     * @param height
     * @param grazingIncidence 
     * @param azimuthal
     * @param distanceToPreceedingElement
     * @param entranceArmLength
     * @param exitArmLength
     * @param misalignment
     * @param slopeError
     * @param previous
     * @param global
    */
    SphereMirror::SphereMirror(const char* name, const double width, const double height, const double grazingIncidence, const double azimuthal, const double distanceToPreceedingElement, const double entranceArmLength, const double exitArmLength, const std::vector<double> misalignmentParams, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous, bool global)
        : OpticalElement(name, { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, width, height, degToRad(grazingIncidence), degToRad(azimuthal), degToRad(grazingIncidence), distanceToPreceedingElement, misalignmentParams, { 0,0,0,0,0,0 }, slopeError, previous, global),
        m_entranceArmLength(entranceArmLength),
        m_exitArmLength(exitArmLength),
        m_grazingIncidenceAngle(degToRad(grazingIncidence))
    {
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};


        calcRadius(); // calculate the radius
        setSurface(std::make_unique<Quadric>(std::vector<double>{1,0,0,0, 1,1,0,-m_radius, 0,0,1,0, 0,0,0,0}));
    }

    /**
     * Calculates transformation matrices, and sets parameters for the quadric surface.
     * Sets mirror-specific parameters in this class.
     *
     * @param name
     * @param width
     * @param height
     * @param grazingIncidenceAngle     angle in which the main ray should hit the element. given in degree
     * @param position                  position of element in world coordinates
     * @param orientation               orientation of element in world coordinates
     * @param entranceArmLength
     * @param exitArmLength
     * @param slopeError
     *
    */
    SphereMirror::SphereMirror(const char* name, const double width, const double height, const double grazingIncidenceAngle, glm::dvec4 position, glm::dmat4x4 orientation, const double entranceArmLength, const double exitArmLength, const std::vector<double> slopeError)
        : OpticalElement(name, { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, width, height, position, orientation, { 0,0,0,0,0,0 }, slopeError),
        m_entranceArmLength(entranceArmLength),
        m_exitArmLength(exitArmLength),
        m_grazingIncidenceAngle(degToRad(grazingIncidenceAngle))

    {
        calcRadius(); // calculate the radius
        setSurface(std::make_unique<Quadric>(std::vector<double>{1,0,0,0, 1,1,0,-m_radius, 0,0,1,0, 0,0,0,0}));
    }

     /**
     * Calculates transformation matrices, and sets parameters for the quadric surface.
     * Sets mirror-specific parameters in this class.
     *
     * @param name
     * @param width
     * @param height
     * @param radius                    radius of sphere
     * @param position                  position of element in world coordinates
     * @param orientation               orientation of element in world coordinates
     * @param slopeError
    */
    SphereMirror::SphereMirror(const char* name, const double width, const double height, double radius, glm::dvec4 position, glm::dmat4x4 orientation, const std::vector<double> slopeError)
        : OpticalElement(name, { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, width, height, position, orientation, { 0,0,0,0,0,0 }, slopeError)
    {
        setSurface(std::make_unique<Quadric>(std::vector<double>{1,0,0,0, 1,1,0,-radius, 0,0,1,0, 0,0,0,0}));
    }


    SphereMirror::~SphereMirror()
    {
    }

    // TODO(Theresa): move this to user params and just give the radius as a parameter to the sphere class?
    void SphereMirror::calcRadius() {
        m_radius = 2.0 / sin(m_grazingIncidenceAngle) / (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength);
    }


    double SphereMirror::getRadius() const {
        return m_radius;
    }


    double SphereMirror::getExitArmLength() const {
        return m_exitArmLength;
    }

    double SphereMirror::getEntranceArmLength() const {
        return m_entranceArmLength;
    }
}
