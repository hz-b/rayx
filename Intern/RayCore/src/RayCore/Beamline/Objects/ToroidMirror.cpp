#include "ToroidMirror.h"

namespace RAYX
{

    /**
     * Initializes transformation matrices, and parameters for the toroid in super class (optical element).
     * Sets mirror-specific parameters in this class.
     *
     * @param name                      name of the element
     * @param geometricalShape          either rectangular or elliptical
     * @param width                     width of the element
     * @param height                    height of the element
     * @param grazingIncidenceAngle     angle in which the main ray should hit the element, used to calculate the radii, in rad
     * @param position                  position of element in world coordinate system
     * @param orientation               orientation of element in world coordinate system
     * @param mEntrance                 meridional entrance arm length, used to calculate the radii
     * @param mExit                     meridional exit arm length, used to calculate the radii
     * @param sEntrance                 sagittal entrance arm length, used to calculate the radii
     * @param sExit                     sagittal exit arm length, used to calculate the radii
     * @param slopeError                7 slope error parameters: x-y sagittal (0), y-z meridional (1), thermal distortion x (2),y (3),z (4), cylindrical bowing amplitude y(5) and radius (6)
    */
    ToroidMirror::ToroidMirror(const char* name, const int geometricalShape, const double width, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const double incidenceAngle, const double mEntrance, const double mExit, const double sEntrance, const double sExit, const std::vector<double> slopeError)
        : OpticalElement(name, { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, geometricalShape, width, height, position, orientation, { 0,0,0, 0,0,0 }, slopeError),
        m_sagittalEntranceArmLength(sEntrance),
        m_sagittalExitArmLength(sExit),
        m_meridionalEntranceArmLength(mEntrance),
        m_meridionalExitArmLength(mExit)
    {
        m_geometricalShape = geometricalShape == 0 ? GS_RECTANGLE : GS_ELLIPTICAL;
        if (m_geometricalShape == GS_ELLIPTICAL) {
            setDimensions(-width, -height);
        }
        // TODO(Theresa): maybe move this function outside of this class (same for spheres) because this is derived from user parameters
        calcRadius(incidenceAngle); // calculate the radius

        std::cout << "long Radius: " << m_longRadius << ", short Radius: " << m_shortRadius << std::endl;
        setSurface(std::make_unique<Toroid>(std::vector<double>{m_longRadius, m_shortRadius, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 6, 0, 0, 0}));
        // setSurface(std::make_unique<Toroid>(m_longRadius, m_shortRadius));
    }

    /**
     * Angles given in degree and stored in rad.
     * Initializes transformation matrices, and parameters for the quadric in super class (quadric).
     * Sets mirror-specific parameters in this class.
     *
     * Params:
     * width, height = total width, height of the mirror (x- and z- dimensions)
     * grazingIncidence = desired incidence angle of the main ray
     * azimuthal = rotation of mirror around z-axis
     * distanceToPreceedingElement
     *
    */
    ToroidMirror::ToroidMirror(const char* name, const int geometricalShape, const double width, const double height, const double grazingIncidence, const double azimuthal, const double distanceToPreceedingElement, const double mEntrance, const double mExit, const double sEntrance, const double sExit, const std::vector<double> misalignmentParams, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous, bool global)
        : OpticalElement(name, { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, geometricalShape, width, height, degToRad(grazingIncidence), degToRad(azimuthal), degToRad(grazingIncidence), distanceToPreceedingElement, misalignmentParams, { 0,0,0,0,0,0 }, slopeError, previous, global),
        m_sagittalEntranceArmLength(sEntrance),
        m_sagittalExitArmLength(sExit),
        m_meridionalEntranceArmLength(mEntrance),
        m_meridionalExitArmLength(mExit)
    {
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};

        m_geometricalShape = geometricalShape == 0 ? GS_RECTANGLE : GS_ELLIPTICAL;
        if (m_geometricalShape == GS_ELLIPTICAL) {
            setDimensions(-width, -height);
        }
        calcRadius(degToRad(grazingIncidence)); // calculate the radius
        setSurface(std::make_unique<Toroid>(std::vector<double>{m_longRadius, m_shortRadius, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 6, 0, 0, 0}));
    }

    ToroidMirror::~ToroidMirror()
    {
    }

    /**
     * calculate long and short radius from grazing incidence angle and meridional and sagittal entrance and exit arm lengths
     */
    void ToroidMirror::calcRadius(double incidenceAngle) {
        m_longRadius = 2.0 / sin(incidenceAngle) / (1.0 / m_meridionalEntranceArmLength + 1.0 / m_meridionalExitArmLength);

        if (m_meridionalEntranceArmLength == 0.0 || m_meridionalExitArmLength == 0.0 || incidenceAngle == 0.0) {
            m_shortRadius = 0.0;
        }
        else {
            m_shortRadius = 2.0 * sin(incidenceAngle) / (1.0 / m_sagittalEntranceArmLength + 1.0 / m_sagittalExitArmLength);
        }
    }

    double ToroidMirror::getSagittalEntranceArmLength() const {
        return m_sagittalEntranceArmLength;
    }

    double ToroidMirror::getSagittalExitArmLength() const {
        return m_sagittalExitArmLength;
    }

    double ToroidMirror::getMeridionalEntranceArmLength() const {
        return m_meridionalEntranceArmLength;
    }

    double ToroidMirror::getMeridionalExitArmLength() const {
        return m_meridionalExitArmLength;
    }
}
