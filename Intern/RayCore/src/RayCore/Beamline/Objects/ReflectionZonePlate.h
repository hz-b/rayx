#pragma once
#include "Surface/Quadric.h"
#include "Beamline/OpticalElement.h"

namespace RAYX
{

    class RAYX_API ReflectionZonePlate : public OpticalElement {

    public:

        ReflectionZonePlate(const char* name, const int geometricShape, const int mount, const int curvatureType, const int designType, const int elementOffsetType, const double width, const double height, const double deviation, const double incidenceAngle, const double azimuthal, const double distanceToPreceedingElement, const double designEnergy, const double sourceEnergy, const double orderOfDiffraction, const double designOrderOfDiffraction, const double dAlpha, const double dBeta, const double mEntrance, const double mExit, const double sEntrance, const double sExit, const double shortRadius, const double longRadius, const int additional_zero_order, const double elementOffsetZ, const double fresnelZOffset, const double beta, const std::vector<double> misalignmentParams, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous, bool global);
        ReflectionZonePlate();
        ~ReflectionZonePlate();

        void calcDesignAlphaAngle(const double deviation, const double normalIncidence);
        void calcAlpha();
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

        void calcDesignOrderOfDiffraction(const double designOrderOfDiffraction);
        void focus(double angle);
        double rzpLineDensityDZ(const double X, const double Y, const double Z, const double FX, const double FY, const double FZ, const double WL);

        double getIncidenceAngle() const;
        double getDiffractionAngle() const;
        double getDesignAlphaAngle() const;
        double getDesignBetaAngle() const;

        double getGratingMount() const;

        double getLongRadius() const;
        double getShortRadius() const;

        double getFresnelZOffset() const;
        double getCalcFresnelZOffset() const;
        // input and exit vector lengths
        double getSagittalEntranceArmLength() const;
        double getSagittalExitArmLength() const;
        double getMeridionalEntranceArmLength() const;
        double getMeridionalExitArmLength() const;
        double getR1ArmLength() const;
        double getR2ArmLength() const; // what is this now??

        double getDesignEnergy() const; ///< Mounting;
        double getWaveLength() const;
        double getLineDensity() const;
        double getOrderOfDiffraction() const;
        double getDesignOrderOfDiffraction() const;
        double getDesignEnergyMounting() const; // derived from source?
        void printInfo() const;

    private:
        // User Parameter
        double m_incidenceMainBeamLength;
        double m_meridionalDistance;
        double m_meridionalDivergence;
        double m_fresnelZOffset; ///< parameter given by user in DESIGN_TYPE==DT_ZOFFSET
        double m_calcFresnelZOffset; ///< calculated if DESIGN_TYPE==DT_BETA
        double m_zOff; ///< zoffset dependant on elementOffSetType
        double m_illuminationZ;
        double m_designEnergyMounting; //? derived from source?
        double m_elementOffsetZ; // if given manually
        double m_elementOffsetZCalc; // if not given but has to be calculated by beam divergence

        /** m_designAlphaAngle (incidence beam angle for which formula is designed)
         *  and m_designBetaAngle (outgoing beam angle for which formula is designed)
         *  are used to calculate line density. This is important for correctly
         *  simulating light of different wavelengths.
         */
        double m_designAlphaAngle;
        /** @see m_designAlphaAngle */
        double m_designBetaAngle;
        double m_designOrderOfDiffraction;
        /** Wavelength for which m_designBetaAngle
         *  is correct.
         *  @see m_designAlphaAngle
         */
        double m_designEnergy; // TODO(Jannis): decide designEnergy vs designWavelength
        // TODO(Jannis): rename m_design...
        double m_wavelength;
        double m_sagittalEntranceArmLength;
        double m_sagittalExitArmLength;
        double m_meridionalEntranceArmLength;
        double m_meridionalExitArmLength;


        // TODO(Jannis): remove
        double m_beta0Angle;
        double m_alpha0Angle;
        double m_betaAngle;
        double m_grazingIncidenceAngle;
        double m_R1ArmLength;
        double m_R2ArmLength; //? what is this??
        double m_lineDensity;
        double m_a; ///< calculated from line density, order of diffracion and design energy mounting
        enum DIFFRACTION_METHOD { DM_TWOGRATINGS, DM_2D };
        DIFFRACTION_METHOD m_diffractionMethod; // only use 2D
        enum FULL_EFFICIENCY { FE_OFF, FE_ON };
        FULL_EFFICIENCY m_fullEfficiency; // todo

        // TODO(Jannis): move
        enum GEOMETRICAL_SHAPE { GS_RECTANGLE, GS_ELLIPTICAL };
        GEOMETRICAL_SHAPE m_geometricalShape;
        enum GRATING_MOUNT { GM_DEVIATION, GM_INCIDENCE };
        GRATING_MOUNT m_gratingMount;
        enum CURVATURE_TYPE { CT_PLANE, CT_TOROIDAL, CT_SPHERICAL };
        CURVATURE_TYPE m_curvatureType;

        enum DESIGN_TYPE { DT_ZOFFSET, DT_BETA }; //TODO(Jannis): remove (default is DT_BETA)
        DESIGN_TYPE m_designType;
        enum DERIVATION_METHOD { DM_FORMULA, DM_PLOYNOM }; //TODO(Jannis): remove (default is DM_FORMULA)
        DERIVATION_METHOD m_derivationMethod;
        enum ELEMENTOFFSET_TYPE { EZ_MANUAL, EZ_BEAMDIVERGENCE }; //TODO(Jannis): remove (included in world coordinates)
        ELEMENTOFFSET_TYPE m_elementOffsetType;

        // TODO(Jannis): have a look at this later
        enum IMAGE_TYPE { IT_POINT2POINT, IT_ASTIGMATIC2ASTIGMATIC };
        IMAGE_TYPE m_imageType;
        enum RZP_TYPE { RT_ELLIPTICAL, RT_MERIODIONAL };
        RZP_TYPE m_rzpType;
        enum ADDITIONAL_ZERO_ORDER { AO_OFF, AO_ON };
        ADDITIONAL_ZERO_ORDER m_additionalOrder;

        // Surface Parameter (also in Toroid)
        double m_longRadius;
        double m_shortRadius;

        // Simulation Parameter
        double m_orderOfDiffraction;
    };

} // namespace RAYX
