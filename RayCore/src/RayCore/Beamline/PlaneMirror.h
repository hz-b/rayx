#pragma once
#include "Quadric.h"

namespace RAY
{

    class RAY_API PlaneMirror : public Quadric {

    public:
        
        PlaneMirror(const char* name, double width, double height, double grazingIncidence, double azimuthal, double distanceToPreceedingElement, std::vector<double> misalignmentParams);
        PlaneMirror();
        ~PlaneMirror();

        double getWidth();
        double getHeight();
        double getAlpha();
        double getBeta();
        double getChi();
        double getDist();
        
    private:
        double m_totalWidth;
        double m_totalHeight;
        // grazing incidence, in rad
        double m_alpha;
        double m_beta;
        double m_chi;
        double m_distanceToPreceedingElement;
        //double m_Depth;
        //double m_verDivergence;
        //double m_horDivergence;
        
    };

} // namespace RAY