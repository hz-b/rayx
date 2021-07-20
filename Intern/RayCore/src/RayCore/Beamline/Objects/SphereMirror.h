#pragma once
#include "Surface/Quadric.h"
#include "Beamline/OpticalElement.h"

namespace RAYX
{

    class RAYX_API SphereMirror : public OpticalElement {

    public:

        SphereMirror(const char* name, const double width, const double height, const double grazingIncidence, const double azimuthal, const double distanceToPreceedingElement, const double entranceArmLength, const double exitArmLength, const std::vector<double> misalignmentParams, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous);
        SphereMirror();
        ~SphereMirror();

        void calcRadius();
        double getWidth() const;
        double getHeight() const;
        double getRadius() const;
        double getBeta() const;
        double getAlpha() const;
        double getChi() const;
        double getDist() const;
        double getEntranceArmLength() const;
        double getExitArmLength() const;

    private:
        double m_totalWidth;
        double m_totalHeight;
        double m_radius;
        double m_entranceArmLength;
        double m_exitArmLength;
        // grazing incidence, in rad
        double m_alpha;
        double m_beta;
        double m_chi;
        double m_distanceToPreceedingElement;
        //double m_Depth;
        //double m_verDivergence;
        //double m_horDivergence;

    };

} // namespace RAYX