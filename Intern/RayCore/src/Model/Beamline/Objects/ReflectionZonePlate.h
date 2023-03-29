#pragma once

#include <optional>

#include "Model/Beamline/OpticalElement.h"

namespace RAYX {

enum class CurvatureType {
    Plane,
    Toroidal,
    Spherical
};  // influences surface paramters for eg quadric function or (if torus)
    // newton iteration
enum class DesignType { ZOffset, Beta };                  // TODO(Jannis): remove (default is Beta)
enum class ElementOffsetType { Manual, Beamdivergence };  // TODO(Jannis): remove (included in world coordinates)
enum class RZPType { Elliptical, Meriodional };

class RAYX_API ReflectionZonePlate : public OpticalElement {
  public:
    ReflectionZonePlate(const DesignObject&);

    // for calculating incidence and exit angle from user parameters
    void calcAlpha();
    void calcBeta2();
    void Illumination();
    double calcZOffset();
    double calcDz00();
    // beta and fresnelZOffset are dependant of each other, depending on the
    // given DesignType one is calculated from the other in calcDz00
    void calcFresnelZOffset();  // from beta
    void calcBeta();            // from fresneloffset
    // calculates some more arms from the ones that are given by the user
    // (sagittal and meridional entrance and exit arms). Somehow connected to
    // alpha0Angle and beta0Angle
    void VectorR1Center();
    void VectorR2Center();
    double rzpLineDensityDZ(glm::dvec3 intersection, glm::dvec3 normal, const double WL);

    // order of diffraction can be derived from the design order of diffraction
    // (from user parameter)
    void calcDesignOrderOfDiffraction(const double designOrderOfDiffraction);

    // GETTER
    Rad getIncidenceAngle() const;
    Rad getDiffractionAngle() const;
    Rad getDesignAlphaAngle() const;
    Rad getDesignBetaAngle() const;

    GratingMount getGratingMount() const;

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

  private:
    // User Parameter,

    // the parameters are default zero initialized, as the constructors don't
    // initialize everything and otherwise uninitialized data would be printed
    // in printInfo().
    double m_incidenceMainBeamLength = 0;
    double m_meridionalDistance = 0;
    double m_meridionalDivergence = 0;
    double m_fresnelZOffset = 0;      ///< parameter given by user in
                                      ///< DesignType==DesignType::ZOffset
    double m_calcFresnelZOffset = 0;  ///< calculated if DesignType==DesignType::Beta
    double m_zOff = 0;                ///< zoffset dependant on elementOffSetType
    double m_illuminationZ = 0;
    double m_designEnergyMounting = 0;  //? derived from source?
    // TODO(Theresa): include in world coordinates:
    double m_elementOffsetZ = 0;  // given manually or has to be calculated by
                                  // beam divergence

    /** m_designAlphaAngle (incidence beam angle for which rzp is designed)
     *  and m_designBetaAngle (outgoing beam angle for which rzp is designed)
     *  are used to calculate line density. This is important for correctly
     *  simulating light of different wavelengths.
     */
    Rad m_designAlphaAngle = Rad(0);
    /** @see m_designAlphaAngle */
    Rad m_designBetaAngle = Rad(0);
    double m_designOrderOfDiffraction = 0;
    /** Wavelength for which m_designBetaAngle
     *  is correct.
     *  @see m_designAlphaAngle
     */
    // TODO(Jannis): decide designEnergy vs designWavelength
    double m_designEnergy = 0;
    double m_designWavelength = 0;
    double m_designSagittalEntranceArmLength = 0;
    double m_designSagittalExitArmLength = 0;
    double m_designMeridionalEntranceArmLength = 0;
    double m_designMeridionalExitArmLength = 0;

    // needed for calculating incidence and exit angle, this calculation is
    // moved to somewhere else since the angles are only needed to get the world
    // coordinates of the element
    Rad m_beta0Angle = Rad(0);
    Rad m_alpha0Angle = Rad(0);
    Rad m_betaAngle = Rad(0);
    Rad m_grazingIncidenceAngle = Rad(0);
    double m_R1ArmLength = 0;
    double m_R2ArmLength = 0;
    double m_lineDensity = 0;

    GratingMount m_gratingMount = GratingMount::Deviation;

    // TODO(Jannis): move
    CurvatureType m_curvatureType = CurvatureType::Plane;

    // set to default for now
    // int m_designType;
    int m_derivationMethod = 0;
    // int m_elementOffsetType;
    DesignType m_designType = DesignType::ZOffset;
    ElementOffsetType m_elementOffsetType = ElementOffsetType::Manual;
    // TODO(Jannis): have a look at this later
    ImageType m_imageType = ImageType::Point2Point;
    RZPType m_rzpType = RZPType::Elliptical;
    double m_additionalOrder = 0;

    // Surface Parameter (also in Toroid)
    double m_longRadius = 0;
    double m_shortRadius = 0;

    // Simulation Parameter
    double m_orderOfDiffraction = 0;
};

}  // namespace RAYX
