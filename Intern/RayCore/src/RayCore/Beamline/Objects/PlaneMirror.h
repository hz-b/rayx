#pragma once
#include "Surface/Quadric.h"
#include "Beamline/OpticalElement.h"

namespace RAY
{

    class RAY_API PlaneMirror : public OpticalElement {

    public:

        PlaneMirror(const char* name, const double width, const double height, const double grazingIncidence, const double azimuthal, const double distanceToPreceedingElement, const std::vector<double> misalignmentParams, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous);
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