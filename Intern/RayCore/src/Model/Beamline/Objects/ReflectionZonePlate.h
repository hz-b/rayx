#pragma once
#include <Data/xml.h>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"
#include "Model/Surface/Toroid.h"

namespace RAYX {

class RAYX_API ReflectionZonePlate : public OpticalElement {
  public:
    // shortened constructor that assumes that the angles are already calculated
    // and the position and orientation in world coordinates is already derived
    ReflectionZonePlate(
        const char* name, Geometry::GeometricalShape geometricalShape,
        const int curvatureType, const double width, const double height,
        const double azimuthalAngle, const glm::dvec4 position,
        const glm::dmat4x4 orientation, const double designEnergy,
        const double orderOfDiffraction, const double designOrderOfDiffraction,
        const double dAlpha, const double dBeta, const double mEntrance,
        const double mExit, const double sEntrance, const double sExit,
        const double shortRadius, const double longRadius,
        const int additionalZeroOrder, const double fresnelZOffset,
        const std::vector<double> slopeError);

    // ! temporary constructor for trapezoid (10/11/2021)
    ReflectionZonePlate(
        const char* name, Geometry::GeometricalShape geometricalShape,
        const int curvatureType, const double widthA, const double widthB,
        const double height, const double azimuthalAngle,
        const glm::dvec4 position, const glm::dmat4x4 orientation,
        const double designEnergy, const double orderOfDiffraction,
        const double designOrderOfDiffraction, const double dAlpha,
        const double dBeta, const double mEntrance, const double mExit,
        const double sEntrance, const double sExit, const double shortRadius,
        const double longRadius, const int additionalZeroOrder,
        const double fresnelZOffset, const std::vector<double> slopeError);
    ReflectionZonePlate();
    ~ReflectionZonePlate();

    static std::shared_ptr<ReflectionZonePlate> createFromXML(
        rapidxml::xml_node<>*, const std::vector<xml::Group>& group_context);

    // for calculating incidence and exit angle from user parameters
    void calcAlpha();
    void calcBeta2();
    void Illumination();
    double calcZOffset();
    double calcDz00();
    // beta and fresnelZOffset are dependant of each other, depending on the
    // given DESIGN_TYPE one is calculated from the other in calcDz00
    void calcFresnelZOffset();  // from beta
    void calcBeta();            // from fresneloffset
    // calculates some more arms from the ones that are given by the user
    // (sagittal and meridional entrance and exit arms). Somehow connected to
    // alpha0Angle and beta0Angle
    void VectorR1Center();
    void VectorR2Center();
    double rzpLineDensityDZ(glm::dvec3 intersection, glm::dvec3 normal,
                            const double WL);

    // order of diffraction can be derived from the design order of diffraction
    // (from user parameter)
    void calcDesignOrderOfDiffraction(const double designOrderOfDiffraction);

    // GETTER
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
    double getR2ArmLength() const;

    double getDesignEnergy() const;  ///< Mounting;
    double getWaveLength() const;
    double getLineDensity() const;
    double getOrderOfDiffraction() const;
    double getDesignOrderOfDiffraction() const;
    double getDesignEnergyMounting() const;  // derived from source?
    void printInfo() const;

    enum CURVATURE_TYPE {
        CT_PLANE,
        CT_SPHERICAL,
        CT_TOROIDAL
    };  // influences surface paramters for eg quadric function or (if torus)
        // newton iteration
    enum DESIGN_TYPE {
        DT_ZOFFSET,
        DT_BETA
    };  // TODO(Jannis): remove (default is DT_BETA)
    enum ELEMENTOFFSET_TYPE {
        EZ_MANUAL,
        EZ_BEAMDIVERGENCE
    };  // TODO(Jannis): remove (included in world coordinates)
    enum IMAGE_TYPE { IT_POINT2POINT, IT_ASTIGMATIC2ASTIGMATIC };
    enum RZP_TYPE { RT_ELLIPTICAL, RT_MERIODIONAL };

  private:
    // User Parameter,
    double m_incidenceMainBeamLength;
    double m_meridionalDistance;
    double m_meridionalDivergence;
    double m_fresnelZOffset;      ///< parameter given by user in
                                  ///< DESIGN_TYPE==DT_ZOFFSET
    double m_calcFresnelZOffset;  ///< calculated if DESIGN_TYPE==DT_BETA
    double m_zOff;                ///< zoffset dependant on elementOffSetType
    double m_illuminationZ;
    double m_designEnergyMounting;  //? derived from source?
    // TODO(Theresa): include in world coordinates:
    double m_elementOffsetZ;  // given manually or has to be calculated by beam
                              // divergence

    /** m_designAlphaAngle (incidence beam angle for which rzp is designed)
     *  and m_designBetaAngle (outgoing beam angle for which rzp is designed)
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
    // TODO(Jannis): decide designEnergy vs designWavelength
    double m_designEnergy;
    double m_designWavelength;
    double m_designSagittalEntranceArmLength;
    double m_designSagittalExitArmLength;
    double m_designMeridionalEntranceArmLength;
    double m_designMeridionalExitArmLength;

    // needed for calculating incidence and exit angle, this calculation is
    // moved to somewhere else since the angles are only needed to get the world
    // coordinates of the element
    double m_beta0Angle;
    double m_alpha0Angle;
    double m_betaAngle;
    double m_grazingIncidenceAngle;
    double m_R1ArmLength;
    double m_R2ArmLength;
    double m_lineDensity;

    GRATING_MOUNT m_gratingMount;

    // TODO(Jannis): move
    CURVATURE_TYPE m_curvatureType;

    // set to default for now
    // int m_designType;
    int m_derivationMethod;
    // int m_elementOffsetType;
    DESIGN_TYPE m_designType;
    ELEMENTOFFSET_TYPE m_elementOffsetType;
    // TODO(Jannis): have a look at this later
    IMAGE_TYPE m_imageType;
    RZP_TYPE m_rzpType;
    double m_additionalOrder;

    // Surface Parameter (also in Toroid)
    double m_longRadius;
    double m_shortRadius;

    // Simulation Parameter
    double m_orderOfDiffraction;
};

}  // namespace RAYX
