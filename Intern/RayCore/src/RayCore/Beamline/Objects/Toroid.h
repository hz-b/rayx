#pragma once
#include "Surface/Quadric.h"
#include "Beamline/OpticalElement.h"

namespace RAYX
{

    class RAYX_API Toroid : public OpticalElement {

    public:

        Toroid(const char* name, const int geometricalShape, const double width, const double height, const double grazingIncidence, const double azimuthal, const double distanceToPreceedingElement, const double mEntrance, const double mExit, const double sEntrance, const double sExit, const std::vector<double> misalignmentParams, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous, bool global);
        Toroid();
        ~Toroid();

        void calcRadius();
        double getWidth() const;
        double getHeight() const;
        double getRadius() const;
        double getBeta() const;
        double getAlpha() const;
        double getChi() const;
        double getDist() const;
        double getSagittalEntranceArmLength() const;
        double getSagittalExitArmLength() const;
        double getMeridionalEntranceArmLength() const;
        double getMeridionalExitArmLength() const;

    private:
        double m_totalWidth;
        double m_totalHeight;
        double m_sagittalEntranceArmLength;
        double m_sagittalExitArmLength;
        double m_meridionalEntranceArmLength;
        double m_meridionalExitArmLength;
        double m_longRadius;
        double m_shortRadius;
        enum GEOMETRICAL_SHAPE {GS_RECTANGLE, GS_ELLIPTICAL};
        GEOMETRICAL_SHAPE m_geometricalShape;
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