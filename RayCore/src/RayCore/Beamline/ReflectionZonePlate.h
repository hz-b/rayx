#pragma once
#include "Quadric.h"

namespace RAY
{

    class RAY_API ReflectionZonePlate : public Quadric {

    public:

        enum GRATING_MOUNT { GM_DEVIATION, GM_INCIDENCE };
        GRATING_MOUNT m_gratingMount;
        enum CURVATURE_TYPE { CT_PLANE, CT_TOROIDAL, CT_SPHERICAL };
        CURVATURE_TYPE m_curvatureType;
        enum DESIGN_TYPE { DT_ZOFFSET, DT_BETA }; // use Fresnel Center offset or use beta angle
        DESIGN_TYPE m_designType;
        enum DERIVATION_METHOD { DM_FORMULA, DM_PLOYNOM };
        DERIVATION_METHOD m_derivationMethod;

        ReflectionZonePlate(const char* name, int mount, int curvatureType, double width, double height, double deviation, double grazingIncidence, double azimuthal, double distanceToPreceedingElement, double designEnergy, double orderOfDiffraction, std::vector<double> misalignmentParams);
        ReflectionZonePlate();
        ~ReflectionZonePlate();

        void calcAlpha(double deviation, double normalIncidence);
        double calcDz00();
        // beta and fresnelZOffset are dependant of each other, depending on the given DESIGN_TYPE one is calculated from the other in calcDz00
        void calcFresnelZOffset();
        void calcBeta();
        // calculates some more arms from the ones that are given by the user (sagittal and meridional entrance and exit arms). Somehow connected to alpha0Angle and beta0Angle
        void VectorR1Center();
        void VectorR2Center();

        void calcDesignOrderOfDiffraction(double designOrderOfDiffraction);
        void focus(double angle);

        double getWidth();
        double getHeight();

    private:
        double m_totalWidth;
        double m_totalHeight;
        // angles in rad and normal angles incidence
        double m_alpha; // grazing incidence angle
        double m_beta; // == alpha? grazing exit angle

        double m_designAlphaAngle;
        double m_designBetaAngle; // what is this?
        double m_beta0Angle;
        double m_alpha0Angle; // and what are these? equivalent to designAlphaAngle when calculated from other values?
        double m_betaAngle; // calculated if DESIGN_TYPE == DT_ZOFFSET. design beta angle when m_fresnelZOffset is by user
        // in rad as well
        double m_chi;
        double m_distanceToPreceedingElement;

        double m_longRadius;
        double m_shortRadius;

        double m_frenselZOffset; // parameter given by user in DESIGN_TYPE==DT_ZOFFSET
        double m_calcFresnelZOffset; // calculated if DESIGN_TYPE==DT_BETA
        // input and exit vector lengths
        double m_sagittalEntranceArmLength;
        double m_sagittalExitArmLength;
        double m_meriodicalEntranceArmLength;
        double m_R1ArmLength;
        double m_R2ArmLength; // what is this now??

        double m_designEnergy;//Mounting;
        double m_lineDensity;
        double m_orderOfDiffraction;
        double m_designOrderOfDiffraction;
        double m_designEnergyMounting; // derived from source?
        double m_a; // calculated from line density, order of diffracion and design energy mounting

        double m_radius;

        enum FULL_EFFICIENCY { FE_OFF, FE_ON };
        FULL_EFFICIENCY m_fullEfficiency; // todo
        enum DIFFRACTION_METHOD { DM_TWOGRATINGS, DM_2D };
        DIFFRACTION_METHOD m_diffractionMethod; // todo
        enum ELEMENT_OFFSETZ { EZ_MANUAL, EZ_BEAMDIVERGENCE };
        ELEMENT_OFFSETZ m_elementOffsetZType; // TODO 
        double m_elementOffsetZ; // if given manually
        double m_elementOffsetZCalc; // if not given but has to be calculated by beam divergence
    };

} // namespace RAY
