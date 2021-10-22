#pragma once
#include "Model/Surface/Quadric.h"
#include "Model/Beamline/OpticalElement.h"

namespace RAYX
{

    class RAYX_API SphereGrating : public OpticalElement {

    public:

        // new, shortened constructor
        SphereGrating(const char* name, int mount, int geometricalShape, double width, double height, double radius, glm::dvec4 position, glm::dmat4x4 orientation, double designEnergyMounting, double lineDensity, double orderOfDiffraction, std::vector<double> vls, std::vector<double> slopeError);
        // old constructor
        // SphereGrating(const char* name, int mount, int geometricalShape, double width, double height, double deviation, double normalIncidence, double azimuthal, double distanceToPreceedingElement, double entranceArmLength, double exitArmLength, double designEnergyMounting, double lineDensity, double orderOfDiffraction, std::vector<double> misalignmentParams, std::vector<double> vls, std::vector<double> slopeError, std::shared_ptr<OpticalElement> previous, bool global);
        SphereGrating();
        ~SphereGrating();

        void calcRadius();
        void calcAlpha(double deviation, double normalIncidence);
        void focus(double angle);

        double getRadius() const;
        double getExitArmLength() const;
        double getEntranceArmLength() const;
        double getDeviation() const; // not always calculated
        int getGratingMount() const;
        double getDesignEnergyMounting() const;
        double getLineDensity() const;
        double getOrderOfDiffraction() const;
        double getA() const;
        std::vector<double> getVls() const;

    private:

        double m_radius;
        double m_entranceArmLength;
        double m_exitArmLength;
        double m_deviation; // not always calculated
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

} // namespace RAYX
