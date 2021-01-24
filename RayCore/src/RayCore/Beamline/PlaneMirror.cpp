#include "PlaneMirror.h"

namespace RAY
{
    PlaneMirror::PlaneMirror(double width, double height, double grazingIncidence, double azimuthal, double distanceToPreceedingElement, std::vector<double> misalignmentParams) 
    : PlaneMirror(width, height, grazingIncidence, azimuthal, distanceToPreceedingElement) {
        setMisalignment(misalignmentParams);
    }
    // angles given in degree and stored in rad
    PlaneMirror::PlaneMirror(double width, double height, double grazingIncidence, double azimuthal, double distanceToPreceedingElement) 
    : Quadric({0,0,0,0, width,0,0,-1, height,0,0,0, 0,0,0,0}, grazingIncidence * PI/180, azimuthal * PI/180, grazingIncidence * PI/180, distanceToPreceedingElement) {
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};
        m_totalWidth = width;
        m_totalHeight = height;
        m_chi = azimuthal * PI/180;
        m_alpha = grazingIncidence * PI/180;
        m_beta = m_alpha;
        m_distanceToPreceedingElement = distanceToPreceedingElement;
        // Quadric(inputPoints, m_alpha, m_chi, m_beta, m_distanceToPreceedingElement);
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
