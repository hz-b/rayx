#pragma once
#include "Quadric.h"

namespace RAY
{

    class RAY_API SphereGrating : public Quadric {

    public:
        
        SphereGrating(int mount, double width, double height, double deviation, double normalIncidence, double azimuthal, double distanceToPreceedingElement, double entranceArmLength, double exitArmLength, double designEnergyMounting, double lineDensity, double orderOfDiffraction, std::vector<double> misalignmentParams);
        SphereGrating();
        ~SphereGrating();

        void calcRadius();
        void calcAlpha(double deviation, double normalIncidence);
        void focus(double angle);

        double getWidth();
        double getHeight();
        double getRadius();
        
    private:
        double m_totalWidth;
        double m_totalHeight;
        
        double m_radius;
        double m_exitArmLength;
        double m_entranceArmLength;
        // angles in rad and normal angles (measured from normal! not incidence!!)
        double m_alpha;
        double m_beta;
        double m_deviation; // not always calculated
        // in rad as well
        double m_chi;
        double m_distanceToPreceedingElement;
        enum GRATING_MOUNT  { GM_DEVIATION, GM_INCIDENCE};
        GRATING_MOUNT m_gratingMount;
        double m_designEnergyMounting;
        double m_lineDensity;
        double m_orderOfDiffraction;
        double m_a; // calculated from line density, order of diffracion and design energy mounting
        //double m_Depth;
        //double m_verDivergence;
        //double m_horDivergence;
        
    };

} // namespace RAY