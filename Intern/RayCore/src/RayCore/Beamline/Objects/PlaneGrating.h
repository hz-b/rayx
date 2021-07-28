#pragma once
#include "Surface/Quadric.h"
#include "Beamline/OpticalElement.h"

namespace RAYX
{

    class RAYX_API PlaneGrating : public OpticalElement {

    public:

        PlaneGrating(const char* name, const int mount, const double width, const double height, const double deviation, const double normalIncidence, const double azimuthal, const double distanceToPreceedingElement, const double designEnergyMounting, const double lineDensity, const double orderOfDiffraction, const double fixFocusConstantCFF, const int additional_zero_order, const std::vector<double> misalignmentParams, const std::vector<double> vls, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous, bool global);
        PlaneGrating();
        ~PlaneGrating();

        void calcAlpha(const double deviation, const double normalIncidence);
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
        enum GRATING_MOUNT { GM_DEVIATION, GM_INCIDENCE, GM_CCF, GM_CCF_NO_PREMIRROR };
        GRATING_MOUNT m_gratingMount;
        double m_fixFocusConstantCFF;
        enum ADDITIONAL_ZERO_ORDER { AO_OFF, AO_ON };
        ADDITIONAL_ZERO_ORDER m_additionalOrder;

        double m_designEnergyMounting;
        double m_lineDensity;
        double m_orderOfDiffraction;
        double m_a; // calculated from line density, order of diffracion and design energy mounting
        std::vector<double> m_vls;

    };

} // namespace RAYX
