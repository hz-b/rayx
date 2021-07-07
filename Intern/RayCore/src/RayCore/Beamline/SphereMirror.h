#pragma once
#include "Quadric.h"

namespace RAY
{

    class RAY_API SphereMirror : public Quadric {

    public:
        
        SphereMirror(const char* name, const double width, const double height, const double grazingIncidence, const double azimuthal, const double distanceToPreceedingElement, const double entranceArmLength, const double exitArmLength, const std::vector<double> misalignmentParams, const std::vector<double> slopeError, const Quadric* const previous);
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
        double m_exitArmLength;
        double m_entranceArmLength;
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