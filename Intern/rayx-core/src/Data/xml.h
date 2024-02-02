#pragma once

#include <array>
#include <filesystem>
#include <glm.hpp>
#include <rapidxml.hpp>
#include <vector>

#include "Material/Material.h"
#include "Shader/Constants.h"
#include "Shader/Element.h"
#include "angle.h"

// the direction of a plane, either XY or XZ. This is only used in the parsing.
// Inside of the shader, every plane-shaped object lies in its XZ plane.
// Per default every element has DesignPlane::XZ, but ImagePlane and Slit have DesignPlane::XY. 
// Thus they need a bit of extra handling to convert them to the shaders XZ plane.
enum class DesignPlane { XY, XZ };

namespace RAYX {

// forward declarations:
class EnergyDistribution;
enum class CentralBeamstop;
enum class CurvatureType;
enum class CylinderDirection;
enum class FigureRotation;
enum class GratingMount;
enum class SourceDist;
enum class GeometricalShape;
enum class ElectronEnergyOrientation;
enum class SourcePulseType;
enum class EnergySpreadUnit;
enum class SigmaType;

// An error in position and orientation that an object might have.
struct Misalignment {
    double m_translationXerror;
    double m_translationYerror;
    double m_translationZerror;
    Rad m_rotationXerror;
    Rad m_rotationYerror;
    Rad m_rotationZerror;
};

namespace xml {

// a representation of a <group>-tag used in parsing.
struct Group {
    glm::dvec4 m_position;
    glm::dmat4x4 m_orientation;
};

// These functions get a `paramname` argument and look for <param
// id="`paramname`">...</param> entries in the XML node to then return it's
// content in the out-argument. They return `false` to express failure.
bool param(const rapidxml::xml_node<>* node, const char* paramname, rapidxml::xml_node<>** out);

// These call the above `param` function, and convert its output to the required type.
bool paramDouble(const rapidxml::xml_node<>* node, const char* paramname, double* out);
bool paramInt(const rapidxml::xml_node<>* node, const char* paramname, int* out);
bool paramStr(const rapidxml::xml_node<>* node, const char* paramname, const char** out);
bool paramDvec3(const rapidxml::xml_node<>* node, const char* paramname, glm::dvec3* out);

// These functions parse more complex parts of beamline objects, and are used by multiple objects in their construction mechanism.
// They itself use the above param* functions.
bool paramMisalignment(const rapidxml::xml_node<>* node, Misalignment* out);
bool paramPositionNoGroup(const rapidxml::xml_node<>* node, glm::dvec4* out);
bool paramOrientationNoGroup(const rapidxml::xml_node<>* node, glm::dmat4x4* out);
bool paramSlopeError(const rapidxml::xml_node<>* node, SlopeError* out);
bool paramVls(const rapidxml::xml_node<>* node, std::array<double, 6>* out);
bool paramEnergyDistribution(const rapidxml::xml_node<>* node, const std::filesystem::path& rmlFile, EnergyDistribution* out);

bool paramElectronEnergyOrientation(const rapidxml::xml_node<>* node, ElectronEnergyOrientation* out);
bool paramSourcePulseType(const rapidxml::xml_node<>* node, SourcePulseType* out);

bool paramPositionAndOrientation(const rapidxml::xml_node<>* node, const std::vector<xml::Group>& group_context, glm::dvec4* out_pos,
                                 glm::dmat4x4* out_ori);
bool paramMaterial(const rapidxml::xml_node<>* node, Material* out);

// node needs to be a <group>-tag, output will be written to `out`.
bool parseGroup(rapidxml::xml_node<>* node, xml::Group* out);

// The Parser gives you utility functions to construct your own objects from RML files.
struct RAYX_API Parser {
    Parser(rapidxml::xml_node<>* node, std::vector<xml::Group> group_context, std::filesystem::path rmlFile);

    const char* name() const;

    const char* type() const;

    // parsers for fundamental types
    double parseDouble(const char* paramname) const;
    int parseInt(const char* paramname) const;
    const char* parseStr(const char* paramname) const;
    glm::dvec3 parseDvec3(const char* paramname) const;

    // parsers for derived parameters
    Misalignment parseMisalignment() const;
    SlopeError parseSlopeError() const;
    std::array<double, 6> parseVls() const;
    EnergyDistribution parseEnergyDistribution() const;
    glm::dvec4 parsePosition() const;
    glm::dmat4x4 parseOrientation() const;
    Material parseMaterial() const;
    Cutout parseCutout(DesignPlane) const;
    QuadricSurface parseQuadricParameters() const;
    CubicSurface parseCubicParameters() const;
    ElectronEnergyOrientation parseElectronEnergyOrientation() const;
    SourcePulseType parseSourcePulseType() const;
    double parseImageType() const;
    double parseAdditionalOrder() const;
    Rad parseAzimuthalAngle() const;

    // Parsers for trivial derived parameters
    // this allows for convenient type-safe access to the corresponding parameters.
    inline int parseNumberRays() const { return parseInt("numberRays"); }
    inline double parseSourceWidth() const { return parseDouble("sourceWidth"); }
    inline double parseSourceHeight() const { return parseDouble("sourceHeight"); }
    inline double parseSourceDepth() const { return parseDouble("sourceDepth"); }
    inline double parseHorDiv() const { return parseDouble("horDiv") / 1000.0; }
    inline double parseVerDiv() const { return parseDouble("verDiv") / 1000.0; }
    inline SourceDist parseSourceWidthDistribution() const { return static_cast<SourceDist>(parseInt("sourceWidthDistribution")); }
    inline SourceDist parseSourceHeightDistribution() const { return static_cast<SourceDist>(parseInt("sourceHeightDistribution")); }
    inline SourceDist parseHorDivDistribution() const { return static_cast<SourceDist>(parseInt("horDivDistribution")); }
    inline SourceDist parseVerDivDistribution() const { return static_cast<SourceDist>(parseInt("verDivDistribution")); }
    inline int parseLinearPol0() const { return parseInt("linearPol_0"); }
    inline int parseLinearPol45() const { return parseInt("linearPol_45"); }
    inline int parseCircularPol() const { return parseInt("circularPol"); }
    inline double parseTotalWidth() const { return parseDouble("totalWidth"); }
    inline double parseTotalLength() const { return parseDouble("totalLength"); }
    inline double parseTotalHeight() const { return parseDouble("totalHeight"); }
    inline Rad parseGrazingIncAngle() const { return Deg(parseDouble("grazingIncAngle")).toRad(); }
    inline double parseArmLength() const { return parseDouble("armLength"); }
    inline double parseEntranceArmLength() const { return parseDouble("entranceArmLength"); }
    inline double parseExitArmLength() const { return parseDouble("exitArmLength"); }
    inline double parseEntranceArmLengthMer() const { return parseDouble("entranceArmLengthMer"); }
    inline double parseExitArmLengthMer() const { return parseDouble("exitArmLengthMer"); }
    inline double parseEntranceArmLengthSag() const { return parseDouble("entranceArmLengthSag"); }
    inline double parseExitArmLengthSag() const { return parseDouble("exitArmLengthSag"); }
    inline CentralBeamstop parseCentralBeamstop() const { return static_cast<CentralBeamstop>(parseInt("centralBeamstop")); }
    inline double parseTotalWidthStop() const { return parseDouble("stopWidth"); }
    inline double parseTotalHeightStop() const { return parseDouble("stopHeight"); }
    inline double parseOpeningWidth() const { return parseDouble("openingWidth"); }
    inline double parseOpeningHeight() const { return parseDouble("openingHeight"); }
    inline double parseOpeningShape() const { return parseDouble("openingShape"); }
    inline GratingMount parseGratingMount() const { return static_cast<GratingMount>(parseInt("gratingMount")); }
    inline double parseRadius() const { return parseDouble("radius"); }
    inline double parseDesignEnergy() const { return parseDouble("designEnergy"); }
    inline double parseLineDensity() const { return parseDouble("lineDensity"); }
    inline double parseOrderDiffraction() const { return parseDouble("orderDiffraction"); }
    inline double parseDesignEnergyMounting() const { return parseDouble("designEnergyMounting"); }

    inline CurvatureType parseCurvatureType() const { return static_cast<CurvatureType>(parseInt("curvatureType")); }

    inline double parseDesignOrderDiffraction() const { return parseDouble("designOrderDiffraction"); }
    inline Rad parseDesignAlphaAngle() const { return Deg(parseDouble("designAlphaAngle")).toRad(); }
    inline Rad parseDesignBetaAngle() const { return Deg(parseDouble("designBetaAngle")).toRad(); }
    inline double parseShortRadius() const { return parseDouble("shortRadius"); }
    inline double parseLongRadius() const { return parseDouble("longRadius"); }
    inline double parseFresnelZOffset() const { return parseDouble("FresnelZOffset"); }
    inline CylinderDirection parseBendingRadius() const { return static_cast<CylinderDirection>(parseInt("bendingRadius")); }
    inline double parseBendingRadiusDouble() const { return parseDouble("bendingRadius"); }
    inline double parseParameterA11() const { return parseDouble("parameter_a11"); }
    inline FigureRotation parseFigureRotation() const { return static_cast<FigureRotation>(parseInt("figureRotation")); }
    // TODO: Are values stored as 0.0 if set to AUTO?[RAY-UI]
    inline Rad parseDesignGrazingIncAngle() const { return Deg(parseDouble("designGrazingIncAngle")).toRad(); }
    inline double parseLongHalfAxisA() const { return parseDouble("longHalfAxisA"); }
    inline double parseShortHalfAxisB() const { return parseDouble("shortHalfAxisB"); }
    // if old ray ui file, need to recalculate position and orientation because
    // those in rml file are wrong. not necessary when our recalculated position
    // and orientation is stored
    inline double parseDistancePreceding() const { return parseDouble("distancePreceding"); }
    inline int parseMisalignmentCoordinateSystem() const { return parseInt("misalignmentCoordinateSystem"); }
    inline double parseVerEbeamDivergence() const { return parseDouble("verEbeamDiv"); }
    inline double parseElectronEnergy() const { return parseDouble("electronEnergy"); }
    inline int parseAlignmentError() const { return parseInt("alignmentError"); }
    inline double parsePhotonFlux() const { return parseDouble("photonFlux"); }
    inline double parsePhotonEnergy() const { return parseDouble("photonEnergy"); }
    inline double parseEnergySpread() const { return parseDouble("energySpread"); }
    inline EnergySpreadUnit parseEnergySpreadUnit() const { return static_cast<EnergySpreadUnit>(parseInt("energySpreadUnit")); }

    inline int parseNumOfEquidistantCircles() const { return parseDouble("numberCircles"); }
    inline Rad parseMaxOpeningAngle() const { return parseDouble("maximumOpeningAngle") / 1000.0; }
    inline Rad parseMinOpeningAngle() const { return parseDouble("minimumOpeningAngle") / 1000.0; }
    inline Rad parseDeltaOpeningAngle() const { return parseDouble("deltaOpeningAngle") / 1000.0; }

    inline double parseParameterP() const { return parseDouble("parameter_P"); }
    inline double parseParameterPType() const { return parseDouble("parameter_P_type"); }

    inline SigmaType parseSigmaType() const {return static_cast<SigmaType>(parseInt("sigmaType"));}
    inline double parseUndulatorLength() const {return parseDouble("undulatorLength"); }
    inline double parseElectronSigmaX() const {return parseDouble("electronSigmaX"); }
    inline double parseElectronSigmaXs() const {return parseDouble("electronSigmaXs"); }
    inline double parseElectronSigmaY() const {return parseDouble("electronSigmaY"); }
    inline double parseElectronSigmaYs() const {return parseDouble("electronSigmaYs"); }

    // the XML node of the object you intend to parse.
    rapidxml::xml_node<>* node;

    // The stack of groups which contain this object.
    std::vector<xml::Group> group_context;

    // the RML file we are currently parsing.
    std::filesystem::path rmlFile;
};

}  // namespace xml

/// The DesignObject contains the design parameters. But the actual OpticalElements does not contain them.
/// The DesignObject is utilized to construct OpticalElements from it's parameters.
///
/// For now objects can only be created from XML, but later on - when the GUI is around,
/// we might want to add an abstract superclass DesignObject.
/// DesignObject would have child classes xml::Parser and gui::ObjectBuilder or something.
/// This superclass DesignObject could mostly keep the API of the xml::Parser, to make the transition trivial.
using DesignObject = xml::Parser;

}  // namespace RAYX
