#include "Toroid.h"

namespace RAYX
{

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
    Toroid::Toroid(const char* name, const int geometricShape, const double width, const double height, const double grazingIncidence, const double azimuthal, const double distanceToPreceedingElement, const double mEntrance, const double mExit, const double sEntrance, const double sExit, const std::vector<double> misalignmentParams, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous, bool global)
        : OpticalElement(name, { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, width, height, rad(grazingIncidence), rad(azimuthal), rad(grazingIncidence), distanceToPreceedingElement, misalignmentParams, { 0,0,0,0,0,0 }, slopeError, previous, global),
        m_sagittalEntranceArmLength(sEntrance),
        m_sagittalExitArmLength(sExit),
        m_meridionalEntranceArmLength(mEntrance),
        m_meridionalExitArmLength(mExit)
    {
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};

        m_geometricalShape = geometricShape == 0 ? GS_RECTANGLE : GS_ELLIPTICAL;
        if (m_geometricalShape == GS_ELLIPTICAL) {
            setDimensions(-width, -height);
        }
        calcRadius(); // calculate the radius
        setElementParameters({ m_longRadius, m_shortRadius,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 });
        setSurface(std::make_unique<Quadric>(std::vector<double>{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 6, 0, 0, 0}));
    }

    Toroid::~Toroid()
    {
    }

    /**
     * calculate long and short radius from grazing incidence angle and meridional and sagittal entrance and exit arm lengths
     */
    void Toroid::calcRadius() {
        m_longRadius = 2.0 / sin(getAlpha()) / (1.0 / m_meridionalEntranceArmLength + 1.0 / m_meridionalExitArmLength);

        if (m_meridionalEntranceArmLength == 0.0 || m_meridionalExitArmLength == 0.0 || getAlpha() == 0.0) {
            m_shortRadius = 0.0;
        }
        else {
            m_shortRadius = 2.0 * sin(getAlpha()) / (1.0 / m_sagittalEntranceArmLength + 1.0 / m_sagittalExitArmLength);
        }
    }

    double Toroid::getSagittalEntranceArmLength() const {
        return m_sagittalEntranceArmLength;
    }

    double Toroid::getSagittalExitArmLength() const {
        return m_sagittalExitArmLength;
    }

    double Toroid::getMeridionalEntranceArmLength() const {
        return m_meridionalEntranceArmLength;
    }

    double Toroid::getMeridionalExitArmLength() const {
        return m_meridionalExitArmLength;
    }
}
