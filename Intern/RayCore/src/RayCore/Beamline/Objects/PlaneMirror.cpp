#include "PlaneMirror.h"

namespace RAYX
{

    /**
     * angles given in degree and stored in rad
     * initializes transformation matrices, and parameters for the quadric in super class (quadric)
     * sets mirror-specific parameters in this class
     * @param width              width of mirror (x-dimension in element coord. sys.)
     * @param height             height of mirror (z-dimension in element coord. sys.)
     * @param grazingIncidence   desired incidence angle of the main ray
     * @param azimuthal          rotation of mirror around z-axis
     * @param dist               distance to preceeding element
     * @param slopeError         7 slope error parameters: x-y sagittal (0), y-z meridional (1), thermal distortion x (2),y (3),z (4), cylindrical bowing amplitude y(5) and radius (6)
     * @param misalignmentParams angles and distances for the mirror's misalignment
     *
    */
    PlaneMirror::PlaneMirror(const char* name, const double width, const double height, const double grazingIncidence, const double azimuthal, const double dist, const std::vector<double> misalignmentParams, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous)
        : OpticalElement(name, { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, width, height, rad(grazingIncidence), rad(azimuthal), rad(grazingIncidence), dist, misalignmentParams, { 0,0,0,0,0,0 }, slopeError, previous),
        m_totalWidth(width), m_totalHeight(height), m_alpha(rad(grazingIncidence)), m_beta(m_alpha), m_chi(rad(azimuthal)), m_distanceToPreceedingElement(dist) {
        setSurface(std::make_unique<Quadric>(std::vector<double>{0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0}));
    }

    PlaneMirror::~PlaneMirror()
    {
    }

    double PlaneMirror::getWidth() {
        return m_totalWidth;
    }

    double PlaneMirror::getHeight() {
        return m_totalHeight;
    }

    double PlaneMirror::getAlpha() {
        return m_alpha;
    }

    double PlaneMirror::getBeta() {
        return m_beta;
    }

    double PlaneMirror::getChi() {
        return m_chi;
    }

    double PlaneMirror::getDist() {
        return m_distanceToPreceedingElement;
    }

}
