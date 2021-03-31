#ifndef PLANEGRATING_H
#define PLANEGRATING_H
#include "Quadric.h"

namespace RAY
{

    class RAY_API PlaneGrating : public Quadric {

    public:
        
        PlaneGrating(const char* name, int mount, double width, double height, double deviation, double normalIncidence, double azimuthal, double distanceToPreceedingElement, double designEnergyMounting, double lineDensity, double orderOfDiffraction, double fixFocusConstantCFF, std::vector<double> misalignmentParams, std::vector<double> vls);
        PlaneGrating();
        ~PlaneGrating();

        void calcAlpha(double deviation, double normalIncidence);
        void focus(double angle);

        double getWidth();
        double getHeight();
        double getAlpha();
        double getBeta();
        // in rad as well
        double getChi();
        double getDistanceToPreceedingElement();
        int getGratingMount();
        double getFixFocusConstantCFF();

        double getDesignEnergyMounting();
        double getLineDensity();
        double getOrderOfDiffraction();
        double getA(); // calculated from line density, order of diffracion and design energy mounting
        std::vector<double> getVls();
        
    private:
        double m_totalWidth;
        double m_totalHeight;
        // angles in rad and normal angles (measured from normal! not incidence!!)
        double m_alpha;
        double m_beta;
        // in rad as well
        double m_chi;
        double m_distanceToPreceedingElement;
        enum GRATING_MOUNT  { GM_DEVIATION, GM_INCIDENCE, GM_CCF, GM_CCF_NO_PREMIRROR};
        GRATING_MOUNT m_gratingMount;
        double m_fixFocusConstantCFF;

        double m_designEnergyMounting;
        double m_lineDensity;
        double m_orderOfDiffraction;
        double m_a; // calculated from line density, order of diffracion and design energy mounting
        std::vector<double> m_vls;
        
    };

} // namespace RAY

#endif
