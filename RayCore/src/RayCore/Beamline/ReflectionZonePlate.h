#pragma once
#include "Quadric.h"

namespace RAY
{

    class RAY_API ReflectionZonePlate : public Quadric {

    public:
        
        ReflectionZonePlate(const char* name, int mount, int curvatureType, double width, double height, double deviation, double incidenceAngle, double azimuthal, double distanceToPreceedingElement, double designEnergy, double sourceEnergy, double orderOfDiffraction, double designOrderOfDiffraction, double dAlpha, double dBeta, double mEntrance, double mExit, double sEntrance, double sExit, double shortRadius, double longRadius, double elementOffsetZ, std::vector<double> misalignmentParams, Quadric* previous);
        ReflectionZonePlate();
        ~ReflectionZonePlate();

        void calcDesignAlphaAngle(double deviation, double normalIncidence);
        void calcAlpha2();
        void calcBeta2();
        void Illumination();
        double calcZOffset();
        double calcDz00();
        // beta and fresnelZOffset are dependant of each other, depending on the given DESIGN_TYPE one is calculated from the other in calcDz00
        void calcFresnelZOffset();
        void calcBeta();
        // calculates some more arms from the ones that are given by the user (sagittal and meridional entrance and exit arms). Somehow connected to alpha0Angle and beta0Angle
        void VectorR1Center();
        void VectorR2Center();

        void calcDesignOrderOfDiffraction(double designOrderOfDiffraction);
        void focus(double angle);
        double rzpLineDensityDZ(double X, double Y, double Z, double FX, double FY, double FZ, double WL);

        double getWidth();
        double getHeight();
        double getAlpha();
        double getBeta();
        double getDesignAlphaAngle();
        double getDesignBetaAngle();

        double getGratingMount();

        double getLongRadius();
        double getShortRadius();

        double getFresnelZOffset();
        double getCalcFresnelZOffset();
        // input and exit vector lengths
        double getSagittalEntranceArmLength();
        double getSagittalExitArmLength();
        double getMeridionalEntranceArmLength();
        double getMeridionalExitArmLength();
        double getR1ArmLength();
        double getR2ArmLength(); // what is this now??

        double getDesignEnergy();//Mounting;
        double getWaveLength();
        double getLineDensity();
        double getOrderOfDiffraction();
        double getDesignOrderOfDiffraction();
        double getDesignEnergyMounting(); // derived from source?

    private:
        double m_totalWidth;
        double m_totalHeight;
        // angles in rad and normal angles incidence
        double m_alpha; // grazing incidence angle
        double m_beta; // == alpha? grazing exit angle

        double m_incidenceMainBeamLength;
        double m_meridionalDistance;
        double m_meridionalDivergence;
        double m_designAlphaAngle;
        double m_designBetaAngle; // what is this?

        double m_beta0Angle;
        double m_alpha0Angle; // and what are these? equivalent to designAlphaAngle when calculated from other values?
        double m_betaAngle; // calculated if DESIGN_TYPE == DT_ZOFFSET. design beta angle when m_fresnelZOffset is by user
        // in rad as well
        double m_chi;
        double m_grazingIncidenceAngle;
        double m_distanceToPreceedingElement;

        double m_longRadius;
        double m_shortRadius;

        double m_frenselZOffset; // parameter given by user in DESIGN_TYPE==DT_ZOFFSET
        double m_calcFresnelZOffset; // calculated if DESIGN_TYPE==DT_BETA
        // input and exit vector lengths
        double m_sagittalEntranceArmLength;
        double m_sagittalExitArmLength;
        double m_meridionalEntranceArmLength;
        double m_meridionalExitArmLength;
        double m_R1ArmLength;
        double m_R2ArmLength; // what is this??

        double m_zOff; // zoffset dependant on elementOffSetType
        double m_illuminationZ;

        double m_designEnergy;
        double m_wavelength; // hvlam(energy)
        double m_lineDensity;
        double m_orderOfDiffraction;
        double m_designOrderOfDiffraction;
        double m_designEnergyMounting; // derived from source?
        double m_a; // calculated from line density, order of diffracion and design energy mounting


        enum FULL_EFFICIENCY { FE_OFF, FE_ON };
        FULL_EFFICIENCY m_fullEfficiency; // todo
        enum DIFFRACTION_METHOD { DM_TWOGRATINGS, DM_2D };
        DIFFRACTION_METHOD m_diffractionMethod; // todo
        double m_elementOffsetZ; // if given manually
        double m_elementOffsetZCalc; // if not given but has to be calculated by beam divergence
    };

} // namespace RAY
