#include "PlaneMirror.h"

namespace RAY
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
     * @param misalignmentParams angles and distances for the mirror's misalignment
     * 
    */
    PlaneMirror::PlaneMirror(const char* name, double width, double height, double grazingIncidence, double azimuthal, double dist, std::vector<double> misalignmentParams) 
    : Quadric(name, {0,0,0,0, width,0,0,-1, height,0,0,0, 0,0,0,0}, {width, height,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},rad(grazingIncidence), rad(azimuthal), rad(grazingIncidence), dist, misalignmentParams, {0,0,0,0,0,0}) {
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};
        m_totalWidth = width;
        m_totalHeight = height;
        m_chi = rad(azimuthal);
        m_alpha = rad(grazingIncidence);
        m_beta = m_alpha; // mirror -> exit angle = incidence angle
        m_distanceToPreceedingElement = dist;
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
