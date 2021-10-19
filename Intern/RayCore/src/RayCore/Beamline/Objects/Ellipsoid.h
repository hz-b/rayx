#pragma once
#include "Surface/Quadric.h"
#include "Beamline/OpticalElement.h"

namespace RAYX
{

    class RAYX_API Ellipsoid : public OpticalElement {

    public:

        Ellipsoid(const char* name, const double width, const double height, const double grazingIncidence, const double azimuthal, const double distanceToPreceedingElement,
            const double entranceArmLength, const double exitArmLength, const int coordSys, const int figRot, const double a11, const std::vector<double> misalignmentParams, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous, bool global);
        Ellipsoid();
        ~Ellipsoid();

        void calcRadius();
        void calcHalfAxes();
        void calcAlphaBeta();
        double getRadius();
        double getExitArmLength();
        double getEntranceArmLength();
        double getMy0(); // center of ellipsoid
        double getMz0(); // -"-
        double getIncidenceAngle() const;
        double getShortHalfAxisB(); // b
        double getLongHalfAxisA(); // a
        double getOffsetY0(); // always = 0?
        double getAlpha1(); // from tangent angle and grazing incidence???
        // derived params
        double getTangentAngle();
        double getA34();
        double getA33();
        double getA44();
        double getHalfAxisC();

        enum FIGURE_ROTATION { FR_YES, FR_PLANE, FR_A11 };
        enum COORDINATESYSTEM_TYPE { CS_CURVATURE, CS_MIRROR };

    private:
        double m_radius;
        // grazing incidence, in rad
        double m_incidence;
        double m_entranceArmLength;
        double m_exitArmLength;
        double m_y0; // center of ellipsoid
        double m_z0; // -"-
        double m_shortHalfAxisB; // b
        double m_longHalfAxisA; // a
        double m_offsetY0; // always = 0?
        //double m_alpha1; // from tangent angle and grazing incidence???
        // derived params
        double m_tangentAngle;
        double m_a34;
        double m_a33;
        double m_a44;

        FIGURE_ROTATION m_figureRotation;
        COORDINATESYSTEM_TYPE m_misalignmentCoordSys;
        double m_a11; // param for quadric funciton, given by user

        double m_halfAxisC; // derived from figure_rotation, a_11 and half axes a, b
        //double m_Depth;
        //double m_verDivergence;
        //double m_horDivergence;

    };

} // namespace RAYX