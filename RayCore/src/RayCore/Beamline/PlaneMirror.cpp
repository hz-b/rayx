#include "PlaneMirror.h"

namespace RAY
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
    PlaneMirror::PlaneMirror(double width, double height, double grazingIncidence, double azimuthal, double distanceToPreceedingElement, std::vector<double> misalignmentParams) 
    : Quadric({0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0}, {width, height,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},rad(grazingIncidence), rad(azimuthal), rad(grazingIncidence), distanceToPreceedingElement) {
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};
        m_totalWidth = width;
        m_totalHeight = height;
        m_chi = rad(azimuthal);
        m_alpha = rad(grazingIncidence);
        m_beta = m_alpha; // mirror -> exit angle = incidence angle
        m_distanceToPreceedingElement = distanceToPreceedingElement;
        setMisalignment(misalignmentParams);
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
}
