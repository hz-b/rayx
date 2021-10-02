#pragma once
#include "Surface/Quadric.h"
#include "Beamline/OpticalElement.h"

namespace RAYX
{

    class RAYX_API PlaneGrating : public OpticalElement {

    public:

        // old constructor with several user parameters to calculate incidence and exit angle (alpha, beta) has been moved to somewhere else
        PlaneGrating(const char* name, const int mount, const double width, const double height, const double deviation, const double normalIncidence, const double azimuthal, const double distanceToPreceedingElement, const double designEnergyMounting, const double lineDensity, const double orderOfDiffraction, const double fixFocusConstantCFF, const int additional_zero_order, const std::vector<double> misalignmentParams, const std::vector<double> vls, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous, bool global);
        PlaneGrating(const char* name, const double width, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const double designEnergyMounting, const double lineDensity, const double orderOfDiffraction, const int additional_zero_order, const std::vector<double> vls, const std::vector<double> slopeError);
        PlaneGrating();
        ~PlaneGrating();

        // functions used to derive incidence and exit angle from user parameters, will be moved to somewhere else
        void calcAlpha(const double deviation, const double normalIncidence);
        void focus(double angle);
        int getGratingMount();
        double getFixFocusConstantCFF();

        double getDesignEnergyMounting();
        double getLineDensity();
        double getOrderOfDiffraction();
        std::vector<double> getVls();

    private:
        
        int m_additionalOrder;
        double m_designEnergyMounting;
        double m_lineDensity;
        double m_orderOfDiffraction;
        std::vector<double> m_vls;

        enum GRATING_MOUNT {GM_DEVIATION, GM_INCIDENCE};
        GRATING_MOUNT m_gratingMount;
    };

} // namespace RAYX
