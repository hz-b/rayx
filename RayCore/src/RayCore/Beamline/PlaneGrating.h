#ifndef PLANEGRATING_H
#define PLANEGRATING_H
#include "Quadric.h"

namespace RAY
{

    class RAY_API PlaneGrating : public Quadric {

    public:
        
        PlaneGrating(int mount, double width, double height, double deviation, double normalIncidence, double azimuthal, double distanceToPreceedingElement, double designEnergyMounting, double lineDensity, double orderOfDiffraction, std::vector<double> misalignmentParams);
        PlaneGrating();
        ~PlaneGrating();

        void calcAlpha(double deviation, double normalIncidence);
        void focus(double angle);

        double getWidth();
        double getHeight();
        
    private:
        double m_totalWidth;
        double m_totalHeight;
        // angles in rad and normal angles (measured from normal! not incidence!!)
        double m_alpha;
        double m_beta;
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

#endif