#include "SphereMirror.h"

namespace RAYX
{

    /**
     * angles given in degree and stored in rad
     * initializes transformation matrices, and parameters for the quadric in super class (quadric)
     * sets mirror-specific parameters in this class
     * @params:
     *          width, height = total width, height of the mirror (x- and z- dimensions)
     *          grazingIncidence = desired incidence angle of the main ray
     *          azimuthal = rotation of mirror around z-axis
     *          distanceToPreceedingElement
     *
    */
    SphereMirror::SphereMirror(const char* name, const double width, const double height, const double grazingIncidence, const double azimuthal, const double distanceToPreceedingElement, const double entranceArmLength, const double exitArmLength, const std::vector<double> misalignmentParams, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous)
        : OpticalElement(name, { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, width, height, rad(grazingIncidence), rad(azimuthal), rad(grazingIncidence), distanceToPreceedingElement, misalignmentParams, { 0,0,0,0,0,0 }, slopeError, previous),
        m_totalWidth(width),
        m_totalHeight(height),
        m_entranceArmLength(entranceArmLength),
        m_exitArmLength(exitArmLength),
        m_alpha(rad(grazingIncidence)),
        m_beta(m_alpha), // mirror -> exit angle = incidence angle
        m_chi(rad(azimuthal)),
        m_distanceToPreceedingElement(distanceToPreceedingElement)
    {
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};


        calcRadius(); // calculate the radius
        setSurface(std::make_unique<Quadric>(std::vector<double>{1, 0, 0, 0, 0, 1, 0, -m_radius, 0, 0, 1, 0, 0, 0, 0, 0}));
    }

    SphereMirror::~SphereMirror()
    {
    }

    void SphereMirror::calcRadius() {
        double theta = m_alpha; // grazing incidence in rad
        m_radius = 2.0 / sin(theta) / (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength);
    }


    double SphereMirror::getWidth() const {
        return m_totalWidth;
    }

    double SphereMirror::getHeight() const {
        return m_totalHeight;
    }

    double SphereMirror::getRadius() const {
        return m_radius;
    }

    double SphereMirror::getBeta() const {
        return m_beta;
    }

    double SphereMirror::getAlpha() const {
        return m_alpha;
    }

    double SphereMirror::getChi() const {
        return m_chi;
    }
    double SphereMirror::getDist() const {
        return m_distanceToPreceedingElement;
    }

    double SphereMirror::getExitArmLength() const {
        return m_exitArmLength;
    }

    double SphereMirror::getEntranceArmLength() const {
        return m_entranceArmLength;
    }
}
