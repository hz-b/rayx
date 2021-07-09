#pragma once
#include "Surface/Quadric.h"
#include "OpticalElement.h"

namespace RAY
{

    class RAY_API SphereGrating : public OpticalElement {

    public:

        SphereGrating(const char* name, int mount, double width, double height, double deviation, double normalIncidence, double azimuthal, double distanceToPreceedingElement, double entranceArmLength, double exitArmLength, double designEnergyMounting, double lineDensity, double orderOfDiffraction, std::vector<double> misalignmentParams, std::vector<double> vls, std::vector<double> slopeError, std::shared_ptr<OpticalElement> previous);
        SphereGrating();
        ~SphereGrating();

        void calcRadius();
        void calcAlpha(double deviation, double normalIncidence);
        void focus(double angle);

        double getWidth() const;
        double getHeight() const;
        double getRadius() const;
        double getExitArmLength() const;
        double getEntranceArmLength() const;
        // angles in rad and normal angles (measured from normal! not incidence!!)
        double getAlpha() const;
        double getBeta() const;
        double getDeviation() const; // not always calculated
        // in rad as well
        double getChi() const;
        double getDistanceToPreceedingElement() const;
        int getGratingMount() const;
        double getDesignEnergyMounting() const;
        double getLineDensity() const;
        double getOrderOfDiffraction() const;
        double getA() const;
        std::vector<double> getVls() const;

    private:
        double m_totalWidth;
        double m_totalHeight;

        double m_radius;
        double m_entranceArmLength;
        double m_exitArmLength;
        // angles in rad and normal angles (measured from normal! not incidence!!)
        double m_alpha;
        double m_beta;
        double m_deviation; // not always calculated
        // in rad as well
        double m_chi;
        double m_distanceToPreceedingElement;
        enum GRATING_MOUNT { GM_DEVIATION, GM_INCIDENCE };
        GRATING_MOUNT m_gratingMount;
        double m_designEnergyMounting;
        double m_lineDensity;
        double m_orderOfDiffraction;
        double m_a; // calculated from line density, order of diffracion and design energy mounting
        std::vector<double> m_vls;
        //double m_Depth;
        //double m_verDivergence;
        //double m_horDivergence;

    };

} // namespace RAY
